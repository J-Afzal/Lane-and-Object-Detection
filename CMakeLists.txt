CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

PROJECT(Lane-and-Object-Detection)

SET(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Executable names used by the CI/CD workflows
SET(executable_name "lane-and-object-detection")
SET(performance_tests_executable_name ${executable_name}-performance-tests)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/tests/performance_tests)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/install/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/submodules/sqlite3)

SET(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/)
FIND_PACKAGE(OpenCV REQUIRED)

ADD_EXECUTABLE(
    ${executable_name}
    ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/VideoManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/FrameBuilder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/Performance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/detectors/ObjectDetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/detectors/LaneDetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/LaneAndObjectDetection.rc
)
TARGET_LINK_LIBRARIES(${executable_name} ${OpenCV_LIBS})

ADD_EXECUTABLE(
    ${performance_tests_executable_name}
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/VideoManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/FrameBuilder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/helpers/Performance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/detectors/ObjectDetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/detectors/LaneDetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/performance_tests/PerformanceTests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodules/sqlite3/sqlite3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/LaneAndObjectDetection.rc
)
TARGET_LINK_LIBRARIES(${performance_tests_executable_name} ${OpenCV_LIBS})

# Place the executable and opencv_world library into the same folder
SET_TARGET_PROPERTIES(${executable_name}                   PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${executable_name}/)
SET_TARGET_PROPERTIES(${performance_tests_executable_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${performance_tests_executable_name}/)

IF(NOT ${CONFIGURE_ONLY})
    IF(UNIX AND APPLE)
        IF(${CMAKE_BUILD_TYPE} STREQUAL "Release")
            CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/lib/libopencv_world.4.11.0.dylib ${CMAKE_BINARY_DIR}/${executable_name}/libopencv_world.4.11.0.dylib                   COPYONLY)
            CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/lib/libopencv_world.4.11.0.dylib ${CMAKE_BINARY_DIR}/${performance_tests_executable_name}/libopencv_world.4.11.0.dylib COPYONLY)
        ELSE()
            CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/lib/libopencv_world.4.11.0.dylib ${CMAKE_BINARY_DIR}/${executable_name}/libopencv_world.4.11.0.dylib                   COPYONLY)  # TODO: test
            CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/lib/libopencv_world.4.11.0.dylib ${CMAKE_BINARY_DIR}/${performance_tests_executable_name}/libopencv_world.4.11.0.dylib COPYONLY)
        ENDIF()
    ENDIF()

    IF(UNIX AND NOT APPLE)
        IF(${CMAKE_BUILD_TYPE} STREQUAL "Release")
            CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/lib/libopencv_world.so.4.11.0 ${CMAKE_BINARY_DIR}/${executable_name}/libopencv_world.so.4.11.0                   COPYONLY)
            CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/lib/libopencv_world.so.4.11.0 ${CMAKE_BINARY_DIR}/${performance_tests_executable_name}/libopencv_world.so.4.11.0 COPYONLY)
        ELSE()
            CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/lib/libopencv_world.so.4.11.0 ${CMAKE_BINARY_DIR}/${executable_name}/libopencv_world.so.4.11.0                   COPYONLY) # TODO: test
            CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/lib/libopencv_world.so.4.11.0 ${CMAKE_BINARY_DIR}/${performance_tests_executable_name}/libopencv_world.so.4.11.0 COPYONLY)
        ENDIF()
    ENDIF()

    IF(WIN32)
        IF(${CMAKE_BUILD_TYPE} STREQUAL "Release")
            CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/bin/opencv_world4110.dll ${CMAKE_BINARY_DIR}/${executable_name}/opencv_world4110.dll                   COPYONLY)
            CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/bin/opencv_world4110.dll ${CMAKE_BINARY_DIR}/${performance_tests_executable_name}/opencv_world4110.dll COPYONLY)
        ELSE()
            CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/bin/opencv_world4110d.dll ${CMAKE_BINARY_DIR}/${executable_name}/opencv_world4110d.dll                  COPYONLY)
            CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/submodules/opencv/build/bin/opencv_world4110d.dll ${CMAKE_BINARY_DIR}/${performance_tests_executable_name}/opencv_world4110d.dll COPYONLY)
        ENDIF()
    ENDIF()
ENDIF()
