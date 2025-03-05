/******************************************************************************
 *                                                                            *
 * Copyright (C) 2023 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>

#include "ergoCubEmotions.h"

using namespace yarp::os;
using namespace cv;

ErgoCubEmotions::ErgoCubEmotions()
{
}

ErgoCubEmotions::~ErgoCubEmotions()
{
}

bool ErgoCubEmotions::attach(RpcServer& source)
{
    return yarp().attachAsServer(source);
}

bool ErgoCubEmotions::configure(ResourceFinder& rf)
{
    Bottle &bGroup = rf.findGroup("general");
    nExpressions = bGroup.find("num_expressions").asInt32();
    nTransitions = bGroup.find("num_transitions").asInt32();
    fullscreen = bGroup.find("fullscreen").asBool();
    int graphic_elements = bGroup.find("num_graphics").asInt32();

    for(int i = 0; i < nExpressions; i++)
    {
        std::ostringstream expression_i;
        expression_i << "expression_" << i;
        Bottle &bExpression = rf.findGroup(expression_i.str());
        if(bExpression.get(0).asString().empty())
        {
            yError() << "Missing expression" << i << "in config file!";
            return false;
        }
        std::string name = bExpression.find("name").asString();
        std::string type = bExpression.find("type").asString();
        std::string file = bExpression.find("file").asString();
        std::string filePath = rf.findFile(file);

        avlEmotions.emplace_back(name);

        if(std::count(avlEmotions.begin(), avlEmotions.end(), name) > 1)
        {
            yError() << name << "is not a unique expression";
            return false;
        }

        std::pair<std::string, std::string> par = std::make_pair(type, filePath);
        imgMap[name] = par;

        if(!(std::count(videoFileNames.begin(), videoFileNames.end(), filePath)))
        {
            videoFileNames.push_back(filePath);
            VideoCapture cap(filePath);
            videoCaptures.push_back(cap);
        }
    }

    for(int j = 0; j < nTransitions; j++)
    {
        std::ostringstream transition_j;
        transition_j << "transition_" << j;
        Bottle &bTransition = rf.findGroup(transition_j.str());
        if(bTransition.get(0).asString().empty())
        {
            yError() << "Missing transition" << j << "in config file!";
            return false;
        }
        std::string source = bTransition.find("source").asString();
        std::string destination = bTransition.find("destination").asString();

        if(imgMap.count(source) < 1 || imgMap.count(destination) < 1)
        {
            yError() << "Transition" << j << "is pointing to a non existing emotion!";
            return false;
        }

        if(transitionMap.count({source, destination}))
        {
            yError() << "Transition from" << source << "to" << destination << "is not unique!";
            return false;
        }

        std::string file = bTransition.find("file").asString();
        std::string filePath = rf.findFile(file);
        std::pair<std::string, std::string> par = std::make_pair(source, destination);
        transitionMap[par] = filePath;

        if(!(std::count(videoFileNames.begin(), videoFileNames.end(), filePath)))
        {
            videoFileNames.push_back(filePath);
        }
    }

    for (int j = 0; j < graphic_elements; j++)
    {
        std::ostringstream element_j;
        element_j << "graphic_" << j;
        std::string group_name = element_j.str();
        auto& elementGroup = rf.findGroup(group_name);
        if (elementGroup.isNull())
        {
            yError() << "Missing group" << group_name;
            return false;
        }
        auto element = GraphicElement::parse(elementGroup);
        if (!element)
        {
            yError() << "Failed to parse graphic element" << group_name;
            return false;
        }
        if (graphicElements.find(element->name) != graphicElements.end())
        {
            yError() << "Duplicate graphic element" << element->name;
            return false;
        }
        graphicElements[element->name] = element;
    }

    isTransition = true;
    command = "neutral";

    if(fullscreen){
        namedWindow("emotion", WND_PROP_FULLSCREEN);
        setWindowProperty("emotion", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
    }
    else{
        namedWindow("emotion", WINDOW_NORMAL);
    }

    std::string portprefix{""};
    if(rf.check("portprefix") && rf.find("portprefix").isString()) {
        portprefix = rf.find("portprefix").asString();
        if (portprefix[0] != '/') {
            yError()<<"Portprefix does not start with /, closing";
            return false;
        }
    }
    cmdPort.open(portprefix+"/ergoCubEmotions/rpc");
    attach(cmdPort);
    return true;
}

bool ErgoCubEmotions::close()
{
    std::lock_guard<std::mutex> guard(mutex);
    cmdPort.close();
    destroyAllWindows();
    return true;
}

double ErgoCubEmotions::getPeriod()
{
    return 1.0;
}

bool ErgoCubEmotions::updateModule()
{
    bool isTransition_local;
    std::pair<std::string, std::string> info;
    std::string current_local, command_local;
    {
        std::lock_guard<std::mutex> guard(mutex);
        isTransition_local = isTransition;
        current_local = currentCommand;
        command_local = command;
        auto it = imgMap.find(command);
        if (it != imgMap.end())
        {
            info = it->second;
            currentCommand = command;
        }
        isTransition = false;
    }


    if(info.first == "image")
    {
        if(isTransition_local)
        {
            showTransition(current_local, command_local);
        }
        Mat img_tmp = imread(info.second);
        if(img_tmp.empty())
        {
            yDebug() << "Could not read the image!";
            return true;
        }
        else
        {
            img = img_tmp;
            updateFrame();
        }
    }
    else if(info.first == "video")
    {
        if(isTransition_local)
        {
            showTransition(current_local, command_local);
        }

        VideoCapture cap;
        for (size_t i = 0; i < videoFileNames.size(); i++)
        {
            if(info.second == videoFileNames[i])
            {
                cap = videoCaptures.at(i);
            }
        }

        Mat frame;

        while(cap.isOpened())
        {
            cap >> frame;
            if(frame.empty())
            {
                // We still need to update the frame in case of
                // graphic elements updates
                updateFrame();
                break;
            }
            img = frame;
            updateFrame();
        }
        cap.release();
    }

    return true;
}

bool ErgoCubEmotions::setEmotion(const std::string& command)
{
    std::lock_guard<std::mutex> guard(mutex);

    if(imgMap.find(command) == imgMap.end())
    {
        yError() << command << "not found!";
        return false;
    }

    this->command = command;
    isTransition = true;

    if (currentCommand == command)
    {
        yWarning() << command << "is already set!";
        isTransition = false;
    }

    return true;
}

void ErgoCubEmotions::showTransition(const std::string& current, const std::string& desired)
{
    for(auto k = transitionMap.cbegin(); k!= transitionMap.cend(); k++)
    {
        if(k->first.first == current && k->first.second == desired)
        {
            VideoCapture capTrans;
            for (size_t i = 0; i < videoFileNames.size(); i++)
            {
                if(k->second == videoFileNames[i])
                {
                    capTrans.open(videoFileNames[i]);
                }
            }
            Mat frameTrans;

            while(capTrans.isOpened())
            {
                capTrans >> frameTrans;
                if(frameTrans.empty())
                {
                    break;
                }
                img = frameTrans;
                updateFrame();
            }
            capTrans.release();
            return;
        }
    }
}

bool ErgoCubEmotions::setGraphicVisibility(const std::string& name, const bool visible)
{
    auto element = graphicElements.find(name);
    if (element == graphicElements.end())
    {
        yError() << "Unknown graphic element" << name;
        return false;
    }
    element->second->visible = visible;
    return true;
}

bool ErgoCubEmotions::setGraphicColor(const std::string& name, const double r, const double g, const double b)
{
    auto element = graphicElements.find(name);
    if (element == graphicElements.end())
    {
        yError() << "Unknown graphic element" << name;
        return false;
    }
    std::lock_guard<std::mutex> guard(element->second->mutex);
    element->second->color = cv::Scalar(b, g, r);
    return true;
}

std::vector<std::string> ErgoCubEmotions::availableGraphics()
{
    std::vector<std::string> output;

    for (auto& element : graphicElements)
    {
        output.push_back(element.first);
    }
    return output;
}

std::vector<std::string> ErgoCubEmotions::availableEmotions()
{
    return avlEmotions;
}

void ErgoCubEmotions::updateFrame()
{
    //Use a led for listening with different colors and an animation for speaking
    //use one led also for thinking
    imgEdited = img.clone();
    for (auto& element : graphicElements)
    {
        element.second->draw(imgEdited);
    }
    imshow("emotion", imgEdited);
    pollKey();
}

std::shared_ptr<GraphicElement> GraphicElement::parse(const yarp::os::Bottle& options)
{
    if (!options.check("type"))
    {
        yError() << "Missing type in options!";
        return std::shared_ptr<GraphicElement>();
    }
    if (!options.check("name"))
    {
        yError() << "Missing name in options!";
        return std::shared_ptr<GraphicElement>();
    }
    if (!options.check("color_rgb"))
    {
        yError() << "Missing color_rgb in options!";
        return std::shared_ptr<GraphicElement>();
    }

    std::string name = options.find("name").asString();
    std::string type = options.find("type").asString();
    bool visible = options.check("visible", yarp::os::Value(true)).asBool();

    auto color = options.find("color_rgb").asList();
    if (!color || color->size() != 3)
    {
        yError() << "color_rgb must have 3 elements!";
        return std::shared_ptr<GraphicElement>();
    }
    if (!color->get(0).isInt32() || !color->get(1).isInt32() || !color->get(2).isInt32())
    {
        yError() << "Color elements must be integers!";
        return std::shared_ptr<GraphicElement>();
    }
    // RGB to BGR
    cv::Scalar colorScalar(color->get(2).asInt32(), color->get(1).asInt32(), color->get(0).asInt32());


    std::shared_ptr<GraphicElement> output;
    if (type == "circle")
    {
        output = Circle::parse(options);
    }
    else if (type == "stadium")
    {
        output = Stadium::parse(options);
    }
    else
    {
        yError() << "Unknown type" << type;
    }

    if (output)
    {
        output->name = name;
        output->visible = visible;
        output->color = colorScalar;
    }

    return output;

}

void Circle::draw(cv::Mat& img)
{
    std::lock_guard<std::mutex> guard(mutex);
    if (!visible)
    {
        return;
    }
    circle(img, center, radius, color, FILLED);
}

std::shared_ptr<GraphicElement> Circle::parse(const yarp::os::Bottle& options)
{
    if (!options.check("center"))
    {
        yError() << "Missing center option!";
        return std::shared_ptr<GraphicElement>();
    }
    if (!options.check("radius"))
    {
        yError() << "Missing radius in options!";
        return std::shared_ptr<GraphicElement>();
    }

    auto center = options.find("center").asList();
    if (!center || center->size() != 2)
    {
        yError() << "Center must have 2 elements!";
        return std::shared_ptr<GraphicElement>();
    }
    if (!center->get(0).isInt32() || !center->get(1).isInt32())
    {
        yError() << "Center elements must be integers!";
        return std::shared_ptr<GraphicElement>();
    }
    cv::Point centerPoint(center->get(0).asInt32(), center->get(1).asInt32());

    auto& radius = options.find("radius");
    if (!radius.isInt32())
    {
        yError() << "Radius must be an integer!";
        return std::shared_ptr<GraphicElement>();
    }

    auto output = std::make_shared<Circle>();

    output->center = centerPoint;
    output->radius = radius.asInt32();

    return output;
}

void Stadium::draw(cv::Mat& img)
{
    std::lock_guard<std::mutex> guard(mutex);
    if (!visible)
    {
        return;
    }
    // Calculate the radius of the semicircles
    int radius = height / 2;

    // Calculate the coordinates of the rectangle
    Point rectTopLeft(center.x - width / 2, center.y - radius);
    Point rectBottomRight(center.x + width / 2, center.y + radius);

    // Draw the rectangle
    rectangle(img, rectTopLeft, rectBottomRight, color, FILLED);

    // Draw the semicircles
    ellipse(img, Point(center.x - width / 2, center.y), Size(radius, radius), 270, 180, 360, color, FILLED);
    ellipse(img, Point(center.x + width / 2, center.y), Size(radius, radius), 90, 180, 360, color, FILLED);
}

std::shared_ptr<GraphicElement> Stadium::parse(const yarp::os::Bottle& options)
{
    if (!options.check("center"))
    {
        yError() << "Missing center option!";
        return std::shared_ptr<GraphicElement>();
    }
    if (!options.check("height"))
    {
        yError() << "Missing height in options!";
        return std::shared_ptr<GraphicElement>();
    }
    if (!options.check("width"))
    {
        yError() << "Missing width in options!";
        return std::shared_ptr<GraphicElement>();
    }

    auto center = options.find("center").asList();
    if (!center || center->size() != 2)
    {
        yError() << "Center must have 2 elements!";
        return std::shared_ptr<GraphicElement>();
    }
    if (!center->get(0).isInt32() || !center->get(1).isInt32())
    {
        yError() << "Center elements must be integers!";
        return std::shared_ptr<GraphicElement>();
    }
    cv::Point centerPoint(center->get(0).asInt32(), center->get(1).asInt32());

    auto& height = options.find("height");
    if (!height.isInt32())
    {
        yError() << "Height must be an integer!";
        return std::shared_ptr<GraphicElement>();
    }

    auto& width = options.find("width");
    if (!width.isInt32())
    {
        yError() << "Width must be an integer!";
        return std::shared_ptr<GraphicElement>();
    }

    auto output = std::make_shared<Stadium>();
    output->center = centerPoint;
    output->height = height.asInt32();
    output->width = width.asInt32();
    return output;
}
