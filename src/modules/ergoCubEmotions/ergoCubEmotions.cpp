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
    this->rf=&rf;
    Bottle &bGroup = rf.findGroup("general");
    nExpressions = bGroup.find("num_expressions").asInt32();
    nTransitions = bGroup.find("num_transitions").asInt32();
    for (int i = 0; i < nExpressions; i++)
    {
        std::ostringstream expression_i;
        expression_i << "expression_" << i;
        Bottle &bExpression = rf.findGroup(expression_i.str());
        std::string name = bExpression.find("name").asString();
        std::string type = bExpression.find("type").asString();
        std::string file = bExpression.find("file").asString();
        
        avlEmotions.emplace_back(name);

        std::pair<std::string, std::string> par = std::make_pair(type, file);
        imgMap[name] = par;
    }

    for (int j = 0; j < nTransitions; j++)
    {
        std::ostringstream transition_j;
        transition_j << "transition_" << j;
        Bottle &bTransition = rf.findGroup(transition_j.str());
        std::string source = bTransition.find("source").asString();
        std::string destination = bTransition.find("destination").asString();
        std::string file = bTransition.find("file").asString();
        
        std::pair<std::string, std::string> par = std::make_pair(source, destination);
        transitionMap[par] = file;
    }

    command = "neutral";

    namedWindow("emotion", WND_PROP_FULLSCREEN);
    setWindowProperty("emotion", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);  
    path = rf.findFile("expressions/images/exp_img_2.png");
    img = imread(path);
    if(img.empty())
    {
        yError() << "Could not read the image";
        return false;
    }
    imshow("emotion", img);
    waitKey(1000);

    cmdPort.open("/ergoCubEmotions/rpc");
    attach(cmdPort);
    return true;
}

bool ErgoCubEmotions::close()
{
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
    for(auto it = imgMap.cbegin(); it!= imgMap.cend(); it++)
    {
        if(it->first == command)
        {   
            if(it->second.first == "image")
            {   
                showTransition();
                path = rf->findFile(it->second.second);
                Mat img_tmp = imread(path);
                if(img_tmp.empty())
                {
                    yDebug() << "Could not read the image!";
                    break;
                }
                else
                {
                    img = img_tmp;
                    imshow("emotion", img);
                    pollKey();
                }
            }
            else if(it->second.first == "video")
            {
                showTransition();
                path = rf->findFile(it->second.second);
                VideoCapture cap(path);
                Mat frame;

                while(cap.isOpened())
                {
                    cap >> frame;
                    if(frame.empty())
                        break;
                    imshow("emotion", frame);
                    pollKey();  
                }
            }
        }
    }
    return true;
}

bool ErgoCubEmotions::setEmotion(const std::string& command)
{   
    if(imgMap.count(command) < 1)
    {
        yError() << "Command not found!";
        return false;
    }

    else
    {
        cmd_tmp = this->command;
        this->command = command;
        return true;
    }
}

void ErgoCubEmotions::showTransition()
{
    for(auto k = transitionMap.cbegin(); k!= transitionMap.cend(); k++)
    {
        if(k->first.first == cmd_tmp && k->first.second == command)
        {
            std::string pathTrans = rf->findFile(k->second);
            VideoCapture capTrans(pathTrans);
            Mat frameTrans;

            while(capTrans.isOpened())
            {
                capTrans >> frameTrans;
                if(frameTrans.empty())
                    break;
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