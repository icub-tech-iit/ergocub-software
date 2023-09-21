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
    if(coupling_group_bottle.isNull())
    {
        yError()<<"HandMk5CouplingHandler: missing coupling parameters, check your configuration file";
        return false;
    }

    return parseFingerParameters(coupling_group_bottle);
}

bool HandMk5CouplingHandler::decouplePos (yarp::sig::Vector& current_pos)
{
    if (m_coupledJoints.size()!=m_couplingSize) return false;

    const yarp::sig::Vector tmp = current_pos;

    /* thumb_add <-- thumb_add */
    current_pos[m_coupledJoints[0]] = tmp[0];
    /* thumb_oc <-- thumb_prox */
    current_pos[m_coupledJoints[1]] = tmp[1];
    /* index_add <-- index_add */
    current_pos[m_coupledJoints[2]] = tmp[3];
    /* index_oc <-- index_prox */
    current_pos[m_coupledJoints[3]] = tmp[4];
    /* middle_oc <-- middle_prox */
    current_pos[m_coupledJoints[4]] = tmp[6];
    /**
     * ring_pinky_oc <-- pinkie_prox
     * as, on the real robot, the coupled group composed of ring_prox, ring_dist, pinkie_prox and pinkie_dist
     * is controlled using the encoder on the pinkie_prox as feedback
     */
    current_pos[m_coupledJoints[5]] = tmp[10];

    return true;
}

bool HandMk5CouplingHandler::decoupleVel (yarp::sig::Vector& current_vel)
{
    if (m_coupledJoints.size()!=m_couplingSize) return false;

    const yarp::sig::Vector tmp = current_vel;

    /* thumb_add <-- thumb_add */
    current_vel[m_coupledJoints[0]] = tmp[0];
    /* thumb_oc <-- thumb_prox */
    current_vel[m_coupledJoints[1]] = tmp[1];
    /* index_add <-- index_add */
    current_vel[m_coupledJoints[2]] = tmp[3];
    /* index_oc <-- index_prox */
    current_vel[m_coupledJoints[3]] = tmp[4];
    /* middle_oc <-- middle_prox */
    current_vel[m_coupledJoints[4]] = tmp[6];
    /**
    * ring_pinky_oc <-- pinkie_prox
    * as, on the real robot, the coupled group composed of ring_prox, ring_dist, pinkie_prox and pinkie_dist
    * is controlled using the encoder on the pinkie_prox as feedback
    */
    current_vel[m_coupledJoints[5]] = tmp[10];

    return true;
}

bool HandMk5CouplingHandler::decoupleAcc (yarp::sig::Vector& current_acc)
{
    /**
     * Acceleration control not available for fingers on the real robot.
     * Note: this method is never called within the controlboard plugin code.
     */
    return false;
}

bool HandMk5CouplingHandler::decoupleTrq (yarp::sig::Vector& current_trq)
{
    /**
     * Torque control not available for fingers on the real robot.
     */
    return false;
}

yarp::sig::Vector HandMk5CouplingHandler::decoupleRefPos (yarp::sig::Vector& pos_ref)
{
    if (m_coupledJoints.size()!=m_couplingSize) {yCError( HANDMK5COUPLINGHANDLER) << "HandMk5CouplingHandler: Invalid coupling vector"; return pos_ref;}

    yarp::sig::Vector out(pos_ref.size());

    /* thumb_add <-- thumb_add */
    out[0] = pos_ref[m_coupledJoints[0]];
    /* thumb_prox <-- thumb_oc */
    out[1] = pos_ref[m_coupledJoints[1]];
    /* thumb_dist <-- coupling_law(thumb_prox) */
    out[2] = evaluateCoupledJoint(out[1], "thumb");
    /* index_add <-- index_add */
    out[3] = pos_ref[m_coupledJoints[2]];
    /* index_prox <-- index_oc */
    out[4] = pos_ref[m_coupledJoints[3]];
    /* index_dist <-- coupling_law(index_prox) */
    out[5] = evaluateCoupledJoint(out[4], "index");
    /* middle_prox <-- middle_oc */
    out[6] = pos_ref[m_coupledJoints[4]];
    /* middle_dist <-- coupling_law(middle_prox) */
    out[7] = evaluateCoupledJoint(out[6], "middle");
    /* ring_prox <-- ring_pinky_oc */
    out[8] = pos_ref[m_coupledJoints[5]];
    /* ring_dist <-- coupling_law(ring_prox) */
    out[9] = evaluateCoupledJoint(out[8], "ring");
    /* pinky_prox <-- ring_pinky_oc */
    out[10] = pos_ref[m_coupledJoints[5]];
    /* pinky_dist <-- coupling_law(pinky_prox) */
    out[11] = evaluateCoupledJoint(out[10], "pinky");

    return out;
}


