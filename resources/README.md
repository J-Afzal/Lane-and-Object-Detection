# Instructions for Setting up Development Environments
# Windows 10
0. Download all pre-requisites
    * Microsoft Visual Studio
    * CMake (Add to PATH)
    * CUDA 11.2
    * cuDNN 8.1.1

1. Download OpenCV 4.5.1 and OpenCV-contrib 4.5.1, unzip them, and create a build folder in the same directory

2. Open the CMake GUI and select the unzipped OpenCV 4.5.1 as source directory and the build folder as the build directory and select Microsoft Visual Studio as the compiler

3. Press configure and change:
    * BUILD_opencv_world=ON
    * OPENCV_EXTRA_MODULES_PATH to the modules folder in the unzipped OpenCV-contrib 4.5.1
    * WITH_CUDA=ON
    * OPENCV_DNN_CUDA=ON
    * ENABLE_FAST_MATH=ON
    * WITH_TBB=ON
    * OPENCV_ENABLE_NONFREE=ON

4. Press configure again and change:
    * CUDA_FAST_MATH=ON
    * For your GPU set the correct GPU arch number and GPU family name

5. Press configure, then generate, then open project

6. Build the BUILD and INSTALL solutions

7. For each Microsoft Visual Studio project, go to project properties:
    * VC++ directories
        * Include Directories
            * add {Path to build folder}\install\include
            * add C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.2\include
        * Library Directories
            * add {Path to build folder}\install\x64\vc16\lib
            * add C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.2\lib\x64
    * Linker
        * Additional Dependencies
            * add opencv_world451.lib
            * add cudart.lib

# Ubuntu / Jetson Nano 
0. Setup Jetson Nano (Skip for Ubuntu)
    * Download NVIDIA Jetpack SDK 4.5.1 from https://developer.nvidia.com/embedded/jetpack
    * Format and flash SD card with the downloaded file
        * Formatter = https://www.sdcard.org/downloads/formatter/sd-memory-card-formatter-for-windows-download/
        * Flasher = https://www.balena.io/etcher/
    * Boot up Jetson Nano and follow the Welcome prompts (make sure to choose MAXN mode)

### Open up the terminal and execute the following:
1. Download all pre-requisites and press enter whenever asked to continue: (OK if python wheels fails)

       sudo apt upgrade && sudo apt update && sudo apt install build-essential cmake pkg-config unzip yasm git checkinstall && sudo apt install libjpeg-dev libpng-dev libtiff-dev && sudo apt install libavcodec-dev libavformat-dev libswscale-dev libavresample-dev && sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev && sudo apt install libxvidcore-dev x264 libx264-dev libfaac-dev libmp3lame-dev libtheora-dev && sudo apt install libfaac-dev libmp3lame-dev libvorbis-dev && sudo apt install libopencore-amrnb-dev libopencore-amrwb-dev && sudo apt-get install libdc1394-22 libdc1394-22-dev libxine2-dev libv4l-dev v4l-utils && cd /usr/include/linux && sudo ln -s -f ../libv4l1-videodev.h videodev.h && cd ~ && sudo apt-get install libgtk-3-dev && sudo apt-get install libtbb-dev && sudo apt-get install libatlas-base-dev gfortran && sudo apt-get install libprotobuf-dev protobuf-compiler && sudo apt-get install libgoogle-glog-dev libgflags-dev && sudo apt-get install libgphoto2-dev libeigen3-dev libhdf5-dev doxygen && sudo apt-get install python3-dev python3-pip && sudo -H pip3 install -U pip numpy && sudo apt install python3-testresources


2. Add CUDA to the path (Ubuntu desktop will need to install CUDA and add it to the PATH)

       echo 'export PATH=/usr/local/cuda/bin${PATH:+:${PATH}}' >> ~/.bashrc && \
       echo 'export LD_LIBRARY_PATH=/usr/local/cuda/lib64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}' >> ~/.bashrc


3. Download jtop to enable jetson_clocks, adjust fan speed, and much more (Skip for Ubuntu Desktop)

       sudo -H pip install -U jetson-stats && \
       sudo systemctl restart jetson_stats.service && \
       sudo reboot

4. cd to Downloads, then download OpenCV and OpenCV-contrib, then unzip both, then create and cd into build folder:

       cd ~/Downloads && \
       wget -O opencv.zip https://github.com/opencv/opencv/archive/4.5.1.zip && \
       wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.5.1.zip && \
       unzip opencv.zip && \
       unzip opencv_contrib.zip && \
       mkdir build && cd build

5. Build OpenCV with CUDA (For the Jetson Nano make sure there is an extra 4 GB (so total 6 GB) of swap memory using jtop and then remove after compilation)

       cmake \
       -D CMAKE_BUILD_TYPE=RELEASE \
       -D CMAKE_INSTALL_PREFIX=/usr/local \
       -D WITH_TBB=ON \
       -D WITH_CUDA=ON \
       -D OPENCV_DNN_CUDA=ON \
       -D CUDA_ARCH_BIN=5.3 \
       -D ENABLE_FAST_MATH=1 \
       -D CUDA_FAST_MATH=1 \
       -D WITH_CUBLAS=1 \
       -D WITH_V4L=ON \
       -D WITH_OPENGL=ON \
       -D WITH_GSTREAMER=ON \
       -D OPENCV_GENERATE_PKGCONFIG=ON \
       -D OPENCV_PC_FILE_NAME=opencv.pc \
       -D OPENCV_ENABLE_NONFREE=ON \
       -D OPENCV_EXTRA_MODULES_PATH=/home/main/Downloads/opencv_contrib-4.5.1/modules \
       ../opencv-4.5.1 && \
       make -j4 && \
       sudo make install -j4 && \
       sudo sh -c 'echo "/usr/local/lib" > /etc/ld.so.conf.d/opencv.conf' && \
       sudo ldconfig
