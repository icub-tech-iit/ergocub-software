/******************************************************************************
 *                                                                            *
 * Copyright (C) 2023 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include <yarp/os/ResourceFinder.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Bottle.h>
#include "ergoCubEmotions.h"

#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

using namespace cv;
using namespace yarp::os;

ErgoCubEmotions::ErgoCubEmotions()
{
}

ErgoCubEmotions::~ErgoCubEmotions()
{
}

bool ErgoCubEmotions::configure(ResourceFinder& config)
{
    auto ret = rpcPort.open("/ergoCubEmotions/rpc");
    attach(rpcPort);
    
    // set window properties
    namedWindow("emotion", WND_PROP_FULLSCREEN);
    setWindowProperty("emotion", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
    Mat start_img = imread("images/neu.png");
    imshow("emotion", start_img);
    int key = pollKey();
    if(key == -1)
    {
        //
    }
    waitKey(0);

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
    return ret;
}

bool ErgoCubEmotions::close()
{
    rpcPort.close();
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

bool ErgoCubEmotions::respond(const Bottle &cmd, Bottle &reply)
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
                    ok = getCommand(cmd.get(1).toString());
                    reply.addVocab32(EMOTION_VOCAB_OK);
                    break;
                }
                case EMOTION_VOCAB_CUN:
                {
                    ok = getCommand(cmd.get(1).toString());
                    reply.addVocab32(EMOTION_VOCAB_OK);
                    break;
                }
                case EMOTION_VOCAB_EVIL:
                {
                    ok = getCommand(cmd.get(1).toString());
                    reply.addVocab32(EMOTION_VOCAB_OK);
                    break;
                }
                case EMOTION_VOCAB_HAPPY:
                {                  
                    ok = getCommand(cmd.get(1).toString());
                    reply.addVocab32(EMOTION_VOCAB_OK);
                    break;
                }
                case EMOTION_VOCAB_NEUTRAL:
                {
                    ok = getCommand(cmd.get(1).toString());
                    reply.addVocab32(EMOTION_VOCAB_OK);
                    break;
                }
                case EMOTION_VOCAB_SAD:
                {
                    ok = getCommand(cmd.get(1).toString());
                    reply.addVocab32(EMOTION_VOCAB_OK);
                    break;
                }
                case EMOTION_VOCAB_SHY:
                {
                    ok = getCommand(cmd.get(1).toString());
                    reply.addVocab32(EMOTION_VOCAB_OK);
                    break;
                }
                case EMOTION_VOCAB_SURPRISED:
                {
                    ok = getCommand(cmd.get(1).toString());
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
}

bool ErgoCubEmotions::getCommand(const std::string command)
{
    Mat image = imread("images/" + command + ".png");

    if(image.empty())
    {
        yDebug() << "Could not read the image!";
    }
    
    imshow("emotion", image);
    int key = pollKey();
    if(key == -1)
    {
        return -1;
    }
    waitKey(0);
    
    return true;
}