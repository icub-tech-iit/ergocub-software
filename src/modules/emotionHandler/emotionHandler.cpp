/******************************************************************************
 *                                                                            *
 * Copyright (C) 2023 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include <yarp/os/ResourceFinder.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Bottle.h>
#include "emotionHandler.h"

#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>

using namespace cv;
using namespace yarp::os;

EmotionHandler::EmotionHandler()
{
}

EmotionHandler::~EmotionHandler()
{
}

bool EmotionHandler::configure(ResourceFinder& config)
{
    auto ret = rpcPort.open("/emotionHandler/rpc");
    attach(rpcPort);
    return ret;
}

bool EmotionHandler::close()
{
    rpcPort.close();
    return true;
}

double EmotionHandler::getPeriod()
{
    return 1.0;
}

bool EmotionHandler::updateModule()
{
    return true;
}

bool EmotionHandler::respond(const Bottle &cmd, Bottle &reply)
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
            namedWindow("emotion", WINDOW_NORMAL);
            resizeWindow("emotion", Size(1920, 1020));
            reply.clear();

            bool ok = false;
            switch (cmd.get(1).asVocab32())
            {
                case EMOTION_VOCAB_ANGRY:
                {
                    ok = getCommand(cmd.get(1).toString());
                    break;
                }
                case EMOTION_VOCAB_CUN:
                {
                    ok = getCommand(cmd.get(1).toString());
                    break;
                }
                case EMOTION_VOCAB_EVIL:
                {
                    ok = getCommand(cmd.get(1).toString());
                    break;
                }
                case EMOTION_VOCAB_HAPPY:
                {                  
                    ok = getCommand(cmd.get(1).toString());
                    break;
                }
                case EMOTION_VOCAB_NEUTRAL:
                {
                    ok = getCommand(cmd.get(1).toString());
                    break;
                }
                case EMOTION_VOCAB_SAD:
                {
                    ok = getCommand(cmd.get(1).toString());
                    break;
                }
                case EMOTION_VOCAB_SHY:
                {
                    ok = getCommand(cmd.get(1).toString());
                    break;
                }
                case EMOTION_VOCAB_SURPRISED:
                {
                    ok = getCommand(cmd.get(1).toString());
                    break;
                }
                default:
                {   
                    reply.addVocab32(EMOTION_VOCAB_FAILED);
                    yDebug() << "Command not recognized!";
                    break;
                }
            }

            reply.addVocab32(EMOTION_VOCAB_OK);
        }
    }
    return true;
}

bool EmotionHandler::getCommand(const std::string command)
{
    Mat image = imread("images/" + command + ".png");

    if(image.empty())
    {
        yDebug() << "Could not read the image!";
    }
    
    imshow("emotion", image);
    waitKey(0);

    return true;
}