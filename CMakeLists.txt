CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

PROJECT(Lane-and-Object-Detection)

SET(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

SET(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/)
FIND_PACKAGE(OpenCV REQUIRED)

# Executable names used by the CI/CD workflows
SET(executable_name "lane-and-object-detection")
SET(executable_performance_tests_name ${executable_name}-performance-tests)

ADD_EXECUTABLE(
    ${executable_name}
    ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/VideoManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/FrameBuilder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/detectors/ObjectDetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/detectors/LaneDetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/Performance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/RollingAverage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/LaneAndObjectDetection.rc
)

TARGET_LINK_LIBRARIES(${executable_name} ${OpenCV_LIBS})
# Place the exe and opencv_world4110.dll in to the same folder
SET_TARGET_PROPERTIES(${executable_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${executable_name}/)
IF (NOT CONFIGURE_ONLY AND WIN32)
    IF(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/bin/${CMAKE_BUILD_TYPE}/opencv_world4110.dll ${CMAKE_BINARY_DIR}/${executable_name}/opencv_world4110.dll COPYONLY)
    ELSE()
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/bin/${CMAKE_BUILD_TYPE}/opencv_world4110d.dll ${CMAKE_BINARY_DIR}/${executable_name}/opencv_world4110d.dll COPYONLY)
    ENDIF()
ENDIF()

ADD_EXECUTABLE(
    ${executable_performance_tests_name}
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/VideoManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/FrameBuilder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/detectors/ObjectDetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/detectors/LaneDetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/Performance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/RollingAverage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/LaneAndObjectDetection.rc
)

TARGET_LINK_LIBRARIES(${executable_performance_tests_name} ${OpenCV_LIBS})
# Place the exe and opencv_world4110.dll in to the same folder
SET_TARGET_PROPERTIES(${executable_performance_tests_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${executable_performance_tests_name}/)
IF (NOT CONFIGURE_ONLY AND WIN32)
    IF(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/bin/${CMAKE_BUILD_TYPE}/opencv_world4110.dll ${CMAKE_BINARY_DIR}/${executable_performance_tests_name}/opencv_world4110.dll COPYONLY)
    ELSE()
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/bin/${CMAKE_BUILD_TYPE}/opencv_world4110d.dll ${CMAKE_BINARY_DIR}/${executable_performance_tests_name}/opencv_world4110d.dll COPYONLY)
    ENDIF()
ENDIF()
