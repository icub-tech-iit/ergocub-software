/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <vector>

#include <yarp/dev/IControlMode.h>
#include <yarp/dev/IEncoders.h>
#include <yarp/dev/IPWMControl.h>
#include <yarp/dev/IPidControl.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Time.h>

using namespace yarp::os;
using namespace yarp::dev;

struct DataExperiment {
  double t;
  double pwm_set;
  double pwm_read;
  double enc;
  double pid_out;
};

IPositionControl *iPos{nullptr};
IPWMControl *iPwm{nullptr};
IControlMode *iCm{nullptr};
IEncoders *iEnc{nullptr};
IPidControl *iPid{nullptr};

double t0 = 0;

std::vector<DataExperiment> data_vec;

class PWMThread : public PeriodicThread {
private:
  ResourceFinder rf_;
  bool done_h = false;
  bool done_l = false;
  DataExperiment data;
  uint8_t j = 0;
  double pwm_ = 0;
  double thr_min = 0;
  double thr_max = 0;
  double v = 0;

public:
  PWMThread(double ts, ResourceFinder &rf, uint8_t joint_id)
      : PeriodicThread(ts), rf_(rf), j(joint_id) {}

  bool threadInit() {
    auto limits = rf_.find("limits").asList();
    thr_min = limits->get(0).asFloat64();
    thr_max = limits->get(1).asFloat64();
    return true;
  }

  void run() {
    data.t = Time::now() - t0;
    data.pwm_set = pwm_;
    iEnc->getEncoder(j, &data.enc);
    iPid->getPidOutput(VOCAB_PIDTYPE_POSITION, j, &data.pid_out);
    iPwm->getDutyCycle(j, &data.pwm_read);

    if (data.enc >= thr_max && pwm_ > .1) {
      done_h = true;
      pwm_ = -pwm_;
      iPwm->setRefDutyCycle(j, pwm_);

    } else if (data.enc <= thr_min && pwm_ < -0.1) {
      done_l = true;
      pwm_ = -pwm_;
      iPwm->setRefDutyCycle(j, pwm_);
    }
    data_vec.push_back(std::move(data));
  }

  bool isDone() { return done_h && done_l; }

  void setInputs(double pwm) {
    pwm_ = pwm;
    iPwm->setRefDutyCycle(j, pwm_);
  }

  void resetFlags() {
    done_h = false;
    done_l = false;
  }
};

bool resetPosition(uint8_t j) {
  iPwm->setRefDutyCycle(j, 0.0);
  iCm->setControlMode(j, VOCAB_CM_POSITION);
  iPos->setRefSpeed(j, 25.0);
  iPos->setRefAcceleration(j, std::numeric_limits<double>::max());
  iPos->positionMove(j, 0.);
  bool done = false;

  while (!done) {
    iPos->checkMotionDone(j, &done);
    Time::yield();
  }

  return done;
}

int main(int argc, char *argv[]) {
  Network yarp;

  if (!yarp.checkNetwork()) {
    yError() << "Unable to find YARP server!";
    return EXIT_FAILURE;
  }

  ResourceFinder rf;
  rf.configure(argc, argv);
  rf.setQuiet(true);

  auto port = rf.check("port", Value("/ergocub/left_arm")).asString();
  auto cycles = rf.check("cycles", Value(10)).asInt32();
  auto timeout = rf.check("timeout", Value(5.0)).asFloat64();
  auto filename = rf.check("filename", Value("output.csv")).asString();
  auto pwm_list = rf.find("pwm-values").asList();
  auto Ts = rf.check("period", Value(0.001)).asFloat64();
  auto joint_id = rf.check("joint-id", Value(12)).asInt8();

  Property conf;
  conf.put("device", "remote_controlboard");
  conf.put("remote", port);
  conf.put("local", "/logger");

  PolyDriver m_driver;
  if (!m_driver.open(conf)) {
    yError() << "Failed to open" << port;
    return EXIT_FAILURE;
  }

  if (!(m_driver.view(iPos) && m_driver.view(iPwm) && m_driver.view(iCm) &&
        m_driver.view(iEnc) && m_driver.view(iPid))) {
    m_driver.close();
    yError() << "Failed to view interfaces";
    return EXIT_FAILURE;
  }

  std::vector<double> pwm_values;

  for (size_t i = 0; i < pwm_list->size(); ++i) {
    pwm_values.push_back(pwm_list->get(i).asFloat64());
  }

  std::unique_ptr<PWMThread> pwm_thr =
      std::make_unique<PWMThread>(Ts, rf, joint_id);

  std::ofstream file;
  file.open(filename);
  data_vec.reserve(100000);

  resetPosition(joint_id);

  iCm->setControlMode(joint_id, VOCAB_CM_PWM);
  iPwm->setRefDutyCycle(joint_id, 0.0);

  t0 = Time::now();
  pwm_thr->start();

  for (auto pwm : pwm_values) {
    for (int i = 0; i < cycles; i++) {
      pwm_thr->setInputs(pwm);
      yDebug() << "(pwm, cycle n): (" << pwm << i << ")";
      while (true) {
        if (pwm_thr->isDone() || pwm_thr->getIterations() > (timeout / Ts)) {
          pwm_thr->resetFlags();
          pwm_thr->resetStat();
          break;
        }
        Time::delay(0.1);
      }
    }
  }

  pwm_thr->stop();

  resetPosition(joint_id);

  for (const auto &d : data_vec) {
    file << d.t << "," << d.pwm_read << "," << d.enc << "," << d.pid_out << ","
         << d.pwm_set << std::endl;
  }

  m_driver.close();
  file.close();

  yInfo() << "Done.";
  return EXIT_SUCCESS;
}
