#include <yarp/os/ResourceFinder.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Bottle.h>
#include "emotionHandler.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

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
    rpcPort.open("/emotionHandler/rpc");
    attach(rpcPort);

    return true;
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
        case EMOTION_VOCAB_SET:
        {
            namedWindow("emotion", WINDOW_NORMAL);
            bool ok = false;
            reply.clear();

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
                    yDebug() << "Command not recognized!";
                    break;
                }
            }
            reply.addVocab32(EMOTION_VOCAB_OK);
        }
    }
    return true;
}

bool EmotionHandler::getCommand(std::string command)
{
    Mat image = imread("/home/martinagloria/ergocub-software/src/modules/emotionHandler/images/" + command + ".png");

    if(image.empty())
    {
        yDebug() << "Could not read the image!";
    }
    
    imshow("emotion", image);
    waitKey(0);

    return true;
}