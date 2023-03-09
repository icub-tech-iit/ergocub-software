
project(emotionHandler)

find_package(YARP 3.7.2 REQUIRED)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PUBLIC emotionHandler.cpp emotionHandler.h)
target_link_libraries(${PROJECT_NAME} ${YARP_LIBRARIES})
install(TARGETS ${PROJECT_NAME} DESTINATION bin)