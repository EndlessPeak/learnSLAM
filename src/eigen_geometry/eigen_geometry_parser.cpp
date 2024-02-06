//
// Created by leesin on 2/6/24.
//
#include "eigen_geometry_parser.h"
#include "eigen_matrix.h"
/**
 * @brief 本部分代码用于解析和调用 Eigen 部分的使用代码
 */

/**
 * @brief 设置关于矩阵和位姿部分的参数
 * @param argparse::ArgumentParser 解析器对象
 * @return
 * */
int set_argument_eigen_geometry(argparse::ArgumentParser& parser) {
    parser.add_description("Display the eigen matrix and geometry examples.");
    // 添加可选参数
    parser.add_argument("-e","--use_eigen")
            .help("display the examples of how to use eigen matrix.")
            .default_value(false)
            .implicit_value(true);
    return 0;
}

/**
 * @brief 解析关于矩阵和位姿部分的参数
 * @param argparse::ArgumentParser 解析器对象
 * */
int parse_argument_eigen_geometry(argparse::ArgumentParser& parser) {
    if(parser["--use_eigen"] == true) {
        eigen_matrix_display();
        return 0;
    }
    // 所有可选项均未命中，打印帮助信息
    std::cerr << parser << std::endl;
    return -1;
}