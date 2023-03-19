/******************************************************************************
 *                                                                            *
 * Copyright (C) 2023 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include "ergoCubEmotions.h"

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
    //rf.setDefaultConfigFile("config.ini");
    rf.configure(argc, argv);

    ErgoCubEmotions ergoCubEmotions;
    ergoCubEmotions.configure(rf);

    return ergoCubEmotions.runModule();
}