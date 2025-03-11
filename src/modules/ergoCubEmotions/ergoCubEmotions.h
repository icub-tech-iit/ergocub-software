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
    cv::Scalar color; //BGR
    std::atomic<bool> visible {true};
    std::mutex mutex;

    virtual void draw(cv::Mat& img) = 0;

    static std::shared_ptr<GraphicElement> parse(const yarp::os::Bottle& options);
};

class Circle : public GraphicElement
{
public:
    cv::Point center;
    int radius {0};

    void draw(cv::Mat& img) override;

    static std::shared_ptr<GraphicElement> parse(const yarp::os::Bottle& options);
};

class Stadium : public GraphicElement
{
public:
    cv::Point center;
    int height {0};
    int width {0};

    void draw(cv::Mat& img) override;

    static std::shared_ptr<GraphicElement> parse(const yarp::os::Bottle& options);
};

class Source
{
public:
    bool loop{ false };
    virtual cv::Mat newImage() = 0;
    virtual void restart() = 0;
};

class VideoSource : public Source
{
    cv::VideoCapture cap;
    cv::Mat frame;
    double fps {30.0};
    double lastFrameTime{ -1.0 };

public:
    bool open(const std::string& path);
    cv::Mat newImage() override;
    void restart() override;
};

class ImageSource : public Source
{
    cv::Mat img;
    bool shouldRestart{ false };
public:
    bool open(const std::string& path);
    cv::Mat newImage() override;
    void restart() override;
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
    bool interruptModule() override;


    bool setEmotion(const std::string &command) override;
    std::vector<std::string> availableEmotions() override;
    void showTransition(const std::string &current, const std::string &desired);

    bool setGraphicVisibility(const std::string& name, const bool visible) override;
    bool setGraphicColor(const std::string& name, const double r, const double g, const double b) override;
    std::vector<std::string> availableGraphics() override;

    void updateFrame();

    yarp::os::RpcServer cmdPort;
    int nExpressions;
    int nTransitions;
    bool isTransition;
    bool fullscreen;
    std::atomic<bool> shouldUpdate { false };
    std::unordered_map<std::string, std::shared_ptr<Source>> emotions;
    std::unordered_map<std::string, std::unordered_map<std::string, VideoSource>> transitions;
    std::string command;
    std::string currentCommand;
    std::vector<std::string> avlEmotions;
    cv::Mat img, imgEdited;
    std::mutex mutex;
    std::unordered_map<std::string, std::shared_ptr<GraphicElement>> graphicElements;
};

#endif
