cmake_minimum_required(VERSION 3.21)

project(Lane-and-Object-Detection)

set(CMAKE_CXX_STANDARD 20)

Set(OpenCV_DIR C:\\dev\\opencv\\build)

find_package(OpenCV REQUIRED)

include_directories(include)

add_executable(Main
        source/main.cpp
        include/pch.hpp
        include/helpers/VideoManager.hpp
        source/helpers/VideoManager.cpp
        include/detectors/ObjectDetector.hpp
        source/detectors/ObjectDetector.cpp
        include/detectors/LaneDetector.hpp
        source/detectors/LaneDetector.cpp
        include/helpers/Performance.hpp
        source/helpers/Performance.cpp
        include/helpers/RollingAverage.hpp
        source/helpers/RollingAverage.cpp
        )

target_link_libraries(Main ${OpenCV_LIBS})

add_executable(Performance_Tests
        tests/main.cpp
        include/pch.hpp
        include/helpers/PerformanceTests.hpp
        source/helpers/PerformanceTests.cpp
        include/helpers/VideoManager.hpp
        source/helpers/VideoManager.cpp
        include/detectors/ObjectDetector.hpp
        source/detectors/ObjectDetector.cpp
        include/detectors/LaneDetector.hpp
        source/detectors/LaneDetector.cpp
        include/helpers/Performance.hpp
        source/helpers/Performance.cpp
        include/helpers/RollingAverage.hpp
        source/helpers/RollingAverage.cpp
        )

target_link_libraries(Performance_Tests ${OpenCV_LIBS})
