## About

The application, written in C++, aims to obtain information related to the entry and exit of vehicles in parking lots. Using various image processing techniques, the license plate number is identified in the image, segmented, rectified, read with the help of an AI model, and stored in a history log. 

## Build Requirements

* C++ 17
* CMake 3.16.0
* OpenCV 4.5.0
* Qt 6.2.4
* Tesseract 5.4.1

## Installation

1. Install [CMake](https://cmake.org/download/) (at least version 3.16.0)
2. Install [OpenCV](https://opencv.org/releases/) (at least version 4.5.0)
3. Install [Qt](https://www.qt.io/download) (at least version 6.2.4)
2. Install [Tesseract](https://tesseract-ocr.github.io/tessdoc/Compiling.html) (at least version 5.4.1)
4. Clone this repository
5. Build the project by running the `build.bat` file
6. Run the project generated in the `build` folder (e.g. `build\ParkingLotsSecurity.sln` for Visual Studio)

## Poster

![alt text](./Documentation/PatrascGeorgeDaniel_AFCO_2024.jpg)
