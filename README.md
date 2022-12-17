# Lane and Object Detection

## About

Lane and object detection for use in autonomous vehicles using OpenCV and YOLOv4. The software is run by instantiating the `VideoManger` class with a string to the input video file and yolo resources folder and then calling it's `Run()` method. If a camera input is desired, then pass the appropriate integer, as required by OpenCV, as a string.

## Output Videos

The `results` folder contains the following videos:

* `Pre-recorded Video Test` shows an example of lane detection on roads that the software was developed on and was for (motorways)
* `Real World Test (Roof FOV)` and `Real World Test (Bonnet FOV)` The bonnet camera location was able to detect the fainter/less defined road markings than the roof camera location, due to its closer proximity to the road, while the roof camera location gave less false lanes detections/less noisy
* `Software Analysis for No YOLOv4` shows the frame times for the software side by side with the output and that peaks occur during lane detection when many road markings picked up in the ROI frame
* `A Potential Curved Road Solution` shows a potential solution to detecting heavily curved roads. Compare this video with 'Software Analysis for No YOLOv4' in terms of lane detection and pay attention to how the ROI frame moves at the top left

## Output GIF

<p align="center"> <img src="screenshots/output.gif" width=1000> </p>

## Options

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

## Performance

<p align="center"> <img src="tests/graphs/fps_all.png"> </p>
<h4 align="center"> Desktop = Ryzen 5800x, GTX 960 4 GB, and 32 GB RAM </h4>
<h4 align="center"> Jetson Nano = Jetson Nano 4 GB (B01 Model) </h4>
<h4 align="center"> (For all performance graphs see the tests folder) </h4>

## Building [![CMake](https://github.com/J-Afzal/Lane-and-Object-Detection/workflows/CMake/badge.svg)](https://github.com/J-Afzal/Lane-and-Object-Detection/actions/workflows/cmake.yml)

For instructions on how to build the OpenCV and CUDA (optional) development environment, please refer to the `resources/README.md` file. To build the project, use the following CMake command in the project root directory:

``` cmd
cmake -S . -B build
```

## Warning

The yolo4.weights file could not be uploaded due to GitHub's 100 MB upload limit, but can be downloaded from [here](https://github.com/AlexeyAB/darknet/releases/download/darknet_yolo_v3_optimal/yolov4.weights) and should be copied to the `resources/yolo` folder.
