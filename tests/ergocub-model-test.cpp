
#include <iDynTree/Core/Axis.h>
#include <iDynTree/KinDynComputations.h>
#include <iDynTree/Model/JointState.h>
#include <iDynTree/Model/Indices.h>
#include <iDynTree/Model/Model.h>
#include <iDynTree/Model/RevoluteJoint.h>

#include <iDynTree/ModelIO/ModelLoader.h>

#include <yarp/os/Property.h>
#include <cmath>
#include <cstdlib>

inline bool checkDoubleAreEqual(const double & val1,
                                const double & val2,
                                const double tol)
{
    if( std::fabs(val1-val2) > tol )
    {
        return false;
    }

    return true;
}


template<typename VectorType1, typename VectorType2>
bool checkVectorAreEqual(const VectorType1 & dir1,
                         const VectorType2 & dir2,
                         const double tol)
{
    if( dir1.size() != dir2.size() )
    {
        return false;
    }

    for(int i=0; i < dir1.size(); i++)
    {
        if( std::fabs(dir1(i)-dir2(i)) > tol )
        {
            return false;
        }
    }
    return true;
}

template<typename MatrixType1, typename MatrixType2>
bool checkMatrixAreEqual(const MatrixType1 & mat1,
                         const MatrixType2 & mat2,
                         const double tol)
{
    if( mat1.rows() != mat2.rows() ||
        mat1.cols() != mat2.cols() )
    {
        return false;
    }

    for(int i=0; i < mat1.rows(); i++)
    {
        for(int j=0; j < mat1.cols(); j++ )
        {
            if( std::fabs(mat1(i,j)-mat2(i,j)) > tol )
            {
                return false;
            }
        }
    }
    return true;
}

bool checkTransformAreEqual(const iDynTree::Transform & t1,
                            const iDynTree::Transform & t2,
                            const double tol)
{
    return checkMatrixAreEqual(t1.getRotation(),t2.getRotation(),tol) &&
           checkVectorAreEqual(t1.getPosition(),t2.getPosition(),tol);
}


bool getAxisInRootLink(iDynTree::KinDynComputations & comp,
                                 const std::string jointName,
                                 iDynTree::Axis & axisInRootLink)
{
    iDynTree::LinkIndex rootLinkIdx = comp.getFrameIndex("root_link");

    if( rootLinkIdx == iDynTree::FRAME_INVALID_INDEX )
    {
        std::cerr << "ergocub-model-test error: impossible to find root_link in model" << std::endl;
        return false;
    }

    iDynTree::JointIndex jntIdx = comp.getRobotModel().getJointIndex(jointName);

    if( jntIdx == iDynTree::JOINT_INVALID_INDEX )
    {
        std::cerr << "ergocub-model-test error: impossible to find " << jointName << " in model" << std::endl;
        return false;
    }

    iDynTree::LinkIndex childLinkIdx = comp.getRobotModel().getJoint(jntIdx)->getSecondAttachedLink();

    // Check that the joint are actually revolute as all the joints in iCub
    const iDynTree::RevoluteJoint * revJoint = dynamic_cast<const iDynTree::RevoluteJoint *>(comp.getRobotModel().getJoint(jntIdx));

    if( !revJoint )
    {
        std::cerr << "ergocub-model-test error: " << jointName << " is not revolute " << std::endl;
        return false;
    }

    if( !revJoint->hasPosLimits() )
    {
        std::cerr << "ergocub-model-test error: " << jointName << " is a continous joint" << std::endl;
        return false;
    }

    axisInRootLink = comp.getRelativeTransform(rootLinkIdx,childLinkIdx)*(revJoint->getAxis(childLinkIdx));

    return true;
}

