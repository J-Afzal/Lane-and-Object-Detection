<p align="center"> <img src="screenshots/output.gif"> </p>

# About
Lane and object detection for use in autonomous vehicles using OpenCV and YOLOv4. The software is run by instantiating the `VideoManger` class with a string to the input video file and yolo resources folder and then calling it's `Run()` method. If a camera input is desired, then pass the appropriate integer, as required by OpenCV, as a string.

A YouTube playlist can be found [here](https://youtube.com/playlist?list=PLFJGOGaRWoxDm-Em5-mNv7JbYHmCRMm9b), check the description of each video for an explanation. </p>

The following optional parameters can be passed at instantiation:

`ObjectDetectorType`
* Detector::NONE = No object detection
* Detector::STANDARD = Standard object detection (default)
* Detector::TINY = Lower accuracy but higher FPS object detection

`BackEndType`
* BackEnd::CPU = CPU
* BackEnd::CUDA = NVIDIA CUDA (default)

`BlobSize`
* BlobSize::ONE = 288
* BlobSize::TWO = 320
* BlobSize::THREE = 416
* BlobSize::FOUR = 512
* BlobSize::FIVE = 608 (default)

# Performance
<p align="center"> <img src="tests/graphs/fps_all.png"> </p>
<h4 align="center"> Desktop = Ryzen 5800x, GTX 960 4 GB, and 32 GB RAM </h4>
<h4 align="center"> Jetson Nano = Jetson Nano 4 GB (B01 Model) </h4>
<h4 align="center"> (For all performance graphs see the tests folder) </h4>

# Building [![CMake](https://github.com/J-Afzal/Lane-and-Object-Detection/workflows/CMake/badge.svg)](https://github.com/J-Afzal/Lane-and-Object-Detection/actions/workflows/cmake.yml)
For instructions on how to build the OpenCV and CUDA (optional) development environment, please refer to the `resources/README.md` file. To build the project, use the following CMake command in the project root directory:
```
cmake -S . -B build
```

# Warning
The yolo4.weights file could not be uploaded due to GitHub's 100 MB upload limit, but can be downloaded from [here](https://github.com/AlexeyAB/darknet/releases/download/darknet_yolo_v3_optimal/yolov4.weights) and should be copied to the `resources/yolo` folder.
