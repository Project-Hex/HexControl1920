CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

# Enable C++11
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Project Name
project (HexControl)

# Find OpenCV, OpenCV_DIR variable should be set to the
# absolute path to the directory containing OpenCVConfig.cmake
find_package(OpenCV REQUIRED)

file(GLOB_RECURSE Foo_SOURCES "../src/*.cpp")
file(GLOB_RECURSE Foo_HEADERS "../src/*.h")

set (Foo_INCLUDE_DIRS "")
foreach (_headerFile ${Foo_HEADERS})
	get_filename_component(_dir ${_headerFile} PATH)
	list (APPEND Foo_INCLUDE_DIRS)
endforeach()
list(REMOVE_DUPLICATES Foo_INCLUDE_DIRS ${Foo_INCLUDE_DIRS})

add_executable (HexControl ${Foo_SOURCES})

target_include_directories(HexControl PRIVATE ${Foo_INCLUDE_DIRS})
target_link_libraries(HexControl LINK_PRIVATE ${OpenCV_LIBS})
