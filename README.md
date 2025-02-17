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

## Documentation

Checkout the GitHub pages-hosted [documentation page](https://J-Afzal.github.io/Lane-and-Object-Detection) built using doxygen.

![Documentation Homepage](./resources/screenshots/DocumentationHomepage.png)

## General Information

TODO

## CI / CD

[![Continuous Integration](https://github.com/J-Afzal/Lane-and-Object-Detection/actions/workflows/ContinuousIntegration.yml/badge.svg)](https://github.com/J-Afzal/Lane-and-Object-Detection/actions/workflows/ContinuousIntegration.yml)
[![Continuous Deployment](https://github.com/J-Afzal/Lane-and-Object-Detection/actions/workflows/ContinuousDeployment.yml/badge.svg)](https://github.com/J-Afzal/Lane-and-Object-Detection/actions/workflows/ContinuousDeployment.yml)

The continuous integration workflow runs against all commits on pull requests, builds the code, runs unit tests and performs
linting checks.

The continuous deployment workflow runs against all commits to main, builds the code and deploys the executables as a release.

## Development Setup

For development a few extra tools are needed to check for linting issues locally. For this clone the repo with the
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

> [!IMPORTANT]
> The `yolo4.weights` file could not be uploaded due to GitHub's 100 MB upload limit, but can be downloaded
> [here](https://github.com/AlexeyAB/darknet/releases/download/darknet_yolo_v3_optimal/yolov4.weights) and should be copied to
> the `resources/yolo` folder.

### Notes

Any generator can be used to build the project but to run `clang-tidy`/`clang-format` CMake must be configured using a generator
that creates a `compile_commands.json` file in the build directory before running `clang-tidy`/`clang-format` (e.g.
`-G "Ninja"`, `-G "NMake Makefiles"`, etc)

On Windows, Visual Studio 2022 can be used by opening the folder as a CMake project and Visual Studio Code can be used by
opening the folder through the `Developer PowerShell for VS` (otherwise you may see errors around cl.exe not being found).

On windows, clang-tidy and clang-format can be installed using the `LLVM-x.x.x-win64.exe` binary from the
[LLVM release page](https://github.com/llvm/llvm-project/releases/tag/llvmorg-19.1.6) or from
[chocolatey](https://community.chocolatey.org/packages/llvm) using `choco install llvm -y`.

<!--

TODO

1. Clean up C++ lane detection code and supplementary code (ignore object detection)
    Add default CLI support to pass required paths (and add to readme)
    Move RollingAverage to within lane detection somehow?
    PrintToFrame should be video manager responsibility not other classes
    upgrade video manager to not have to create a new object to adjust settings (set properties?) (for performance tests)
    standardise error messages

    Why is NAN being used everywhere?
    param/var could be better G_VIDEO_WIDTH -> G_INPUT_VIDEO_WIDTH
    magic numbers everywhere
    clean up comments
    clean up rest of readme
    remove cv::scalar
    NO LINTs

x. Clean up C++ object detection code
   replace backend enum with // cv::dnn::Backend // cv::dnn::Target#
   param names don't match enum names
   magic numbers everywhere
   clean up comments
   clean up rest of readme
   remove cv::scalar

x. Upgrade to newer YOLO
    Any perf or accuracy increases?

x. Clean up performance test code (replace with C++ https://alandefreitas.github.io/matplotplusplus/ ) or delete

x. Implement C GUI window to encapsulate main code and performance test code with debug mode to show all stats (and roi/canny/hough line debug views) and option to adjust ROI?

x. Test with CUDA

-->
