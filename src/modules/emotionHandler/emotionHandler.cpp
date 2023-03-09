#include <yarp/os/ResourceFinder.h>
#include "emotionHandler.h"

bool EmotionHandler::configure(ResourceFinder& config)
{

    return true;
}

bool EmotionHandler::close()
{

    return true;
}

bool EmotionHandler::interruptModule()
{

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
    switch(command.get(0).asVocab32())
    {
        case EMOTION_VOCAB_SET:
        {
            switch (command.get(1).asVocab32())
            {
            case EMOTION_VOCAB_ANGRY:
                /* code */
                break;
            case EMOTION_VOCAB_CUN:
                /* code */
                break;
            case EMOTION_VOCAB_EVIL:
                /* code */
                break;
            case EMOTION_VOCAB_HAPPY:
                /* code */
                break;
            case EMOTION_VOCAB_NEUTRAL:
                /* code */
                break;
            case EMOTION_VOCAB_SAD:
                /* code */
                break;
            case EMOTION_VOCAB_SHY:
                /* code */
                break;
            case EMOTION_VOCAB_SURPRISED:
                /* code */
                break;
            default:
                break;
            }
        }
    }
    return true;
}