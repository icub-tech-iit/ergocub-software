/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef HANDMK5COUPLINGHANDLER_H
#define HANDMK5COUPLINGHANDLER_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IJointCoupling.h>

#include <unordered_map>
#include <vector>
#include <string>

/** TBD
 */
class HandMk5CouplingHandler : public yarp::dev::DeviceDriver,
                               public yarp::dev::IJointCoupling {
private:

    /**
     * Parameters from https://icub-tech-iit.github.io/documentation/hands/hands_mk5_coupling
     */
    struct FingerParameters
    {
        double L0x;
        double L0y;
        double q2bias;
        double q1off;
        double k;
        double d;
        double l;
        double b;
    };

    std::unordered_map<std::string, FingerParameters> mFingerParameters;

    struct Range {
        Range() : min(0), max(0){}
        double min;
        double max;
    };
    // These could go in a Basic class
    yarp::sig::VectorOf<int> m_coupledJoints;
    std::vector<std::string> m_coupledJointNames;
    std::unordered_map<int, Range> m_coupledJointLimits;
    unsigned int m_controllerPeriod;
    unsigned int m_couplingSize;

    /*
     * This method implements the law q2 = q2(q1) from
     * https://icub-tech-iit.github.io/documentation/hands/hands_mk5_coupling,
     * i.e., the absolute angle of the distal joint q2 with respect to the palm.
     *
     * The inputs q1 and the return value of the function are in degrees.
     */
    double evaluateCoupledJoint(const double& q1, const std::string& finger_name);

    /*
     * This method implements the law \frac{\partial{q2}}{\partial{q1}} from
     * https://icub-tech-iit.github.io/documentation/hands/hands_mk5_coupling,
     * i.e., the jacobian of the absolute angle of the distal joint q2 measured from the palm,
     * with respect to the proximal joint q1.
     *
     * The input q1 is in degrees.
     */
    double evaluateCoupledJointJacobian(const double& q1, const std::string& finger_name);

    bool parseFingerParameters(yarp::os::Bottle& hand_params);
public:
    HandMk5CouplingHandler();
    ~HandMk5CouplingHandler() override;


    virtual bool decouplePos(yarp::sig::Vector& current_pos);
    virtual bool decoupleVel(yarp::sig::Vector& current_vel);
    virtual bool decoupleAcc(yarp::sig::Vector& current_acc);
    virtual bool decoupleTrq(yarp::sig::Vector& current_trq) override;
    virtual yarp::sig::Vector decoupleRefPos(yarp::sig::Vector& pos_ref) override;
    virtual yarp::sig::Vector decoupleRefVel(yarp::sig::Vector& vel_ref, const yarp::sig::Vector& pos_feedback) override;
    virtual yarp::sig::Vector decoupleRefTrq(yarp::sig::Vector& trq_ref) override;

    // These could go in a Basic class
    virtual yarp::sig::VectorOf<int> getCoupledJoints() override { return m_coupledJoints; };
    virtual std::string getCoupledJointName(int joint) override { return m_coupledJointNames[joint]; };
    virtual bool checkJointIsCoupled(int joint) override { return m_coupledJoints[joint] != -1; };


    virtual void setCoupledJointLimit(int joint, const double& min, const double& max) override { m_coupledJointLimits[joint].min = min; m_coupledJointLimits[joint].max = max; };
    virtual void getCoupledJointLimit(int joint, double& min, double& max) override { min = m_coupledJointLimits[joint].min; max = m_coupledJointLimits[joint].max; };

    //DeviceDriver
    bool close() override;
    /**
        * Configure with a set of options.
        * @param config The options to use
        * @return true iff the object could be configured.
        */
    bool open(yarp::os::Searchable& config) override;
};

#endif // HANDMK5COUPLINGHANDLER_H