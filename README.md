# Lane and Object Detection

Lane and object detection for use in autonomous vehicles using OpenCV and YOLOv7.

## Getting Started

Binaries can be found in [Releases](https://github.com/J-Afzal/Lane-and-Object-Detection/releases) or, if preferred, CMake can
be used to build the project from source either through the PowerShell helper function:

```text
Import-Module ./modules/Build.psd1

Build-CppCodeUsingCMake -Platform windows-latest -BuildType Release -BuildDirectory build -Parallel 8 -Verbose

./build/lane-and-object-detection/lane-and-object-detection OR ./build/lane-and-object-detection/lane-and-object-detection.exe
```

> [!WARNING]
> If on Windows make sure to run the above within a Visual Studio Developer Command Prompt otherwise you may see errors around
> `cl.exe` not being found.

or manually:

```text
git clone --recurse-submodules https://github.com/J-Afzal/Lane-and-Object-Detection.git
cd Lane-and-Object-Detection

# Build OpenCV
cd ./submodules/opencv/
cmake -S . -B ./build -D "CMAKE_BUILD_TYPE=Release" -D "BUILD_opencv_world=ON"
cmake --build ./build --config Release --parallel 8
cd ../..

# Build Lane and Object Detection
cmake -S . -B ./build -D "CMAKE_BUILD_TYPE=Release"
cmake --build ./build --config Release

./build/lane-and-object-detection/lane-and-object-detection OR ./build/lane-and-object-detection/lane-and-object-detection.exe
```

> [!WARNING]
> If on Windows make sure to add -G "NMake Makefiles" otherwise the required dlls may not be in the correct location.

Here are the full list of options for Lane and Object Detection:

```text
Usage: lane-and-object-detection --input ... --yolo-folder-path ... --object-detector-type ... [optional]

OPTIONS:

Generic Options:

  -h --help                       Display available options.

Required Options:

  -i --input                      File path or camera ID
  -y --yolo-folder-path           Path to the yolo folder

Optional options:

  -o --object-detector-type       One of: none, standard or tiny. (default = none)
  -b --object-detector-backend    One of: cpu or cuda (default = cpu)
  -s --object-detector-blob-size  One of: 208, 320, 416, 512 or 608 (default = 208)
```

> [!IMPORTANT]
> The `yolov7.weights` file could not be uploaded due to GitHub's 100 MB upload limit, but can be downloaded
> [here](https://github.com/AlexeyAB/darknet/releases/download/yolov4/yolov7.weights) and should be copied to the
> `./resources/yolo` folder.

## Documentation

Checkout the [documentation page](https://J-Afzal.github.io/Lane-and-Object-Detection) built using Doxygen and hosted using
Github pages.

![Documentation Homepage](./resources/screenshots/DocumentationHomepage.png)

## General Information

This program uses YOLOv7 for object detection and OpenCV for lane detection. OpenCV is also used for handling IO such as reading
and writing video file as well as display the output to a window.

The object detection is able to detect most road-related objects such as cars, buses, trucks, cyclists, pedestrians, traffic
lights, etc. The accuracy and speed of detection is dependent upon the available GPU compute. For higher quality detection
select a higher blob size. For more performant detection select a lower blob size, and/or select the tiny detector type and/or
CUDA backend.

The lane detection code is custom made for this program. The one sentence summary would be that it: creates an ROI frame to
isolate the road markings immediately in front of the car, runs the Canny algorithm to retrieve the edges, runs the Hough
transform to get the straight lines, de-noises and categorises the straight lines to determine the location of the current lane.

The program has two modes:

1. `Normal Mode` which only displays the current driving state and the detected left and right lane line types:

    ![Normal Mode](./resources/screenshots/NormalMode.png)

1. `Debug Mode` which additionally displays the current FPS, average FPS and the ROI, Canny and Hough intermediary frames.

    ![Debug Mode](./resources/screenshots/DebugMode.png)

Both modes support recording the output of the main frame.

![Recording Mode](./resources/screenshots/RecordingMode.png)

## CI / CD

[![Continuous Integration](https://github.com/J-Afzal/Lane-and-Object-Detection/actions/workflows/ContinuousIntegration.yml/badge.svg)](https://github.com/J-Afzal/Lane-and-Object-Detection/actions/workflows/ContinuousIntegration.yml)
[![Continuous Deployment](https://github.com/J-Afzal/Lane-and-Object-Detection/actions/workflows/ContinuousDeployment.yml/badge.svg)](https://github.com/J-Afzal/Lane-and-Object-Detection/actions/workflows/ContinuousDeployment.yml)

The continuous integration workflow runs against all commits on pull requests, builds the code, runs unit tests and performs
linting checks.

The continuous deployment workflow runs against all commits to main, builds the code and deploys the executables as a release.

## Development Setup

For development a few extra tools are needed to check for linting issues locally which include the
[`Linters`](https://github.com/J-Afzal/Linters) and [`OpenCV`](https://github.com/opencv/opencv) submodules:

```text
git clone --recurse-submodules https://github.com/J-Afzal/Lane-and-Object-Detection.git
```

The development dependencies are:

- Git
- PowerShell version >= 5
- npm dependencies via `npm install`
- clang-tidy >= 19 and clang-format >= 19
- CMake >= 3.20
- Ninja >= 1.12.1

All linting helper functions can be found in the [`Linters`](https://github.com/J-Afzal/Linters) submodule.

Any generator can be used to build the project but to prior to running `clang-tidy`/`clang-format` CMake must be configured
using a generator that creates a `compile_commands.json` file in the build directory (e.g. `-G "Ninja"`, `-G "NMake Makefiles"`,
etc)

On windows, clang-tidy and clang-format can be installed using the `LLVM-x.x.x-win64.exe` binary from the
[LLVM release page](https://github.com/llvm/llvm-project/releases/tag/llvmorg-19.1.6) or from
[chocolatey](https://community.chocolatey.org/packages/llvm) using `choco install llvm -y`.

### IDE

On Windows, Visual Studio 2022 can be used by opening the folder as a CMake project and Visual Studio Code can be used by
opening the folder through the `Developer PowerShell for VS` (otherwise you may see errors around cl.exe not being found).

<!--
x. Performance Tests
  - Add docs to python code
  - Move all performance test consts out of globals to remove dependency on globals.hpp
  - Add black formatting linting check in Linters

  - Get better dash cam video.
    - Both benchmark and all scenarios video
    - (https://www.youtube.com/watch?v=TUgfiNg06GQ and https://www.youtube.com/watch?v=-F-hrZKXM-k)

  - Test performance tests and graph generation (update readme docs on process)
    - Test on windows and also ubuntu and jetson nano? (update installation instructions)
    - Add below section after general information
        ### Performance Tests

        some info on tests and some screenshots
        dependencies = python and pip install

        python -m venv .venv/
        .\.venv\Scripts\activate
        pip install -r ./requirements.txt

        cli call to performance tests
        cli call to performance graphs

  - Performance heat map and any optimisations?
  - Maybe have CI which does perf tests??? How to combine all platforms in to one db/graph?) Upload artifacts?

x. Correct lane detection code
  - Debug lane detection code to make sure it is correct.

-->
