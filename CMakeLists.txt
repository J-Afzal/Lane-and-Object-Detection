CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

PROJECT(Lane-and-Object-Detection)

SET(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Executable names used by the CI/CD workflows
IF(CI_CD)
    IF(APPLE)
        SET(executable_name "lane-and-object-detection-macos-latest")
        SET(executable_name_performance_tests "lane-and-object-detection-macos-latest-performance-tests")
    ELSEIF(UNIX AND NOT APPLE)
        SET(executable_name "lane-and-object-detection-ubuntu-latest")
        SET(executable_name_performance_tests "lane-and-object-detection-ubuntu-latest-performance-tests")
    ELSEIF(WIN32)
        SET(executable_name "lane-and-object-detection-windows-latest")
        SET(executable_name_performance_tests "lane-and-object-detection-windows-latest-performance-tests")
    ELSE()
        SET(executable_name "lane-and-object-detection")
        SET(executable_name_performance_tests "lane-and-object-detection-performance-tests")
    ENDIF()
ELSE()
    SET(executable_name "lane-and-object-detection")
    SET(executable_name_performance_tests "lane-and-object-detection-performance-tests")
ENDIF()

FIND_PACKAGE(OpenCV REQUIRED)

ADD_EXECUTABLE(
    ${executable_name}
    ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/VideoManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/detectors/LaneDetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/detectors/ObjectDetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/Performance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/RollingAverage.cpp
)

ADD_EXECUTABLE(
    ${executable_name_performance_tests}
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp
    source/helpers/PerformanceTests.cpp
    source/helpers/VideoManager.cpp
    source/detectors/LaneDetector.cpp
    source/detectors/ObjectDetector.cpp
    source/helpers/Performance.cpp
    source/helpers/RollingAverage.cpp
)

TARGET_LINK_LIBRARIES(${executable_name} ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(${executable_name_performance_tests} ${OpenCV_LIBS})

# TODO: add cop task to copy dlls and videos and yolo files
