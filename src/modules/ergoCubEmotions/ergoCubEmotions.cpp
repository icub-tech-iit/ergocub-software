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

    if (nExpressions <= 0)
    {
        yError() << "Number of expressions must be greater than 0!";
        return false;
    }

    std::string defaultExpression = bGroup.find("default").asString();

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

        if (defaultExpression.empty())
        {
            defaultExpression = name;
        }

        if (filePath.empty())
        {
            yError() << "The path of the file" << file
                     << "for the emotion" << name
                     << "got resolved to an empty path."
                     << " The specified file may not exist.";
            return false;
        }

        if (emotions.count(name))
        {
            yError() << name << "is not a unique expression!";
            return false;
        }


        if (type == "image")
        {
            auto source = std::make_shared<ImageSource>();
            if (!source->open(filePath))
            {
                yError() << "Could not open the image" << file
                         << "with path" << filePath
                         << "for the expression" << name;
                return false;
            }
            emotions[name] = source;
        }
        else if (type == "video")
        {
            auto source = std::make_shared<VideoSource>();
            source->loop = bExpression.check("loop", yarp::os::Value(false)).asBool();
            if (!source->open(filePath))
            {
                yError() << "Could not open the video" << file
                         << "with path" << filePath
                         << "for the expression" << name;
                return false;
            }
            emotions[name] = source;
        }
        else
        {
            yError() << "Unknown type" << type << "for expression" << name;
            return false;
        }

        avlEmotions.emplace_back(name);
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

        if(emotions.count(source) == 0 || emotions.count(destination) == 0)
        {
            yError() << "Transition" << j << "is pointing to a non existing emotion!";
            return false;
        }

        if (transitions.count(source) != 0 && transitions[source].count(destination) != 0)
        {
            yError() << "Transition from" << source << "to" << destination << "is not unique!";
            return false;
        }

        std::string file = bTransition.find("file").asString();
        std::string filePath = rf.findFile(file);

        if (filePath.empty())
        {
            yError() << "The path of the file" << file
                << "for the transition from" << source
                << "to" << destination
                << "got resolved to an empty path."
                << " The specified file may not exist.";
            return false;
        }

        VideoSource transition;
        if (!transition.open(filePath))
        {
            yError() << "Could not open the file" << file
                     << "with path" << filePath
                     << "for the transition from" << source
                     << "to" << destination;
            return false;
        }

        transitions[source][destination] = transition;
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
    if (emotions.find(defaultExpression) == emotions.end())
    {
        yError() << "The default expression " << defaultExpression << "has not been found!";
        return false;
    }
    command = defaultExpression;

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
    shouldUpdate = true;
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
    std::string current_local, command_local;
    std::shared_ptr<Source> source;
    {
        std::lock_guard<std::mutex> guard(mutex);
        isTransition_local = isTransition;
        current_local = currentCommand;
        command_local = command;
        auto new_it = emotions.find(command);
        if (new_it == emotions.end())
        {
            // This should not have happened
            // since the same check is done in
            // setEmotion call
            yError() << command << "not found!";
            return false;
        }
        auto current_it = emotions.find(currentCommand);
        if (current_it != emotions.end())
        {
            current_it->second->restart();
        }
        currentCommand = command;
        source = new_it->second;
        isTransition = false;
        shouldUpdate = false;
    }

    if (!source)
    {
        yError() << "Unknown emotion" << command_local;
        return false;
    }

    if (isTransition_local)
    {
        showTransition(current_local, command_local);
    }

    Mat img_tmp = source->newImage();
    bool updatedOnce = false;
    while (!img_tmp.empty() && !shouldUpdate)
    {
        img = img_tmp;
        updateFrame();
        img_tmp = source->newImage();
        updatedOnce = true;
    }
    if (!updatedOnce)
    {
        // We still need to update the frame in case of
        // graphic elements updates
        updateFrame();
    }
    if (img_tmp.empty() && source->loop)
    {
        source->restart();
    }
    return true;
}

bool ErgoCubEmotions::setEmotion(const std::string& command)
{
    std::lock_guard<std::mutex> guard(mutex);

    if(emotions.find(command) == emotions.end())
    {
        yError() << command << "not found!";
        return false;
    }

    this->command = command;

    if (currentCommand == command)
    {
        yWarning() << command << "is already set!";
        isTransition = false;
    }
    else
    {
        isTransition = true;
        shouldUpdate = true;
    }

    return true;
}

void ErgoCubEmotions::showTransition(const std::string& current, const std::string& desired)
{
    auto sourceIter = transitions.find(current);
    if (sourceIter == transitions.end())
    {
        return;
    }

    auto transitionIter = sourceIter->second.find(desired);

    if (transitionIter == sourceIter->second.end())
    {
        return;
    }

    VideoSource& source = transitionIter->second;

    Mat img_tmp = source.newImage();
    while (!img_tmp.empty() && !shouldUpdate)
    {
        img = img_tmp;
        updateFrame();
        img_tmp = source.newImage();
    }
    source.restart();
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

bool VideoSource::open(const std::string& path)
{
    bool output = cap.open(path);
    if (!output)
    {
        return false;
    }
    fps = cap.get(CAP_PROP_FPS);
    return true;
}

cv::Mat VideoSource::newImage()
{
    double currentTime = yarp::os::Time::now();
    if (lastFrameTime > 0.0)
    {
        double elapsedTime = currentTime - lastFrameTime;
        double frameTime = 1.0 / fps;
        yarp::os::Time::delay(std::max(0.0, frameTime - elapsedTime));
    }
    cap >> frame;
    lastFrameTime = yarp::os::Time::now();
    return frame;
}

void VideoSource::restart()
{
    cap.set(CAP_PROP_POS_MSEC, 0.0);
    lastFrameTime = -1.0;
}

bool ImageSource::open(const std::string& path)
{
    img = imread(path);
    return !img.empty();
}

cv::Mat ImageSource::newImage()
{
    if (shouldRestart)
    {
        // We notify that the image has been read
        // as is the case for the video source
        // when we reach the end of the video
        return cv::Mat();
    }
    shouldRestart = true;
    return img;
}

void ImageSource::restart()
{
    shouldRestart = false;
}
