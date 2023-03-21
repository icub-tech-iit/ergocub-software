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

    return true;
}

bool ErgoCubEmotions::close()
{
    cmdPort.close();
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

bool ErgoCubEmotions::startModule()
{
    //std::lock_guard<std::mutex> lg(mtx);
    // set window properties
    namedWindow("emotion", WND_PROP_FULLSCREEN);
    setWindowProperty("emotion", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
    //use std::map to chose the correct image
    Mat start_img = imread("/home/martinagloria/ergocub-software/src/modules/ergoCubEmotions/expressions/images/exp_img_2.png");
    if(start_img.empty())
    {
        yDebug() << "Could not read the image!";
    }

    imshow("emotion", start_img);
    int key = pollKey();
    if(key == -1)
    {
        return 1;
    }
    waitKey(0);

    return true;
}

bool ErgoCubEmotions::setHappy()
{
    std::lock_guard<std::mutex> lg(mtx);
    Mat happy = imread("/home/martinagloria/ergocub-software/src/modules/ergoCubEmotions/expressions/images/exp_img_1.png");
    if(happy.empty())
    {
        yDebug() << "Could not read the image!";
    }

    imshow("emotion", happy);
    int key = pollKey();
    if(key == -1)
    {
        return 1;
    }
    waitKey(0);

    return true;
}

bool ErgoCubEmotions::setAngry()
{
    std::lock_guard<std::mutex> lg(mtx);
    Mat angry = imread("/home/martinagloria/ergocub-software/src/modules/ergoCubEmotions/expressions/images/exp_img_0.png");
    if(angry.empty())
    {
        yDebug() << "Could not read the image!";
    }

    imshow("emotion", angry);
    int key = pollKey();
    if(key == -1)
    {
        return 1;
    }
    waitKey(0);

    return true;
}

bool ErgoCubEmotions::setShy()
{
    std::lock_guard<std::mutex> lg(mtx);
    Mat shy = imread("/home/martinagloria/ergocub-software/src/modules/ergoCubEmotions/expressions/images/exp_img_3.png");
    if(shy.empty())
    {
        yDebug() << "Could not read the image!";
    }

    imshow("emotion", shy);
    int key = pollKey();
    if(key == -1)
    {
        return 1;
    }
    waitKey(0);

    return true;
}

bool ErgoCubEmotions::setNeutral()
{
    std::lock_guard<std::mutex> lg(mtx);
    Mat neutral = imread("/home/martinagloria/ergocub-software/src/modules/ergoCubEmotions/expressions/images/exp_img_2.png");
    if(neutral.empty())
    {
        yDebug() << "Could not read the image!";
    }

    imshow("emotion", neutral);
    int key = pollKey();
    if(key == -1)
    {
        return 1;
    }
    waitKey(0);

    return true;
}

bool ErgoCubEmotions::stopModule()
{
    // std::lock_guard<std::mutex> lg(mtx);
    destroyAllWindows();
    stopModule();

    return true;
}