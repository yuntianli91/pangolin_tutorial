# Find Pangolin (https://github.com/stevenlovegrove/Pangolin)
find_package(Pangolin 0.5 REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(SimplePlot main.cpp)
target_link_libraries(SimplePlot ${Pangolin_LIBRARIES} ${OpenCV_LIBS})