yarp::sig::Vector HandMk5CouplingHandler::decoupleRefVel (yarp::sig::Vector& vel_ref, const yarp::sig::Vector& pos_feedback)
{
    if (m_coupledJoints.size()!=m_couplingSize) {yCError( HANDMK5COUPLINGHANDLER) << "HandMk5CouplingHandler: Invalid coupling vector"; return vel_ref;}

    /**
     * Extract the current position of proximal joints from pos_feedback.
     */
    double lastThumbProx = pos_feedback[1];
    double lastIndexProx = pos_feedback[4];
    double lastMiddleProx = pos_feedback[6];
    double lastRingProx = pos_feedback[8];
    double lastPinkyProx = pos_feedback[10];

    /**
     * In the following, we use the fact that:
     * /dot{distal_joint} = \partial{distal_joint}{proximal_joint} \dot{proximal_joint}.
     */

    yarp::sig::Vector out(vel_ref.size());

    /* thumb_add <-- thumb_add */
    out[0] = vel_ref[m_coupledJoints[0]];
    /* thumb_prox <-- thumb_oc */
    out[1] = vel_ref[m_coupledJoints[1]];
    /* thumb_dist <-- coupling_law_jacobian(thumb_prox_position) * thumb_prox */
    out[2] = evaluateCoupledJointJacobian(lastThumbProx, "thumb") * out[1];
    /* index_add <-- index_add */
    out[3] = vel_ref[m_coupledJoints[2]];
    /* index_prox <-- index_oc */
    out[4] = vel_ref[m_coupledJoints[3]];
    /* index_dist <-- coupling_law_jacobian(index_prox_position) * index_prox */
    out[5] = evaluateCoupledJointJacobian(lastIndexProx, "index") * out[4];
    /* middle_prox <-- middle_oc */
    out[6] = vel_ref[m_coupledJoints[4]];
    /* middle_dist <-- coupling_law_jacobian(middle_prox_position) * middle_prox */
    out[7] = evaluateCoupledJointJacobian(lastMiddleProx, "middle") * out[6];
    /* ring_prox <-- ring_pinky_oc */
    out[8] = vel_ref[m_coupledJoints[5]];
    /* ring_dist <-- coupling_law_jacobian(ring_prox_position) * ring_prox */
    out[9] = evaluateCoupledJointJacobian(lastRingProx, "ring") * out[8];
    /* pinky_prox <-- ring_pinky_oc */
    out[10] = vel_ref[m_coupledJoints[5]];
    /* pinky_dist <-- coupling_law(pinky_prox) */
    out[11] = evaluateCoupledJointJacobian(lastPinkyProx, "pinky") * out[10];

    return out;
}

yarp::sig::Vector HandMk5CouplingHandler::decoupleRefTrq (yarp::sig::Vector& trq_ref)
{
    /**
     * Torque control not available for fingers on the real robot.
     */
    return trq_ref;
}

double HandMk5CouplingHandler::evaluateCoupledJoint(const double& q1, const std::string& finger_name)
{
    /**
     * Coupling law taken from from https://icub-tech-iit.github.io/documentation/hands/hands_mk5_coupling
     */
    auto params = mFingerParameters.at(finger_name);
    double q1_rad = q1 * M_PI / 180.0;
    double q1off_rad = params.q1off * M_PI / 180.0;
    double q2bias_rad = params.q2bias * M_PI / 180.0;

    double P1x_q1 = params.d * cos(q1_rad + q1off_rad);
    double P1y_q1 = params.d * sin(q1_rad + q1off_rad);

    double h_sq = std::pow(P1x_q1 - params.L0x, 2) + std::pow(P1y_q1 - params.L0y, 2);
    double h = std::sqrt(h_sq);
    double l_sq = std::pow(params.l, 2);
    double k_sq = std::pow(params.k, 2);

    double q2 = atan2(P1y_q1 - params.L0y, P1x_q1 - params.L0x) + \
        acos((h_sq + l_sq - k_sq) / (2.0 * params.l * h)) + \
        -q2bias_rad - M_PI;

    // The value of q1 is subtracted from the result as the formula provides
    // the absolute angle of the coupled distal joint with respect to the palm.
    return q2 * 180.0 / M_PI - q1;
}

double HandMk5CouplingHandler::evaluateCoupledJointJacobian(const double& q1, const std::string& finger_name)
{
    /**
     * Coupling law jacobian taken from from https://icub-tech-iit.github.io/documentation/hands/hands_mk5_coupling
     */
    auto params = mFingerParameters.at(finger_name);
    double q1_rad = q1 * M_PI / 180.0;
    double q1off_rad = params.q1off * M_PI / 180.0;

    double P1x_q1 = params.d * cos(q1_rad + q1off_rad);
    double P1y_q1 = params.d * sin(q1_rad + q1off_rad);

    double h_sq = std::pow(P1x_q1 - params.L0x, 2) + std::pow(P1y_q1 - params.L0y, 2);
    double h = std::sqrt(h_sq);
    double l_sq = std::pow(params.l, 2);
    double k_sq = std::pow(params.k, 2);

    double dq2_dq1_11 = 1;
    double dq2_dq1_21 = 2 - (std::pow(params.d, 2) - std::pow(params.b, 2)) / (std::pow(params.d, 2) - (params.L0x * P1x_q1 + params.L0y * P1y_q1));
    double dq2_dq1_12 = (params.L0x * P1y_q1 - params.L0y * P1x_q1) * (l_sq - k_sq - h_sq);
    double dq2_dq1_22 = 2 * params.l * h * h_sq * std::sqrt(1 - std::pow((l_sq - k_sq + h_sq) / (2 * params.l * h), 2));
    double dq2_dq1 = dq2_dq1_11 / dq2_dq1_21 + dq2_dq1_12 / dq2_dq1_22;

    // The value of 1 is subtracted from the result as evaluateCoupledJointJacobian provides
    // the jacobian of the absolute angle of the coupled distal joint.
    return dq2_dq1 - 1;
}