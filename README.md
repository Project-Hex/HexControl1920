![img](https://github.com/3888winner/HexControl1920/blob/master/HexCover.jpg?raw=true)
# HexControl1920
Control Team Files for Project Hex 19/20

# Objectives
![img](https://github.com/3888winner/HexControl1920/blob/master/Mission.PNG?raw=true)
## Image Recognition

### Goals
* Make different test images
* Start developing based on test images

### Tasks
* 4 Ground Markers each with its own alphanumeric character (possibly color too)
* Must report back to the ground station the GPS co-ordinates, colour of the inner square and the alphanumeric character.

### How?
* Corner Detection?
* Edge Detection? (Last year's method)
* Color-filtering?
* Other options?

## Communication

### Goals
* Start using python and dronekit
* Start researching appropriate hardware

### Tasks

#### Core Mission Tasks
* Take-off
* Navigation and Cargo Delivery
* Return and Landing
* Option to replete cargo

#### Optional Mission Tasks
* Speed Trial
* Ground Marker Identification
* Area Search

# Getting Started

## Setup

### Windows

The project has a Visual Studio project ready to use. The only prerequisites are a local install of OpenCV.

#### OpenCV

Download the [OpenCV Binaries for Windows](https://opencv.org/releases/) and unpack them somewhere on your system. _(e.g. `C:\Program Files\opencv`)_

Set the `OPENCV_DIR` environment variable to the `.\build\x64\vc15` folder inside your OpenCV  installation directory. _(e.g. `C:\Program Files\opencv\build\x64\vc15`)_

Add the following entry to your `PATH` environment variable: `%OPENCV_DIR%\bin`

> Environment variables can be set under `Control Panel\System and Security\System\Advanced System Settings\Environment Variables...`.

The Visual Studio project will use the OpenCV install specified by your `OPENCV_DIR` environment variable.

## Image Recognition
* OpenCV Install: https://opencv.org/releases/
* Setting up OpenCV with Visual Studio 2017 (Windows): https://www.youtube.com/watch?v=Oi3HaBH8RC4
* Setting up OpenCV with Xcode (Mac): https://www.youtube.com/watch?v=o62iO8SssZk
* OpenCV 4.1.1 Tutorial: https://docs.opencv.org/4.1.1/d9/df8/tutorial_root.html 

## Communication / Drone Control

Install Python 2.7 from the [anaconda website](https://www.anaconda.com/distribution/#download-section). (Make sure that you note the install path)

Once installed, run the anaconda navigator, go to environments and create a new environment, making sure that Python 2.7 is selected.

After the environment has been created, run the anaconda powershell prompt, and type in the following:
```shell
conda activate name-of-the-environment-you-created
```
Make sure that (base) has now to changed to (name-of-your-environment), then type in:
```shell
pip install dronekit
pip install dronekit-sitl
conda install spyder-kernels=0.* 
```
Now you have installed dronekit on a python2.7 virtual environment!

To start programming run spyder(should have been installed with Anaconda).

Before you can start typing in code, you need to make sure that spyder is using the virtual environment you created.

To do that go to `Tools->Preferences->Python interpreter`, and select *Use the following python interpreter*.

Now browse to the location where anaconda is installed then go to `envs/name-of-your-env/python.exe`, for example:
```explorer
C:/Projects/Anaconda/envs/Hex/python.exe
```
Test if it worked by running:
```python
import dronekit, dronekit_sitl
```
If it executed with no problems then you're ready to start writing dronekit programs!

Make sure you install [Mission Planner](http://ardupilot.org/planner/), so you can view your simulation.

[Dronekit guide](https://www.youtube.com/watch?v=TFDWs_DG2QY&list=PLuteWQUGtU9BcXXr3jCG00uVXFwQJkLRa&index=1)

[Dronekit documentation](https://dronekit.netlify.com/guide/index.html)

Check out the dronekit examples on this repo.

## Antenna Tracker
* Setting up an antenna tracker: http://ardupilot.org/antennatracker/index.html
* Forum post: https://www.rcgroups.com/forums/showthread.php?2942118-Build-a-Reliable-and-Cheap-Antenna-Tracker-this-Weekend-%C2%96-by-zs6buj
* Git: https://github.com/zs6buj/AntTracker

## General Guides & Info
* UAS Challenge Rules and Guidelines: https://www.imeche.org/events/challenges/uas-challenge/team-resources/challenge-document-library
* Github Guide: https://guides.github.com/activities/hello-world/
* Git install guide: https://git-scm.com/book/en/v2/Getting-Started-Installing-Git
* ArduCopter: http://ardupilot.org/copter/
* Mission Planner: http://ardupilot.org/planner/
* Qgroundcontrol: http://qgroundcontrol.com/
* Data transmission system: https://github.com/rodizio1/EZ-WifiBroadcast
* Alternatively: https://github.com/HD-Fpv/Open.HD/wiki

## Research & Examples
* https://www.pyimagesearch.com/2015/05/04/target-acquired-finding-targets-in-drone-and-quadcopter-video-streams-using-python-and-opencv/
* https://users.rust-lang.org/t/computer-vision-in-rust/16198/4
* https://github.com/mavlink/rust-mavlink