bool checkBaseLink(iDynTree::KinDynComputations & comp)
{
    iDynTree::LinkIndex rootLinkIdx = comp.getFrameIndex("root_link");

    if( rootLinkIdx == iDynTree::FRAME_INVALID_INDEX )
    {
        std::cerr << "ergocub-model-test error: impossible to find root_link in model" << std::endl;
        return false;
    }

    iDynTree::LinkIndex base_linkIdx = comp.getFrameIndex("base_link");

    if( rootLinkIdx == iDynTree::FRAME_INVALID_INDEX )
    {
        std::cerr << "ergocub-model-test error: impossible to find base_link in model" << std::endl;
        return false;
    }

    if( comp.getRobotModel().getFrameLink(base_linkIdx) != rootLinkIdx )
    {
        std::cerr << "ergocub-model-test error: base_link is not attached to root_link" << std::endl;
        return false;
    }

    if( !checkTransformAreEqual(comp.getRobotModel().getFrameTransform(base_linkIdx),iDynTree::Transform::Identity(),1e-6) )
    {
        std::cerr << "ergocub-model-test error: base_link <---> root_link transform is not an identity" << std::endl;
        return false;
    }

    std::cerr << "ergocub-model-test : base_link test performed correctly " << std::endl;

    return true;
}

bool checkSolesAreParallel(iDynTree::KinDynComputations & comp)
{
    iDynTree::LinkIndex rootLinkIdx = comp.getFrameIndex("root_link");

    if( rootLinkIdx == iDynTree::FRAME_INVALID_INDEX )
    {
        std::cerr << "ergocub-model-test error: impossible to find root_link in model" << std::endl;
        return false;
    }

    iDynTree::LinkIndex l_sole = comp.getFrameIndex("l_sole");

    if( rootLinkIdx == iDynTree::FRAME_INVALID_INDEX )
    {
        std::cerr << "ergocub-model-test error: impossible to find frame l_sole in model" << std::endl;
        return false;
    }

    iDynTree::LinkIndex r_sole = comp.getFrameIndex("r_sole");

    if( rootLinkIdx == iDynTree::FRAME_INVALID_INDEX )
    {
        std::cerr << "ergocub-model-test error: impossible to find frame r_sole in model" << std::endl;
        return false;
    }

    iDynTree::Transform root_H_l_sole = comp.getRelativeTransform(rootLinkIdx,l_sole);
    iDynTree::Transform root_H_r_sole = comp.getRelativeTransform(rootLinkIdx,r_sole);

    // height of the sole should be equal
    double l_sole_height = root_H_l_sole.getPosition().getVal(2);
    double r_sole_height = root_H_r_sole.getPosition().getVal(2);

    if( !checkDoubleAreEqual(l_sole_height,r_sole_height,1e-5) )
    {
        std::cerr << "ergocub-model-test error: l_sole_height is " << l_sole_height << ", while r_sole_height is " << r_sole_height << " (diff : " << std::fabs(l_sole_height-r_sole_height) <<  " )"  << std::endl;
        return false;
    }

    // x should also be equal
    double l_sole_x = root_H_l_sole.getPosition().getVal(0);
    double r_sole_x = root_H_r_sole.getPosition().getVal(0);

    // The increased threshold is a workaround for https://github.com/robotology/icub-model-generator/issues/125
    if( !checkDoubleAreEqual(l_sole_x,r_sole_x, 2e-4) )
    {
        std::cerr << "ergocub-model-test error: l_sole_x is " << l_sole_x << ", while r_sole_x is " << r_sole_x << " (diff : " << std::fabs(l_sole_x-r_sole_x) <<  " )"  << std::endl;
        return false;
    }

    // y should be simmetric
    double l_sole_y = root_H_l_sole.getPosition().getVal(1);
    double r_sole_y = root_H_r_sole.getPosition().getVal(1);

    // The increased threshold is a workaround for https://github.com/robotology/icub-model-generator/issues/125
    if( !checkDoubleAreEqual(l_sole_y,-r_sole_y,1e-4) )
    {
        std::cerr << "ergocub-model-test error: l_sole_y is " << l_sole_y << ", while r_sole_y is " << r_sole_y << " while they should be simmetric (diff : " << std::fabs(l_sole_y+r_sole_y) <<  " )"  << std::endl;
        return false;
    }


    std::cerr << "ergocub-model-test : sole are parallel test performed correctly " << std::endl;

    return true;
}



