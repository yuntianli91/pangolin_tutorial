# Find Pangolin (https://github.com/stevenlovegrove/Pangolin)
find_package(Pangolin 0.5 REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(slam_visualization main.cpp slam_visualizer.cpp)
target_link_libraries(slam_visualization ${Pangolin_LIBRARIES} ${OpenCV_LIBS})
