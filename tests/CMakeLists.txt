# Tests for generated models
# We use iDynTree & YARP for the tests
find_package(iDynTree REQUIRED)
find_package(YARP     REQUIRED)

add_executable(ergocub-model-test ergocub-model-test.cpp)
target_link_libraries(ergocub-model-test ${iDynTree_LIBRARIES} YARP::YARP_os)

macro(add_ergocub_model_test yarpRobotName)
    if (COMPILE_ergoCubModels)
        add_test(NAME ${yarpRobotName}ConsistencyCheck
             COMMAND ergocub-model-test --model ${CMAKE_BINARY_DIR}/${BUILD_PREFIX}/robots/${yarpRobotName}/model.urdf)
    endif()
endmacro()

# Model generated with simmechanics
add_ergocub_model_test(ergoCubGazeboSN000)
add_ergocub_model_test(ergoCubGazeboSN001)
add_ergocub_model_test(ergoCubGazeboSN003)
add_ergocub_model_test(ergoCubSN000)
add_ergocub_model_test(ergoCubSN001)
add_ergocub_model_test(ergoCubSN003)
