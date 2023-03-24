/******************************************************************************
 *                                                                            *
 * Copyright (C) 2023 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#ifndef __ERGOCUBEMOTIONS__
#define __ERGOCUBEMOTIONS__

#include <mutex>

#include <yarp/os/Vocab.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/ConnectionReader.h>

#include "ergoCubEmotions_IDL.h"

// define the vocabs
constexpr  yarp::conf::vocab32_t EMOTION_VOCAB_SET = yarp::os::createVocab32('s','e','t');
constexpr  yarp::conf::vocab32_t EMOTION_VOCAB_GET = yarp::os::createVocab32('g','e','t');
constexpr  yarp::conf::vocab32_t EMOTION_VOCAB_IS  = yarp::os::createVocab32('i','s');
constexpr  yarp::conf::vocab32_t EMOTION_VOCAB_FAILED = yarp::os::createVocab32('f','a','i','l');
constexpr  yarp::conf::vocab32_t EMOTION_VOCAB_OK = yarp::os::createVocab32('o','k');
constexpr  yarp::conf::vocab32_t EMOTION_VOCAB_HELP = yarp::os::createVocab32('h','e','l','p');

constexpr  yarp::conf::vocab32_t EMOTION_VOCAB_NEUTRAL = yarp::os::createVocab32('n','e','u');
constexpr  yarp::conf::vocab32_t EMOTION_VOCAB_HAPPY = yarp::os::createVocab32('h','a','p');
constexpr  yarp::conf::vocab32_t EMOTION_VOCAB_SAD = yarp::os::createVocab32('s','a','d');
constexpr  yarp::conf::vocab32_t EMOTION_VOCAB_SURPRISED = yarp::os::createVocab32('s','u','r');
constexpr  yarp::conf::vocab32_t EMOTION_VOCAB_ANGRY = yarp::os::createVocab32('a','n','g');
constexpr  yarp::conf::vocab32_t EMOTION_VOCAB_EVIL = yarp::os::createVocab32('e','v','i');
constexpr  yarp::conf::vocab32_t EMOTION_VOCAB_SHY = yarp::os::createVocab32('s','h','y');
constexpr  yarp::conf::vocab32_t EMOTION_VOCAB_CUN = yarp::os::createVocab32('c','u','n');

// ergoCubEmotions class definition
class ErgoCubEmotions : public yarp::os::RFModule, public ergoCubEmotions_IDL {
    private:

    public:
        ErgoCubEmotions();
        ~ErgoCubEmotions();
        
        bool attach(yarp::os::RpcServer& source);
        bool configure(yarp::os::ResourceFinder& config);
        bool close();
        bool updateModule();
        double getPeriod();

        bool setEmotion(const std::string& command);

        std::mutex mtx;
        yarp::os::RpcServer cmdPort;

        int nexpressions;
        std::string path;
        std::map<std::pair<std::string, std::string>, std::string> img_map;
};

#endif