bool checkAxisDirections(iDynTree::KinDynComputations & comp)
{

    std::vector<std::string> axisNames;
    std::vector<iDynTree::Direction> expectedDirectionInRootLink;
    axisNames.push_back("torso_roll");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-1,0,0));
    axisNames.push_back("l_hip_pitch");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0,-1,0));
    axisNames.push_back("r_hip_pitch");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0,-1,0));
    axisNames.push_back("r_hip_roll");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-1,0,0));
    axisNames.push_back("r_hip_yaw");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0,0,-1));
    axisNames.push_back("r_knee");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0,-1,0));
    axisNames.push_back("r_ankle_pitch");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0,1,0));
    axisNames.push_back("r_ankle_roll");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-1,0,0));
    axisNames.push_back("l_hip_roll");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(1,0,0));
    axisNames.push_back("l_hip_yaw");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0,0,1));
    axisNames.push_back("l_knee");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0,-1,0));
    axisNames.push_back("l_ankle_pitch");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0,1,0));
    axisNames.push_back("l_ankle_roll");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(1,0,0));
    axisNames.push_back("torso_pitch");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0,1,0));
    axisNames.push_back("torso_yaw");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0,0,-1));
    axisNames.push_back("l_shoulder_pitch");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.250563,0.935113,0.250563));
    axisNames.push_back("r_shoulder_pitch");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.250563,0.935113,-0.250563));
    axisNames.push_back("neck_pitch");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0,-1,0));
    axisNames.push_back("neck_roll");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(1,0,0));
    axisNames.push_back("neck_yaw");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-1.62555e-21,-1.1e-15,1));
    axisNames.push_back("r_shoulder_roll");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.961047,-0.271447,-0.0520081));
    axisNames.push_back("r_shoulder_yaw");
    expectedDirectionInRootLink.push_back(iDynTree::Direction( -0.116648,0.227771,0.966702));
    axisNames.push_back("r_elbow");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.250563,-0.935113,0.250563));
    axisNames.push_back("r_wrist_roll");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.961047,0.271447,0.0520081));
    axisNames.push_back("r_wrist_pitch");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.250563,-0.935113,0.250563));
    axisNames.push_back("r_wrist_yaw");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.116648,0.227771,0.966702));
    axisNames.push_back("r_thumb_add");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.329847,0.160871,0.930227));
    axisNames.push_back("r_thumb_prox");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.525416,0.838737,-0.143034));
    axisNames.push_back("r_thumb_dist");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.525416,0.838737,-0.143034));
    axisNames.push_back("r_index_add");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.250563,-0.935113,0.250563));
    axisNames.push_back("r_index_prox");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.947223,0.290266,0.136064));
    axisNames.push_back("r_index_dist");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.947223,0.290266,0.136064));
    axisNames.push_back("r_middle_prox");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.961047,0.271447,0.0520081));
    axisNames.push_back("r_middle_dist");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.961047,0.271447,0.0520081));
    axisNames.push_back("r_ring_prox");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.973762,0.226085,-0.0259272));
    axisNames.push_back("r_ring_dist");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.973762,0.226085,-0.0259272));
    axisNames.push_back("r_pinkie_prox");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.973762,0.226085,-0.0259272));
    axisNames.push_back("r_pinkie_dist");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.973762,0.226085,-0.0259272));
    axisNames.push_back("l_shoulder_roll");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.961047,-0.271447,0.0520081));
    axisNames.push_back("l_shoulder_yaw");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.116648,-0.227771,0.966702));
    axisNames.push_back("l_elbow");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.250563,-0.935113,-0.250563));
    axisNames.push_back("l_wrist_roll");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.961047,0.271447,-0.0520081));
    axisNames.push_back("l_wrist_pitch");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.250563,-0.935113,-0.250563));
    axisNames.push_back("l_wrist_yaw");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.116648,0.227771,-0.966702));
    axisNames.push_back("l_thumb_add");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.329847,0.160871,-0.930227));
    axisNames.push_back("l_thumb_prox");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.525416,0.838737,0.143034));
    axisNames.push_back("l_thumb_dist");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(0.525416,0.838737,0.143034));
    axisNames.push_back("l_index_add");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.250563,-0.935113,-0.250563));
    axisNames.push_back("l_index_prox");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.947223,0.290266,-0.136064));
    axisNames.push_back("l_index_dist");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.947223,0.290266,-0.136064));
    axisNames.push_back("l_middle_prox");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.961047,0.271447,-0.0520081));
    axisNames.push_back("l_middle_dist");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.961047,0.271447,-0.0520081));
    axisNames.push_back("l_ring_prox");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.973762,0.226085,0.0259272));
    axisNames.push_back("l_ring_dist");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.973762,0.226085,0.0259272));
    axisNames.push_back("l_pinkie_prox");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.973762,0.226085,0.0259272));
    axisNames.push_back("l_pinkie_dist");
    expectedDirectionInRootLink.push_back(iDynTree::Direction(-0.973762,0.226085,0.0259272));


    for(int i=0; i < axisNames.size(); i++)
    {
        std::string axisToCheck = axisNames[i];
        iDynTree::Axis axisInRootLink;
        iDynTree::Direction expectedDirection = expectedDirectionInRootLink[i];
        bool getAxisOk = getAxisInRootLink(comp,axisToCheck,axisInRootLink);

        if( !getAxisOk ) {
            return false;
        }

        if( !checkVectorAreEqual(axisInRootLink.getDirection(),expectedDirection,1e-5) )
        {
            std::cerr << "ergocub-model-test error:" << axisToCheck << " got direction of " << axisInRootLink.getDirection().toString()
                  << " instead of expected " << expectedDirection.toString() << std::endl;
            return false;
        }
    }

    return true;
}

