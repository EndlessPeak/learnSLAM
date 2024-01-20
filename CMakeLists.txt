cmake_minimum_required(VERSION 3.21)
project(learnSLAM)

set(CMAKE_CXX_STANDARD 17)
find_package(Eigen3 REQUIRED)

add_subdirectory(Thirdparty/argparse)

add_executable(${PROJECT_NAME}
        src/eigen_matrix/eigen_matrix.cpp src/learnSLAM.cpp)

target_link_libraries(${PROJECT_NAME}
        Eigen3::Eigen
        argparse::argparse
        )