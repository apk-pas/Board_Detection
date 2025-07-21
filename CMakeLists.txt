cmake_minimum_required(VERSION 3.10.0)
project(greenball_detection)

#set(OpenCV_DIR "D:\\Opencv\\opencv\\build\\x64\\cv\\install")
set(CMAKE_BUILD_TYPE Debug)
find_package(OpenCV REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(opencv ${SOURCES})

target_link_libraries(opencv PUBLIC ${OpenCV_LIBS})
target_include_directories(opencv PUBLIC ${OpenCV_INCLUDE_DIRS})
