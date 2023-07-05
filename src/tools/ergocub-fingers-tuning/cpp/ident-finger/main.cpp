/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include <cstdlib>
#include <cmath>
#include <limits>
#include <vector>
#include <fstream>

#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Value.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/IPidControl.h>
#include <yarp/dev/IPWMControl.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/dev/IEncoders.h>

using namespace yarp::os;
using namespace yarp::dev;

struct DataExperiment {
    double t;
    double pwm_set;
    double pwm_read;
    double enc;
    double pid_out;
};

int main(int argc, char * argv[])
{
    Network yarp;
    if (!yarp.checkNetwork()) {
        yError() << "Unable to find YARP server!";
        return EXIT_FAILURE;
    }

    ResourceFinder rf;
    rf.configure(argc, argv);

    auto robot = rf.check("robot", Value("ergocub")).asString();
    auto part = rf.check("part", Value("left_arm")).asString();
    auto joint_id = rf.check("joint-id", Value(1)).asInt8();
    auto cycles = rf.check("cycles", Value(10)).asInt32();
    auto threshold = rf.check("threshold", Value(20)).asInt32();
    auto timeout = rf.check("timeout", Value(5000)).asInt32();
    auto filename = rf.check("filename", Value("output.csv")).asString();

    std::string port = "/" + robot + "/" + part;

    PolyDriver m_driver;
    IPositionControl* iPos{ nullptr };
    IPWMControl* iPwm{ nullptr };
    IControlMode* iCm{ nullptr };
    IEncoders* iEnc{ nullptr };
    IPidControl* iPid{ nullptr };
    std::vector<double> pwm_values{10, 25, 50, 75};
    std::ofstream file;

    file.open(filename);

    std::vector<DataExperiment> data_vec;
    data_vec.reserve(100000);

    Property conf;
    conf.put("device", "remote_controlboard");
    conf.put("remote", port);
    conf.put("local", "/logger");
    if (!m_driver.open(conf)) {
        yError() << "Failed to open";
        return EXIT_FAILURE;
    }
    if (!(m_driver.view(iPos) && m_driver.view(iPwm) &&
          m_driver.view(iCm) && m_driver.view(iEnc) && m_driver.view(iPid))) {
        m_driver.close();
        yError() << "Failed to view interfaces";
        return EXIT_FAILURE;
    }

    // set control mode to position
    iCm->setControlMode(joint_id, VOCAB_CM_POSITION);
    // Homing
    iPos->setRefSpeed(joint_id, 5.);
    iPos->setRefAcceleration(joint_id, std::numeric_limits<double>::max());
    iPos->positionMove(joint_id, 0.);
    auto done{ false };
    while (!done) {
        iPos->checkMotionDone(joint_id, &done);
        Time::yield();
    }

    DataExperiment data;
    auto t0 = Time::now();
    auto t1{t0};

    // set control mode to pwm
        iCm->setControlMode(joint_id, VOCAB_CM_PWM);
        iPwm->setRefDutyCycle(joint_id,0.0);

    // drives the joint between +20/-20 degrees for each vlaue of PWM stored in pwm_values
    for(auto pwm : pwm_values) {
        uint t = 0;
        for(int i=0; i<cycles*2; i++) {
        auto done_h = false;
        auto done_l = false;
        done = false;
        pwm = pwm * -1;
        iPwm->setRefDutyCycle(joint_id,pwm);

        while((!done_h || !done_l) && t < timeout) {
            data.t = Time::now() - t0;
            data.pwm_set = pwm;
            iEnc->getEncoder(joint_id, &data.enc);
            iPid->getPidOutput(VOCAB_PIDTYPE_POSITION, joint_id, &data.pid_out);
            iPwm->getDutyCycle(joint_id, &data.pwm_read);


            // if the joint exceeds +20/-20 degrees invert the PWM
            if (Time::now() - t1 >= .001) {
        //       yDebug() << data.pwm << " " << data.enc;
                t ++;
                if(data.enc > threshold) {
                    if(!done_h) {
                        pwm = pwm * -1;
                        iPwm->setRefDutyCycle(joint_id, pwm);
                        done_h = true;

                    }
                }
                else if(data.enc < threshold * -1) {
                    if(!done_l){
                        pwm = pwm * -1;
                        iPwm->setRefDutyCycle(joint_id, pwm);
                        done_l = true;
                    }
                }
                data_vec.push_back(std::move(data));
                t1 = Time::now();
            }
        }
        }

    }
          // sets PWM to zero
        iPwm->setRefDutyCycle(joint_id,0.0);
        // sets control mode to position
        iCm->setControlMode(joint_id, VOCAB_CM_POSITION);
        // Homing
        iPos->setRefSpeed(joint_id, 5.);
        iPos->setRefAcceleration(joint_id, std::numeric_limits<double>::max());
        iPos->positionMove(joint_id, 0.);
        while (!done) {
                iPos->checkMotionDone(joint_id, &done);
                Time::yield();
            }
    // writes the log
    for (const auto& d : data_vec) {
            file << d.t << "," << d.pwm_read << ","
                << d.enc << "," << d.pid_out << "," << d.pwm_set << std::endl;
            }

    m_driver.close();
    file.close();

    yInfo()<<"Main returning...";
    return EXIT_SUCCESS;
}
