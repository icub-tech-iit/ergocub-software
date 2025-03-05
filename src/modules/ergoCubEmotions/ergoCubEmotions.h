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
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "ergoCubEmotions_IDL.h"

#include <mutex>
#include <map>
#include <unordered_map>
#include <memory>

class GraphicElement
{
public:
    std::string name;
    std::atomic<bool> visible {true};

    virtual void draw(cv::Mat& img) = 0;

    static std::shared_ptr<GraphicElement> parse(const yarp::os::Bottle& options);
};

class Circle : public GraphicElement
{
public:
    cv::Point center;
    int radius;
    cv::Scalar color; //BGR

    Circle() = default;
    Circle(cv::Point center, int radius, cv::Scalar color);

    void draw(cv::Mat& img) override;

    static std::shared_ptr<GraphicElement> parse(const yarp::os::Bottle& options);
};

struct Stadium : public GraphicElement
{
    cv::Point center;
    int height;
    int width;
    cv::Scalar color; //BGR

    Stadium() = default;
    Stadium(cv::Point center, int height, int width, cv::Scalar color);

    void draw(cv::Mat& img) override;

    static std::shared_ptr<GraphicElement> parse(const yarp::os::Bottle& options);
};

class ErgoCubEmotions : public yarp::os::RFModule, public ergoCubEmotions_IDL
{
public:
    ErgoCubEmotions();
    ~ErgoCubEmotions();

    bool attach(yarp::os::RpcServer &source) override;
    bool configure(yarp::os::ResourceFinder &config) override;
    bool close() override;
    bool updateModule() override;
    double getPeriod() override;

    bool setEmotion(const std::string &command) override;
    std::vector<std::string> availableEmotions() override;
    void showTransition(const std::string &current, const std::string &desired);

    bool setGraphicVisibility(const std::string& name, const bool visible) override;
    std::vector<std::string> availableGraphics() override;

    void updateFrame();

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
    cv::Mat img, imgEdited;
    std::mutex mutex;
    std::vector<cv::VideoCapture> videoCaptures;
    std::vector<std::string> videoFileNames;
    std::unordered_map<std::string, std::shared_ptr<GraphicElement>> graphicElements;
};

#endif
