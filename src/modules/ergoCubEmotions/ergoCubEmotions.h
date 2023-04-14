/******************************************************************************
 *                                                                            *
 * Copyright (C) 2023 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#ifndef __ERGOCUBEMOTIONS__
#define __ERGOCUBEMOTIONS__

#include <yarp/os/Vocab.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/ConnectionReader.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "ergoCubEmotions_IDL.h"


class ErgoCubEmotions : public yarp::os::RFModule, public ergoCubEmotions_IDL {
    protected:
        yarp::os::ResourceFinder *rf;

    public:
        ErgoCubEmotions();
        ~ErgoCubEmotions();
        
        bool attach(yarp::os::RpcServer& source);
        bool configure(yarp::os::ResourceFinder& config);
        bool close();
        bool updateModule();
        double getPeriod();

        bool setEmotion(const std::string& command);
        std::vector<std::string> availableEmotions();

        yarp::os::RpcServer cmdPort;
        int nexpressions;
        std::string path;
        std::map<std::string, std::pair<std::string, std::string>> img_map;
        std::string command;
        cv::Mat img;
};
#endif