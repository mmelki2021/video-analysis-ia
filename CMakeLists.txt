cmake_minimum_required(VERSION 3.10)
project(VideoAnalysisIA)

set(CMAKE_CXX_STANDARD 17)

# Option: utiliser ONNX Runtime pour l'inférence (contourne un bug OpenCV 4.6 avec certains ONNX)
set(ONNXRUNTIME_ROOT "" CACHE PATH "Répertoire racine d'ONNX Runtime (include/ et lib/)")
if(ONNXRUNTIME_ROOT)
  set(USE_ONNXRUNTIME 1)
  include_directories(${ONNXRUNTIME_ROOT}/include)
  link_directories(${ONNXRUNTIME_ROOT}/lib)
endif()

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

target_compile_definitions(video_analysis PRIVATE $<$<BOOL:${USE_ONNXRUNTIME}>:USE_ONNXRUNTIME=1>)
target_link_libraries(video_analysis ${OpenCV_LIBS})
if(USE_ONNXRUNTIME)
  target_link_libraries(video_analysis onnxruntime)
endif()
