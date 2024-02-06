//
// Created by leesin on 1/20/24.
//

#include <argparse/argparse.hpp>
#include "camera_calibration.h"
#include "eigen_geometry_parser.h"

int parse_argument(std::vector<std::unique_ptr<argparse::ArgumentParser>>& parsers, int argc, char* argv[]){
    // 获取主要 parser
    // 不能将其从 parser 集合中去除，否则后续所有子 parser 均未命中时，主要 parser 已被销毁，会报错
    argparse::ArgumentParser& main_parser = *parsers[0]; // 解引用 unique_ptr 获取对象引用

    // 使用主要 parser 解析
    try {
        main_parser.parse_args(argc, argv);
    }
    catch (const std::exception& err){
        std::cerr << err.what() << "\n";
        std::cerr << main_parser << std::endl;
        //std::cout << "Error in Parser." << std::endl;
        return 1;
    }

    // 获取子 parser
    // 解引用 unique_ptr 获取对象引用
    argparse::ArgumentParser& calibrate_camera_parser = *parsers[1];
    argparse::ArgumentParser& use_eigen_parser = *parsers[2];

    // 命中子 parser，使用子 parser 解析
    if (main_parser.is_subcommand_used("calibrate_camera")) {
        return parse_argument_calibrate_camera(calibrate_camera_parser);
    }
    else if (main_parser.is_subcommand_used("eigen_geometry")) {
        return parse_argument_eigen_geometry(use_eigen_parser);
    }

    // 所有子 parser 均未命中
    std::cerr << main_parser << std::endl;
    return -1;
}

int main(int argc,char *argv[]){
    // ArgumentParser 禁用了移动和拷贝构造函数
    // 如果需要在其他函数内使用 std::vector<argparse::ArgumentParser
    // 必须要用 unique_ptr 指针

    // 创建父 ArgumentParser
    auto program = std::make_unique<argparse::ArgumentParser>("learnSLAM","V0.1", \
    argparse::default_arguments::all);

    // 创建子 ArgumentParser
    // 禁用子命令的帮助和版本
    auto calibrate_camera =  std::make_unique<argparse::ArgumentParser>("calibrate_camera",\
    "V0.1", argparse::default_arguments::none);
    auto eigen_geometry = std::make_unique<argparse::ArgumentParser>("eigen_geometry", \
    "V0.1",argparse::default_arguments::none);

    // 设置子 parser
    // 向主 parser 添加子 parser
    set_argument_calibrate_camera(*calibrate_camera);
    (*program).add_subparser(*calibrate_camera);
    set_argument_eigen_geometry(*eigen_geometry);
    (*program).add_subparser(*eigen_geometry);

    std::vector<std::unique_ptr<argparse::ArgumentParser>> parsers;
    parsers.push_back(std::move(program));
    parsers.push_back(std::move(calibrate_camera));
    parsers.push_back(std::move(eigen_geometry));

    parse_argument(parsers,argc,argv);
}