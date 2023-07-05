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
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/IPidControl.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/dev/IEncoders.h>

using namespace yarp::os;
using namespace yarp::dev;

struct DataExperiment {
    double t;
    double pid_ref;
    double pid_out;
    double enc;
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
    auto joint_id = rf.check("joint-id", Value(10)).asInt32();
    auto set_point = rf.check("set-point", Value(-60)).asFloat64();
    auto cycles = rf.check("cycles", Value(2)).asInt32();
    auto T = rf.check("T", Value(2.)).asFloat64();
    auto filename = rf.check("filename", Value("output.csv")).asString();

    PolyDriver m_driver;
    IPositionControl* iPos{ nullptr };
    IPidControl* iPid{ nullptr };
    IControlMode* iCm{ nullptr };
    IEncoders* iEnc{ nullptr };
    std::ofstream file;

    file.open(filename);

    std::string port = "/" + robot + "/" + part;
    std::vector<DataExperiment> data_vec;
    data_vec.reserve(1000);
    std::vector<double> velocities{10, 25, 50, 60};

    Property conf;
    conf.put("device", "remote_controlboard");
    conf.put("remote", port);
    conf.put("local", "/logger");
    if (!m_driver.open(conf)) {
        yError() << "Failed to open";
        return EXIT_FAILURE;
    }

    if (!(m_driver.view(iPos) && m_driver.view(iPid) &&
          m_driver.view(iCm) && m_driver.view(iEnc))) {
        m_driver.close();
        yError() << "Failed to view interfaces";
        return EXIT_FAILURE;
    }
    iCm->setControlMode(joint_id, VOCAB_CM_POSITION);

    auto t0 = Time::now();
    // Homing
    iPos->setRefSpeed(joint_id, 40.);
    iPos->setRefAcceleration(joint_id, std::numeric_limits<double>::max());
    iPos->positionMove(joint_id, 0.);
    auto done{ false };
    while (!done) {
        iPos->checkMotionDone(joint_id, &done);
        Time::yield();
    }

    for(auto & ref_speed : velocities)
    {
        auto t1 = Time::now();
        for(int i=0; i<cycles; i++) {

            auto set_point_to_apply = set_point;

            if(i == 1)
                set_point_to_apply = 6;

            DataExperiment data;
            iEnc->getEncoder(joint_id, &data.enc);
            iPos->setRefSpeed(joint_id, ref_speed);
            iPos->positionMove(joint_id, set_point_to_apply);

            //auto t0 = Time::now();
            done = false;

            yInfo() << "ref speed "<< ref_speed << " cycle " << i << " sp " << set_point_to_apply;

            while (!done) {
                data.t = Time::now() - t0;
                iPid->getPidReference(VOCAB_PIDTYPE_POSITION, joint_id, &data.pid_ref);
                iPid->getPidOutput(VOCAB_PIDTYPE_POSITION, joint_id, &data.pid_out);
                iEnc->getEncoder(joint_id, &data.enc);

                if (Time::now() - t1 >= 0.1) {
                    iPos->checkMotionDone(joint_id, &done);
                    t1 = Time::now();
                data_vec.push_back(std::move(data));

                }
                Time::yield();
            }
        }
    }

    for (const auto& d : data_vec) {
        file << d.t << "," << d.pid_ref << ","
             << d.pid_out << "," << d.enc << std::endl;
    }

    // Homing
    iPos->setRefSpeed(joint_id, 40.);
    iPos->setRefAcceleration(joint_id, std::numeric_limits<double>::max());
    iPos->positionMove(joint_id, 0.);
    while (!done) {
        iPos->checkMotionDone(joint_id, &done);
        Time::yield();
    }

    m_driver.close();
    file.close();

    yInfo()<<"Main returning...";
    return EXIT_SUCCESS;
}
