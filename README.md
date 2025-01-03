## About

The application, written in C++, aims to obtain information related to the entry and exit of vehicles in parking lots. Using various image processing techniques, the license plate number is identified in the image, segmented, rectified, read with the help of an AI model, and stored in a history log. 

## Build Requirements

* C++ 17
* CMake 3.16.0
* OpenCV 4.5.0
* Qt 6.2.4
* SQLCipher 4.6.1
* cpp-httplib 0.18.0
* POCO 1.5.1
* Tesseract 5.4.1
* libqrencode 4.1.1

## Installation

1. Install [CMake](https://cmake.org/download/) (at least version 3.16.0)
2. Install [OpenCV](https://opencv.org/releases/) (at least version 4.5.0)
3. Install [Qt](https://www.qt.io/download) (at least version 6.2.4)
4. Install [SQLCipher](https://github.com/sqlcipher/sqlcipher) (at least version 6.1)
5. Install [cpp-httplib](https://github.com/yhirose/cpp-httplib) (at least version 0.18.0)
6. Install [POCO](https://github.com/pocoproject/poco) (at least version 1.5.1)
7. Install [Tesseract](https://tesseract-ocr.github.io/tessdoc/Compiling.html) (at least version 5.4.1)
8. Install [libqrencode](https://github.com/fukuchi/libqrencode) (at least version 4.1.1)
9. Clone this repository
10. Build the project by running the `build.bat` file
11. Run the project generated in the `build` folder (e.g. `build\ParkingLotsSecurity.sln` for Visual Studio)

## Poster

![alt text](./Documentation/PatrascGeorgeDaniel_AFCO_2024.jpg)
