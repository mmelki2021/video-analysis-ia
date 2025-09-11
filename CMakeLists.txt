cmake_minimum_required(VERSION 3.10)
project(VideoAnalysisIA)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} include)

add_executable(video_analysis
    src/main.cpp
    src/detector.cpp
    src/tracker.cpp
    src/analyzer.cpp
    src/visualizer.cpp
    src/exporter.cpp
)

target_link_libraries(video_analysis ${OpenCV_LIBS})
