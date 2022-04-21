# Screenshots from the Real-World Test
<p align="center"> <img src="screenshots/roof_1.png"> </p>
<p align="center"> <img src="screenshots/roof_2.png"> </p>
<p align="center"> <img src="screenshots/bonnet_1.png"> </p>
<p align="center"> <img src="screenshots/bonnet_2.png"> </p>

# Performance
<p align="center"> <img src="tests/graphs/fps_all.png"> </p>
<h4 align="center"> Desktop = Ryzen 5800x, GTX 960 4 GB, and 32 GB RAM </h4>
<h4 align="center"> Jetson Nano = Jetson Nano 4 GB (B01 Model) </h4>

# About
Lane and object detection for use in autonomous vehicles using OpenCV and YOLOv4. A YouTube playlist can be found [here](https://youtube.com/playlist?list=PLFJGOGaRWoxDm-Em5-mNv7JbYHmCRMm9b), check the description of each video for an explanation.

The software is run by instantiating the VideoManger class with a string to the input video file and then calling it's Run() method. If a camera input is desired, then pass the appropriate integer as a string.

The following optional parameters can be passed at instantiation:

`Enumerations::Detector &ObjectDetectorType`
* Enumerations::Detector::NONE = No object detection
* Enumerations::Detector::STANDARD = Standard object detection (default)
* Enumerations::Detector::TINY = Lower accuracy but higher FPS object detection

`Enumerations::BackEnd &BackEndType`
* Enumerations::BackEnd::CPU = CPU
* Enumerations::BackEnd::CUDA = NVIDIA CUDA (default)

`Enumerations::BlobSize &BlobSize`
* Enumerations::BlobSize::ONE = 288
* Enumerations::BlobSize::TWO = 320
* Enumerations::BlobSize::THREE = 416
* Enumerations::BlobSize::FOUR = 512
* Enumerations::BlobSize::FIVE = 608 (default)

`bool &RecordFrameTimes`
* true = Run() will return a std::vector<int> of time taken to compute every frame
* false = Run() will return nothing (default)

# Building [![CMake](https://github.com/J-Afzal/Lane-and-Object-Detection/workflows/CMake/badge.svg)](https://github.com/J-Afzal/Lane-and-Object-Detection/actions/workflows/cmake.yml)
For instructions on how to build the development environment, please refer to the `resources/README.md` file. To build the project, use the following CMake command in the project root directory:
```
cmake -S . -B build
```

# Warning
The yolo4.weights file could not be uploaded due to GitHub's 100 MB upload limit, but can be downloaded from [here](https://github.com/AlexeyAB/darknet/releases/download/darknet_yolo_v3_optimal/yolov4.weights) and should be copied to the `resources/yolo` folder.
