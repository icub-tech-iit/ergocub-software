
project(emotionHandler)
cmake_minimum_required(VERSION 3.22)

find_package(YARP 3.7.2 REQUIRED)
find_package(OpenCV REQUIRED)
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PUBLIC emotionHandler.cpp emotionHandler.h main.cpp)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/images DESTINATION ${CMAKE_BINARY_DIR})

# Link the application with YARP and OpenCV libraries
target_link_libraries(${PROJECT_NAME} ${YARP_LIBRARIES} ${OpenCV_LIBS})
install(TARGETS ${PROJECT_NAME} DESTINATION bin)