/**
 * All the iCub have a odd and not null number of F/T sensors.
 */
bool checkFTSensorsAreOddAndNotNull(iDynTree::ModelLoader & mdlLoader)
{
    int nrOfFTSensors = mdlLoader.sensors().getNrOfSensors(iDynTree::SIX_AXIS_FORCE_TORQUE);

    if( nrOfFTSensors == 0 )
    {
        std::cerr << "ergocub-model-test error: no F/T sensor found in the model" << std::endl;
        return false;
    }

    if( nrOfFTSensors % 2 == 0 )
    {
        std::cerr << "ergocub-model-test : even number of F/T sensor found in the model" << std::endl;
        return false;
    }


    return true;
}

/**
 * All the iCub have a even and not null number of F/T sensors.
 */
bool checkFTSensorsAreEvenAndNotNull(iDynTree::ModelLoader & mdlLoader)
{
    int nrOfFTSensors = mdlLoader.sensors().getNrOfSensors(iDynTree::SIX_AXIS_FORCE_TORQUE);

    if( nrOfFTSensors == 0 )
    {
        std::cerr << "ergocub-model-test error: no F/T sensor found in the model" << std::endl;
        return false;
    }

    if( nrOfFTSensors % 2 == 1 )
    {
        std::cerr << "ergocub-model-test : odd number of F/T sensor found in the model" << std::endl;
        return false;
    }


    return true;
}


bool checkFTSensorIsCorrectlyOriented(iDynTree::KinDynComputations & comp,
                                      const iDynTree::Rotation& expected,
                                      const std::string& sensorName)
{
    // Depending on the ergocub model, the sensor could be absent
    if (!comp.model().isFrameNameUsed(sensorName))
    {
        return true;
    }

    iDynTree::Rotation actual = comp.getRelativeTransform("root_link", sensorName).getRotation();

    if (!checkMatrixAreEqual(expected, actual, 1e-3))
    {
        std::cerr << "ergocub-model-test : transform between root_link and " << sensorName << " is not the expected one, test failed." << std::endl;
        std::cerr << "ergocub-model-test : Expected transform : " << expected.toString() << std::endl;
        std::cerr << "ergocub-model-test : Actual transform : " << actual.toString() << std::endl;
        return false;
    }

    return true;
}


