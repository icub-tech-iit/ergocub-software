/******************************************************************************
 *                                                                            *
 * Copyright (C) 2023 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#ifndef __ERGOCUBEMOTIONS__
#define __ERGOCUBEMOTIONS__

#include <yarp/os/RFModule.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/Thread.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "ergoCubEmotions_IDL.h"

#include <mutex>
#include <map>
#include <unordered_map>

class ErgoCubEmotions : public yarp::os::RFModule, public ergoCubEmotions_IDL
{
public:
    ErgoCubEmotions();
    ~ErgoCubEmotions();

    bool attach(yarp::os::RpcServer &source);
    bool configure(yarp::os::ResourceFinder &config);
    bool close();
    bool updateModule();
    double getPeriod();

    bool setEmotion(const std::string &command);
    std::vector<std::string> availableEmotions();
    void showTransition(const std::string &current, const std::string &desired);

    yarp::os::RpcServer cmdPort;
    int nExpressions;
    int nTransitions;
    bool isTransition;
    bool fullscreen;
    std::unordered_map<std::string, std::pair<std::string, std::string>> imgMap;
    std::map<std::pair<std::string, std::string>, std::string> transitionMap;
    std::string command;
    std::string currentCommand;
    std::vector<std::string> avlEmotions;
    cv::Mat img;
    std::mutex mutex;
    std::vector<cv::VideoCapture> videoCaptures;
    std::vector<std::string> videoFileNames;
};

#endif
