/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "HandMk5CouplingHandler.h"
#include <yarp/os/LogStream.h>
#include <cmath>


YARP_DECLARE_LOG_COMPONENT(HANDMK5COUPLINGHANDLER)


HandMk5CouplingHandler::HandMk5CouplingHandler()
{
    m_couplingSize = 12;
}



bool HandMk5CouplingHandler::parseFingerParameters(yarp::os::Bottle& hand_params)
{
    auto L0x    = hand_params.findGroup("L0x");
    auto L0y    = hand_params.findGroup("L0y");
    auto q2bias = hand_params.findGroup("q2bias");
    auto q1off  = hand_params.findGroup("q1off");
    auto k      = hand_params.findGroup("k");
    auto d      = hand_params.findGroup("d");
    auto l      = hand_params.findGroup("l");
    auto b      = hand_params.findGroup("b");

    constexpr int nFingers = 5;
    // All the +1 is because the first element of the bottle is the name of the group
    if(L0x.size()!=nFingers+1 || L0y.size()!=nFingers+1 || q2bias.size()!=nFingers+1 ||
       q1off.size()!=nFingers+1 || k.size()!=nFingers+1 || d.size()!=nFingers+1 ||
       l.size()!=nFingers+1 || b.size()!=nFingers+1 )
    {
        yError()<<"HandMk5CouplingHandler: invalid hand parameters, check your configuration file";
        return false;
    }


    const std::array<std::string,5> names = {"thumb", "index", "middle", "ring", "pinky"};
    for (std::size_t i = 0; i < names.size(); i++)
    {
        mFingerParameters.insert({names.at(i), {L0x.get(i+1).asFloat32(), L0y.get(i+1).asFloat32(), q2bias.get(i+1).asFloat32(),
                                  q1off.get(i+1).asFloat32(), k.get(i+1).asFloat32(), d.get(i+1).asFloat32(),
                                  l.get(i+1).asFloat32(), b.get(i+1).asFloat32()}});
    }

    return true;
}

bool HandMk5CouplingHandler::open(yarp::os::Searchable& config) {
    yarp::os::Bottle& coupling_group_bottle = config.findGroup("COUPLING_PARAMS");

    // TODO INVOKE ImplementCoupling::initialise()
    if(coupling_group_bottle.isNull())
    {
        yError()<<"HandMk5CouplingHandler: missing coupling parameters, check your configuration file";
        return false;
    }

    return parseFingerParameters(coupling_group_bottle);
}

    bool HandMk5CouplingHandler::convertFromPhysicalJointsToActuatedAxesPos(const yarp::sig::Vector& physJointsPos, yarp::sig::Vector& actAxesPos) { return false; }
    bool HandMk5CouplingHandler::convertFromPhysicalJointsToActuatedAxesVel(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsVel, yarp::sig::Vector& actAxesVel) { return false; }
    bool HandMk5CouplingHandler::convertFromPhysicalJointsToActuatedAxesAcc(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsVel, const yarp::sig::Vector& physJointsAcc, yarp::sig::Vector& actAxesAcc) { return false; }
    bool HandMk5CouplingHandler::convertFromPhysicalJointsToActuatedAxesTrq(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsTrq, yarp::sig::Vector& actAxesTrq) { return false; }
    bool HandMk5CouplingHandler::convertFromActuatedAxesToPhysicalJointsPos(const yarp::sig::Vector& actAxesPos, yarp::sig::Vector& physJointsPos) { return false; }
    bool HandMk5CouplingHandler::convertFromActuatedAxesToPhysicalJointsVel(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesVel, yarp::sig::Vector& physJointsVel) { return false; }
    bool HandMk5CouplingHandler::convertFromActuatedAxesToPhysicalJointsAcc(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesVel, const yarp::sig::Vector& actAxesAcc, yarp::sig::Vector& physJointsAcc) { return false; }
    bool HandMk5CouplingHandler::convertFromActuatedAxesToPhysicalJointsTrq(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesTrq, yarp::sig::Vector& physJointsTrq) { return false; }
