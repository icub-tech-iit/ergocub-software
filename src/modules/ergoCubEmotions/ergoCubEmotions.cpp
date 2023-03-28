/******************************************************************************
 *                                                                            *
 * Copyright (C) 2023 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Bottle.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

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
    cmdPort.open("/ergoCubEmotions/rpc");
    attach(cmdPort);
    
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
        
        std::pair<std::string, std::string> par = std::make_pair(name, type);
        img_map[par] = file;
    }

    namedWindow("emotion", WND_PROP_FULLSCREEN);
    setWindowProperty("emotion", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
    path = rf.findFile("expressions/images/exp_img_2.png");
    Mat start_img = imread(path);
    if(start_img.empty())
    {
        yError() << "Could not read the image";
        return false;
    }

    imshow("emotion", start_img);
    waitKey(1000);
    
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
    return true;
}

bool ErgoCubEmotions::setEmotion(const std::string& command)
{    
    std::lock_guard<std::mutex> lg(mtx);
    for(auto it = img_map.cbegin(); it!= img_map.cend(); it++)
    {
        if(it->first.first == command)
        {    
            if(it->first.second == "image")
            {
                path = rf->findFile(it->second);
                Mat img = imread(path);
                if(img.empty())
                {
                    yDebug() << "Could not read the image!";
                }

                imshow("emotion", img);
                pollKey();
            }

            else if(it->first.second == "video")
            {
                // VideoCapture cap("images/video.mp4");
                // while(1)
                // {
                //     Mat frame;
                //     cap >> frame;
                //     imshow("emotion", frame);
                //     char c=(char)waitKey(25);
                //     if(c==27)
                //         break;
                // }
                // cap.release();
            }
        }
    }

    return true;
}