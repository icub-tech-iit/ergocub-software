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

    path = rf.getHomeContextPath();
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
    //use std::map to chose the correct image
    Mat start_img = imread("/home/martinagloria/ergocub-software/src/modules/ergoCubEmotions/expressions/images/exp_img_2.png");
    if(start_img.empty())
    {
        yDebug() << "Could not read the image!";
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
    //std::lock_guard<std::mutex> lg(mtx);

    return true;
}

/*bool ErgoCubEmotions::respond(const Bottle &cmd, Bottle &reply)
{
    if(cmd.size() < 2)
    {
        yError() << "Invalid command size: it must be 2";
        return false;
    }

    switch (cmd.get(0).asVocab32())
    {
        case EMOTION_VOCAB_HELP:
        {
            // implement
        }

        case EMOTION_VOCAB_SET:
        {
            reply.clear();
            bool ok = false;
            switch (cmd.get(1).asVocab32())
            { 
                case EMOTION_VOCAB_ANGRY:
                {
                    ok = displayEmotion(cmd.get(1).toString());
                    reply.addVocab32(EMOTION_VOCAB_OK);
                    break;
                }
                case EMOTION_VOCAB_CUN:
                {
                    ok = displayEmotion(cmd.get(1).toString());
                    reply.addVocab32(EMOTION_VOCAB_OK);
                    break;
                }
                case EMOTION_VOCAB_EVIL:
                {
                    ok = displayEmotion(cmd.get(1).toString());
                    reply.addVocab32(EMOTION_VOCAB_OK);
                    break;
                }
                case EMOTION_VOCAB_HAPPY:
                {                  
                    ok = displayEmotion(cmd.get(1).toString());
                    reply.addVocab32(EMOTION_VOCAB_OK);
                    break;
                }
                case EMOTION_VOCAB_NEUTRAL:
                {
                    ok = displayEmotion(cmd.get(1).toString());
                    reply.addVocab32(EMOTION_VOCAB_OK);
                    break;
                }
                case EMOTION_VOCAB_SAD:
                {
                    ok = displayEmotion(cmd.get(1).toString());
                    reply.addVocab32(EMOTION_VOCAB_OK);
                    break;
                }
                case EMOTION_VOCAB_SHY:
                {
                    ok = displayEmotion(cmd.get(1).toString());
                    reply.addVocab32(EMOTION_VOCAB_OK);
                    break;
                }
                case EMOTION_VOCAB_SURPRISED:
                {
                    ok = displayEmotion(cmd.get(1).toString());
                    reply.addVocab32(EMOTION_VOCAB_OK);
                    break;
                }
                default:
                {   
                    yDebug() << "Command not recognized!";
                    reply.addVocab32(EMOTION_VOCAB_FAILED);
                    break;
                }
            }
        }
    }
    return true;
}*/

bool ErgoCubEmotions::setEmotion(const std::string& command)
{    
    std::lock_guard<std::mutex> lg(mtx);
    for(auto it = img_map.cbegin(); it!= img_map.cend(); it++)
    {
        if(it->first.first == command)
        {    
            if(it->first.second == "image")
            {
                //Mat img = imread(it->second);
                yDebug() << path;
                Mat img = imread(path + it->second);
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