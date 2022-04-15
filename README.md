# About
Lane and object detection that can be used in autonomous vehicles. A YouTube playlist can be found [here](https://youtube.com/playlist?list=PLFJGOGaRWoxDm-Em5-mNv7JbYHmCRMm9b) where each video has a description with an explanation.

# Building [![Build with CMake](https://github.com/J-Afzal/Lane-and-Object-Detection/actions/workflows/cmake.yml/badge.svg)](https://github.com/J-Afzal/Lane-and-Object-Detection/actions/workflows/cmake.yml)
For instructions on how to build the development environment, please refer to the `resources/README.md` file. To build the project use the following CMake command in the project root directory:
```
cmake -S . -B build
```

# Warning
The yolo4.weights file could not be uploaded due to GitHub's 100 MB upload limit but can be downloaded from [here](https://github.com/AlexeyAB/darknet/releases/download/darknet_yolo_v3_optimal/yolov4.weights) and should be copied to the `resources/yolo` folder.

# Running the software
The software is run by instantiating the VideoManger class and call it's Run() method.

# comment on webcam stuff

The following optional parameters can be passed at instantiation:
```c++
Enumerations::Detector &ObjectDetectorType
```
* `Enumerations::Detector::NONE` = No object detection
* `Enumerations::Detector::STANDARD` = Standard object detection (default)
* `Enumerations::Detector::TINY` = Lower accuracy but higher FPS object detection

```c++
Enumerations::BackEnd &BackEndType
```
* `Enumerations::BackEnd::CPU` = CPU
* `Enumerations::BackEnd::CUDA` = NVIDIA CUDA (default)

```c++
Enumerations::BlobSize &BlobSize
```
* `Enumerations::BlobSize::ONE` = 288
* `Enumerations::BlobSize::TWO` = 320
* `Enumerations::BlobSize::THREE` = 416
* `Enumerations::BlobSize::FOUR` = 512
* `Enumerations::BlobSize::FIVE` = 608

```c++
bool &RecordFrameTimes
``` 
* `true` = Run() will return an std::vector<int> of time taken to compute every frame
* `false` = Run() will return nothing

# Screenshots from the Real World Test
<p align="center"> <img src="screenshots/roof_1.png"> </p>
<p align="center"> <img src="screenshots/roof_2.png"> </p>
<p align="center"> <img src="screenshots/bonnet_1.png"> </p>
<p align="center"> <img src="screenshots/bonnet_2.png"> </p>

# Performance
<p align="center"> <img src="tests/graphs/fps_all.png"> </p>
<h4 align="center"> Desktop = Ryzen 5800x, GTX 960 4 GB, and 32 GB RAM </h4>
<h4 align="center"> Jetson Nano = Jetson Nano 4 GB (B01 Model) </h4>

# Components used in the Real World Test
* Jetson Nano 4 GB (B01 Model)
* HDMI cable for Jetson Nano output
* Micro-USB cable or barrel plug for power
* 64 GB Micro SD Card
* Wi-Fi Card (or use ethernet port)
* Camera:
  * GoPro Hero 5 Black
  * Suction Mount
  * micro HDMI to HDMI cable
  * HDMI to USB video capture device
  * Changed GoPro 'HDMI output' setting to 'Live'
* Waveshare Metal Case for the Jetson Nano
* Keyboard & Mouse
