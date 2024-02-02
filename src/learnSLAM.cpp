//
// Created by leesin on 1/20/24.
//

#include <argparse/argparse.hpp>
#include "camera_calibration.h"

int set_argument_calibrate(argparse::ArgumentParser &parser){
    //add position argument
    parser.add_argument("-w","--width")
        .help("input chessboard width")
        .scan<'i', int>();

    parser.add_argument("-h","--height")
        .help("input chessboard height")
        .scan<'i', int>();

    parser.add_argument("-s","--square_size")
        .help("input chessboard square size")
        .scan<'g', float>();

    return 0;
}

int parse_argument(std::vector<std::unique_ptr<argparse::ArgumentParser>>& parsers, int argc, char* argv[]){
    // 获取主要 parser
    argparse::ArgumentParser& main_parser = *parsers[0]; // 解引用 unique_ptr 获取对象引用
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
    argparse::ArgumentParser& calibrate_parser = *parsers[1]; // 解引用 unique_ptr 获取对象引用
    if (main_parser.is_subcommand_used("calibrate_camera")) {
        try {
            auto width = calibrate_parser.get<int>("--width");
            auto height = calibrate_parser.get<int>("--height");
            auto square_size = calibrate_parser.get<float>("--square_size");
            CalibrateCamera(width, height, square_size);
        }
        catch (const std::exception& err){
            std::cerr << err.what() << "\n";
            std::cerr << calibrate_parser << std::endl;
            // std::cout << "Error in Parser." << std::endl;
            return 1;
        }
        return 0;
    }

    // 所有子 parser 均未命中
    std::cerr << main_parser << std::endl;
    return 1;
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

    set_argument_calibrate(*calibrate_camera);
    (*program).add_subparser(*calibrate_camera);

    std::vector<std::unique_ptr<argparse::ArgumentParser>> parsers;
    parsers.push_back(std::move(program));
    parsers.push_back(std::move(calibrate_camera));

    parse_argument(parsers,argc,argv);
}