bool checkFTSensorsAreCorrectlyOriented(iDynTree::KinDynComputations & comp)
{

    iDynTree::Rotation rootLink_R_sensorFrameLeftArmExpected =
        iDynTree::Rotation(-0.250563, -0.961047, 0.116648,
                           -0.935113, 0.271447, 0.227771,
                           -0.250563, -0.0520081, -0.966702);
    iDynTree::Rotation rootLink_R_sensorFrameRightArmExpected =
        iDynTree::Rotation(-0.250563, 0.961047, 0.116648,
                            0.935113, 0.271447, -0.227771,
                            -0.250563, 0.0520081, -0.966702);

    iDynTree::Rotation rootLink_R_sensorFrameExpectedFoot =
        iDynTree::Rotation(-0.5, 0.866025, 0,
                           -0.866025, -0.5, 0,
                            0, 0, 1);

    iDynTree::Rotation rootLink_R_sensorFrameExpectedLeg =
        iDynTree::Rotation(-0.866025, -0.5, 0,
                           -0.5, 0.866025, 0,
                            0, 0, -1);

    iDynTree::Rotation rootLink_L_sensorFrameExpectedLeg =
        iDynTree::Rotation(-0.866025, 0.5, 0,
                           0.5, 0.866025, 0,
                            0, 0, -1);

    bool ok = checkFTSensorIsCorrectlyOriented(comp, rootLink_R_sensorFrameLeftArmExpected, "l_arm_ft_sensor");
    ok = checkFTSensorIsCorrectlyOriented(comp, rootLink_R_sensorFrameRightArmExpected, "r_arm_ft_sensor") && ok;
    ok = checkFTSensorIsCorrectlyOriented(comp, rootLink_L_sensorFrameExpectedLeg, "l_leg_ft_sensor") && ok;
    ok = checkFTSensorIsCorrectlyOriented(comp, rootLink_R_sensorFrameExpectedLeg, "r_leg_ft_sensor") && ok;
    ok = checkFTSensorIsCorrectlyOriented(comp, rootLink_R_sensorFrameExpectedFoot, "l_foot_rear_ft_sensor") && ok;
    ok = checkFTSensorIsCorrectlyOriented(comp, rootLink_R_sensorFrameExpectedFoot, "r_foot_rear_ft_sensor") && ok;
    ok = checkFTSensorIsCorrectlyOriented(comp, rootLink_R_sensorFrameExpectedFoot, "l_foot_front_ft_sensor") && ok;
    ok = checkFTSensorIsCorrectlyOriented(comp, rootLink_R_sensorFrameExpectedFoot, "r_foot_front_ft_sensor") && ok;
    return ok;
}

int main(int argc, char ** argv)
{
    yarp::os::Property prop;

    // Parse command line options
    prop.fromCommand(argc,argv);

    // Skip upper body tests (to support testing iCubHeidelberg01 model)
    bool skipUpperBody = prop.check("skipUpperBody");

    // Get model name
    if( ! prop.find("model").isString() )
    {
        std::cerr << "ergocub-model-test error: model option not found" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string modelPath = prop.find("model").asString().c_str();

    iDynTree::ModelLoader mdlLoader;
    mdlLoader.loadModelFromFile(modelPath);

    // Open the model
    iDynTree::KinDynComputations comp;
    const bool modelLoaded = comp.loadRobotModel(mdlLoader.model());

    if( !modelLoaded )
    {
        std::cerr << "ergocub-model-test error: impossible to load model from " << modelLoaded << std::endl;
        return EXIT_FAILURE;
    }

    iDynTree::Vector3 grav;
    grav.zero();
    iDynTree::JointPosDoubleArray qj(comp.getRobotModel());
    iDynTree::JointDOFsDoubleArray dqj(comp.getRobotModel());
    qj.zero();
    dqj.zero();

    comp.setRobotState(qj,dqj,grav);

    // Check axis
    if( !checkAxisDirections(comp) )
    {
        return EXIT_FAILURE;
    }
    // Check if base_link exist, and check that is a frame attached to root_link and if its
    // transform is the idyn
    if( !checkBaseLink(comp) )
    {
        return EXIT_FAILURE;
    }


    // Check if l_sole/r_sole have the same distance from the root_link
    if( !checkSolesAreParallel(comp) )
    {
        return EXIT_FAILURE;
    }

    // Now some test that test the sensors
    // The ft sensors orientation respect to the root_link are different to iCubV2 and they are under investigation.
    if( !checkFTSensorsAreEvenAndNotNull(mdlLoader) )
    {
        return EXIT_FAILURE;
    }

    if (!checkFTSensorsAreCorrectlyOriented(comp))
    {
        return EXIT_FAILURE;
    }


    std::cerr << "Check for model " << modelPath << " concluded correctly!" << std::endl;

    return EXIT_SUCCESS;
}
