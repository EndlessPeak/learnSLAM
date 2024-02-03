cmake_minimum_required(VERSION 3.21)
project(learnSLAM)

set(CMAKE_CXX_STANDARD 17)
find_package(Eigen3 REQUIRED)
find_package(OpenCV 4 REQUIRED)

add_subdirectory(Thirdparty/argparse)

include_directories(
        include/camera_calibration
)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/eigen_matrix/eigen_matrix.cpp
        src/camera_calibration/camera_calibration.cpp
        include/camera_calibration/camera_calibration.h
)

target_link_libraries(${PROJECT_NAME}
        Eigen3::Eigen
        ${OpenCV_LIBS}
        argparse::argparse
)