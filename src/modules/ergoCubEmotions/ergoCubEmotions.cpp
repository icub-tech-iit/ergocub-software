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
    nexpressions = bGroup.find("num_expressions").asInt32();
    for (int i = 0; i < nexpressions; i++)
    {
        std::ostringstream expression_i;
        expression_i << "expression_" << i;
        Bottle &bExpression = rf.findGroup(expression_i.str());
        std::string name = bExpression.find("name").asString();
        std::string type = bExpression.find("type").asString();
        std::string file = bExpression.find("file").asString();
        
        std::pair<std::string, std::string> par = std::make_pair(type, file);
        img_map[name] = par;
    }
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
    for(auto it = img_map.cbegin(); it!= img_map.cend(); it++)
    {
        if(it->first == command)
        {   
            if(it->second.first == "image")
            {   
                path = rf->findFile(it->second.second);
                Mat img_tmp = imread(path);
                if(img_tmp.empty())
                {
                    yDebug() << "Could not read the image!";
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
                path = rf->findFile(it->second.second);
                VideoCapture cap(path);
                while(cap.isOpened())
                {
                    Mat frame;
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
    if(img_map.count(command) < 1)
    {
        yError() << "Command not found!";
        return false;
    }

    else
    {
        this->command = command;
        return true;
    }
}

std::vector<std::string> ErgoCubEmotions::availableEmotions()
{
    std::vector<std::string> cmd;

    for(auto it = img_map.cbegin(); it!= img_map.cend(); it++)
    {
        cmd.push_back(it->first);
    }

    return cmd;
}