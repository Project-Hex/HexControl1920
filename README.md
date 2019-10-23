![img](https://github.com/3888winner/HexControl1920/blob/master/HexCover.jpg?raw=true)
# HexControl1920
Control Team Files for Project Hex 19/20

# Objectives
![img](https://github.com/3888winner/HexControl1920/blob/master/Mission.PNG?raw=true)
## Image Recognition

### Research
* What programming language should we use? (C++ or Rust?)
* How can we do it? (OpenCV?)
* How efficient is it? (Can it run on a pi?)

### Tasks
* 4 Ground Markers each with its own alphanumeric character (possibly color too)
* Must report back to the ground station the GPS co-ordinates, colour of the inner square and the alphanumeric character.

### How?
* Corner Detection?
* Edge Detection? (Last year's method)
* Color-filtering?
* Other options?

## Communication

### Research

* What programming language should we use? (python, c++ or mavlink commands?)
* How can we do it? (Mission planner auto or Direct MAVlink command)
* What hardware do we need? (Network cards)

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

## Image Recognition
* OpenCV Install: https://opencv.org/releases/
* Setting up OpenCV with Visual Studio 2017 (Windows): https://www.youtube.com/watch?v=Oi3HaBH8RC4
* Setting up OpenCV with Xcode (Mac): https://www.youtube.com/watch?v=o62iO8SssZk
* OpenCV 4.1.1 Tutorial: https://docs.opencv.org/4.1.1/d9/df8/tutorial_root.html 

## Communication / Drone Control
* Installing Python 2.7: https://www.python.org/downloads/release/python-2716/
* Beginner friendly Python IDE: https://thonny.org/
* Dronekit documentation: https://dronekit.netlify.com/
* Data transmission system: https://github.com/rodizio1/EZ-WifiBroadcast
* Alternatively: https://github.com/HD-Fpv/Open.HD/wiki

## General Guides & Info
* UAS Challenge Rules and Guidelines: https://www.imeche.org/events/challenges/uas-challenge/team-resources/challenge-document-library
* Github Guide: https://guides.github.com/activities/hello-world/
* Git install guide: https://git-scm.com/book/en/v2/Getting-Started-Installing-Git
* ArduCopter: http://ardupilot.org/copter/
* Mission Planner: http://ardupilot.org/planner/
* Qgroundcontrol: http://qgroundcontrol.com/

## Research & Examples
* https://www.pyimagesearch.com/2015/05/04/target-acquired-finding-targets-in-drone-and-quadcopter-video-streams-using-python-and-opencv/
* https://users.rust-lang.org/t/computer-vision-in-rust/16198/4
* https://github.com/mavlink/rust-mavlink
