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
            imshow("emotion", img);
            pollKey();
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
                break;
            }
            imshow("emotion", frame);
            pollKey();
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
                imshow("emotion", frameTrans);
                pollKey();
            }
            capTrans.release();
            return;
        }
    }
}

std::vector<std::string> ErgoCubEmotions::availableEmotions()
{
    return avlEmotions;
}
