# Lane and Object Detection

Lane and object detection for use in autonomous vehicles using OpenCV and YOLOv4.

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

## Documentation

Checkout the [documentation page](https://J-Afzal.github.io/Lane-and-Object-Detection) built using Doxygen and hosted using
Github pages.

![Documentation Homepage](./resources/screenshots/DocumentationHomepage.png)

## General Information

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

> [!IMPORTANT]
> The `yolo4.weights` file could not be uploaded due to GitHub's 100 MB upload limit, but can be downloaded
> [here](https://github.com/AlexeyAB/darknet/releases/download/darknet_yolo_v3_optimal/yolov4.weights) and should be copied to
> the `resources/yolo` folder.

### IDE

On Windows, Visual Studio 2022 can be used by opening the folder as a CMake project and Visual Studio Code can be used by
opening the folder through the `Developer PowerShell for VS` (otherwise you may see errors around cl.exe not being found).

<!--
x. Clean up C++ lane and object detection code.

    Finish all TODO(Main)s (Linters check)
    Commit code

    Update General Information in readme

    Debug lane detection code to make sure it is correct.

x. Clean up performance test code (replace with python C++ https://alandefreitas.github.io/matplotplusplus/ ) or delete

x. clean up folder org (resources and tests)

x. Enhancements
    Upgrade to newer YOLO (and better performance?)
        Any perf or accuracy increases?
    Add info more discretely at the bottom of the frame?
    Press D for debug mode to display lane lines and driving state
        Also add the different frames at the top
        Also allow for ROI to be adjusted? And other variables?
            ROI dimensions need to be dynamic
            ROI line equations
            Canny algorithm thresholds
            Hough transform thresholds and properties
            All other lane detection thresholds
    Allow UI to be scaled up and down?

x. Test with CUDA? (regardless do performance heat map and optimisation)

x. Implement C GUI window to encapsulate main code and performance test code with debug mode to show all stats (and roi/canny
   /hough line debug views) and option to adjust ROI?
        Use Clay?
        where are std::cout displayed?
        run performance tests?
        display and export performance graphs?

x. Find better dash cam footage like american highway footage or m1 motorway footage?
-->
