/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "HandMk5CouplingHandler.h"
#include <yarp/os/LogStream.h>
#include <yarp/os/LogComponent.h>
#include <cmath>


YARP_DECLARE_LOG_COMPONENT(HANDMK5COUPLINGHANDLER)


HandMk5CouplingHandler::HandMk5CouplingHandler()
{
    m_couplingSize = 12;
}



bool HandMk5CouplingHandler::parseFingerParameters(yarp::os::Searchable& config)
{

    yarp::os::Bottle& hand_params = config.findGroup("COUPLING_PARAMS");
    if(hand_params.isNull())
    {
        yCError(HANDMK5COUPLINGHANDLER)<<"Missing COUPLING_PARAMS section in the configuration file";
        return false;
    }
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
        yCError(HANDMK5COUPLINGHANDLER)<<"Invalid hand parameters, check your configuration file";
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

bool HandMk5CouplingHandler::parseCouplingParameters(yarp::os::Searchable& config) {
    yarp::sig::VectorOf<size_t> coupled_physical_joints{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    yarp::sig::VectorOf<size_t> coupled_actuated_axes{0, 1, 2, 3, 4, 5};
    std::vector<std::string> physical_joint_names;
    std::vector<std::string> actuated_axes_names;
    std::vector<std::pair<double, double>> physical_joint_limits;

    yarp::os::Bottle physical_joint_names_bottle = config.findGroup("jointNames");

    if (physical_joint_names_bottle.isNull()) {
        yCError(HANDMK5COUPLINGHANDLER) << "Error cannot find jointNames." ;
        return false;
    }

    for (size_t i = 1; i < physical_joint_names.size(); i++) {
        physical_joint_names.push_back(physical_joint_names_bottle.get(i).asString().c_str());
    }

    yarp::os::Bottle& coupling_params = config.findGroup("COUPLING");
    if (coupling_params.size() ==0)
    {
        yCError(HANDMK5COUPLINGHANDLER) << "Missing param in COUPLING section";
        return false;
    }
    yCDebug(HANDMK5COUPLINGHANDLER) << "Requested couplings:" << coupling_params.toString();
    yCDebug(HANDMK5COUPLINGHANDLER) << "Size: " << coupling_params.size();

    yarp::os::Bottle actuated_axes_names_bottle = coupling_params.findGroup("actuatedAxesNames");

    if (actuated_axes_names_bottle.isNull()) {
        yCError(HANDMK5COUPLINGHANDLER) << "Error cannot find actuatedAxesNames." ;
        return false;
    }

    for (size_t i = 1; i < actuated_axes_names_bottle.size(); i++) {
        actuated_axes_names.push_back(actuated_axes_names_bottle.get(i).asString().c_str());
    }
    physical_joint_limits.resize(physical_joint_names.size());
    yarp::os::Bottle& limits_bottle = config.findGroup("LIMITS");
    if (!limits_bottle.isNull())
    {
        yarp::os::Bottle& pos_limit_min = limits_bottle.findGroup("jntPosMin");
        if (!pos_limit_min.isNull() && static_cast<size_t>(pos_limit_min.size()) == physical_joint_limits.size()+1)
        {
            for(size_t i = 0; i < physical_joint_limits.size(); ++i)
            {
                physical_joint_limits[i].first = pos_limit_min.get(i+1).asFloat64();
            }
        }
        else
        {
            yCError(HANDMK5COUPLINGHANDLER) << "Failed to parse jntPosMin parameter";
            return false;
        }
        yarp::os::Bottle& pos_limit_max = limits_bottle.findGroup("jntPosMax");
        if (!pos_limit_max.isNull() && static_cast<size_t>(pos_limit_max.size()) == physical_joint_limits.size()+1)
        {
            for(size_t i = 0; i < physical_joint_limits.size(); ++i)
            {
                physical_joint_limits[i].second = pos_limit_max.get(i+1).asFloat64();

            }
        }
        else
        {
            yCError(HANDMK5COUPLINGHANDLER) << "Failed to parse jntPosMax parameter";
            return false;
        }
    }
    else
    {
        yCError(HANDMK5COUPLINGHANDLER) << "Failed to parse LIMITS parameter";
        return false;
    }
    initialise(coupled_physical_joints, coupled_actuated_axes, physical_joint_names, actuated_axes_names, physical_joint_limits);
    return true;
}

bool HandMk5CouplingHandler::open(yarp::os::Searchable& config) {

    // TODO INVOKE ImplementCoupling::initialise()
    bool ok = parseFingerParameters(config);
    if (!ok) {
        yCError(HANDMK5COUPLINGHANDLER) << "Error parsing finger parameters";
        return false;
    }

    ok &= parseCouplingParameters(config);
    if (!ok) {
        yCError(HANDMK5COUPLINGHANDLER) << "Error parsing coupling parameters";
        return false;
    }

    return ok;
}

bool HandMk5CouplingHandler::convertFromPhysicalJointsToActuatedAxesPos(const yarp::sig::Vector& physJointsPos, yarp::sig::Vector& actAxesPos) { return false; }
bool HandMk5CouplingHandler::convertFromPhysicalJointsToActuatedAxesVel(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsVel, yarp::sig::Vector& actAxesVel) { return false; }
bool HandMk5CouplingHandler::convertFromPhysicalJointsToActuatedAxesAcc(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsVel, const yarp::sig::Vector& physJointsAcc, yarp::sig::Vector& actAxesAcc) { return false; }
bool HandMk5CouplingHandler::convertFromPhysicalJointsToActuatedAxesTrq(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsTrq, yarp::sig::Vector& actAxesTrq) { return false; }
bool HandMk5CouplingHandler::convertFromActuatedAxesToPhysicalJointsPos(const yarp::sig::Vector& actAxesPos, yarp::sig::Vector& physJointsPos) { return false; }
bool HandMk5CouplingHandler::convertFromActuatedAxesToPhysicalJointsVel(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesVel, yarp::sig::Vector& physJointsVel) { return false; }
bool HandMk5CouplingHandler::convertFromActuatedAxesToPhysicalJointsAcc(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesVel, const yarp::sig::Vector& actAxesAcc, yarp::sig::Vector& physJointsAcc) { return false; }
bool HandMk5CouplingHandler::convertFromActuatedAxesToPhysicalJointsTrq(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesTrq, yarp::sig::Vector& physJointsTrq) { return false; }
