/******************************************************************************
 *                                                                            *
 * Copyright (C) 2023 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include "emotionHandler.h"

using namespace yarp::os;
using namespace std;

int main(int argc, char *argv[])
{
    Network yarp;
    if(!yarp.checkNetwork())
    {
        yError()<<"Yarp doesn't work!";
        return EXIT_FAILURE;
    }
    
    ResourceFinder rf;
    rf.configure(argc, argv);
    EmotionHandler emotionHandler;
    emotionHandler.configure(rf);

    return emotionHandler.runModule();
}