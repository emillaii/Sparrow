# Sparrow - Active Alignment Project

![alt text](https://raw.githubusercontent.com/emillaii/Sparrow/master/Sparrow.png)

> This project is used to develop a cutting edge camera module assemble machine based on latest software architecture. High performance are desirable for the Software Design,  Active Alignment Algorithm, Motion Control, Computer Vision and MIPI Grabbing system. 

### How to build
> Visual Stduio 2017 is used 

### Software need to be installed
> Since the rushing of the software development, a dirty way of calling subprocess (xxx.exe) is used...will find a better way for integrating the software instead of calling .exe. 
* [GNUPlot] - For graph plotting
* [MTFMapper] - For MTF calculation 

### Library Required For Building Sparrow 
Library is put in the google drive, unzip the .libs.zip and put under <Project-Dir>/Sapprow/Sparrow_App
All static library / .dll are built in **x64** 

* [Dothinkey Library - DTCCM2_SDK] - Image Grabber From Mobile Camera Module 
* [XT Motion Library] - Motion Libary 
* [mfc120_x64_dll] - For XT Motion Library (in case if error still popup for the missing .dll after installing VS2013 redistributed)
* [eigen] - Eigen is a C++ template library for linear algebra: matrices, vectors, numerical solvers, and related algorithms.
* [opencv] - Computer Vision library ( Recommend version: 3.1.0 + ) 