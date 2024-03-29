#include "camera_calibration.h"

/* @brief 设置关于相机标定部分的参数
 * @param argparse::ArgumentParser 解析器对象
 * */
int set_argument_calibrate_camera(argparse::ArgumentParser& parser){
    parser.add_description("Calibrate the camera with images.");
    // 添加带数据的参数
    parser.add_argument("-w","--width")
            .help("input chessboard width")
            .required()
            .scan<'i', int>()
            .default_value(int{9});

    parser.add_argument("-h","--height")
            .help("input chessboard height")
            .required()
            .scan<'i', int>()
            .default_value(int{6});

    parser.add_argument("-s","--square_size")
            .help("input chessboard square size")
            .required()
            .scan<'g', float>()
            .default_value(float{25.0});

    parser.add_argument("-l","--image_location")
            .help("locate the images folder.")
            .required()
            .default_value(std::string{"./images"});

    return 0;
}

/* @brief 解析关于相机标定部分的参数
 * @param argparse::ArgumentParser 解析器对象
 * */
int parse_argument_calibrate_camera(argparse::ArgumentParser& parser){
    try {
        auto width = parser.get<int>("--width");
        auto height = parser.get<int>("--height");
        auto square_size = parser.get<float>("--square_size");
        auto image_location = parser.get<std::string>("--image_location");
        CalibrateCamera(width, height, square_size,image_location);
    }
    catch (const std::exception& err){
        std::cerr << err.what() << "\n";
        std::cerr << parser << std::endl;
        // std::cout << "Error in Parser." << std::endl;
        return 1;
    }
    return 0;
}

/* @brief 对相机进行标定
 * @parm chessboard_width 棋盘格宽度
 * @parm chessboard_height 棋盘格高度
 * @parm square_size 棋盘格方块的实际尺寸（毫米）
 * @parm image_location 棋盘格图片文件夹位置
 * */
int CalibrateCamera(int chessboard_width,int chessboard_height,float square_size,std::string& image_location) {
    // 检查路径合法性
    std::filesystem::path images_path(image_location);
    if (!std::filesystem::exists(images_path)){
        std::cerr << images_path << " is not a valid path." << "\n";
        return -1; // 路径地址不存在
    }
    if (std::filesystem::is_regular_file(images_path)) {
        std::cerr << images_path << " is not a content." << "\n";
        return -1; // 路径地址为文件
    }

    // 获取当前目录路径
    // std::filesystem::path images_path = std::filesystem::current_path() / "images";

    // 读取指定目录下的所有图片文件
    // 保存图片的序列
    std::vector<cv::Mat> images;
    // 保存图片的类型
    std::vector<std::string> supported_extensions = {".png", ".jpg", ".jpeg", ".bmp", ".tiff", ".tif"};
    // C++11 基于范围的for循环
    // 使用const确保不修改条目
    // 使用&确保通过引用而不是拷贝来访问
    for (const auto& entry : std::filesystem::directory_iterator(images_path)) {
        // 依次判断每个文件是否是受支持的类型，如果为否，则判断下一个文件
        if (std::find(supported_extensions.begin(), supported_extensions.end(), \
        entry.path().extension().string()) == supported_extensions.end())
            continue;
        // 进行边缘判断 检查条目是否存在（在迭代过程中文件被删除了）以及是否是普通文件（不是目录）
        if (!std::filesystem::exists(entry.path()) || !std::filesystem::is_regular_file(entry.path())) {
            std::cerr << "File does not exist or is not a regular file: " << entry.path() << "\n";
            continue;
        }
        // 读取图片
        cv::Mat image = cv::imread(entry.path().string());
        if (!image.empty()) {
            // OpenCV的 cv::Mat 类设计用于高效处理图像数据，包括优化的复制和移动语义
            // 当一个 cv::Mat 对象被复制时，它通常使用引用计数来避免真实的数据复制，而是进行浅拷贝
            // 因此此处使用 emplace_back 不会有性能提升
            images.push_back(image);
        } else {
            std::cerr << "Failed to read image or image is empty: " << entry.path() << "\n";
        }
    }

    if (images.empty()){
        std::cerr << "Images folder don't have image." << images_path << "\n";
        return -1;
    }
    // 棋盘格角点坐标
    // 3D 世界坐标
    std::vector<std::vector<cv::Point3f>> object_points;
    // 2D 图像坐标
    std::vector<std::vector<cv::Point2f>> image_points;

    // 构建棋盘格角点的 3D 坐标
    // 对于每一张图像，只要找到了角点，则他们的棋盘格角点3D坐标相同。
    std::vector<cv::Point3f> obj;
    for(int i = 0; i < chessboard_height; i++) {
        for(int j = 0; j < chessboard_width; j++) {
            obj.emplace_back(j * square_size, i * square_size, 0);
        }
    }

    // 声明灰度图像
    cv::Mat gray_image;
    // 在所有图像中寻找棋盘格角点
    for(const auto& image : images) {
        cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);

        std::vector<cv::Point2f> corners;
        bool found = cv::findChessboardCorners(gray_image, cv::Size(chessboard_width, chessboard_height), corners);

        if(found) {
            // 为这张图像添加相同的3D世界坐标
            object_points.push_back(obj);
            // 为这张图像添加检测到的角点的2D图像坐标
            image_points.push_back(corners);
        }
    }

    // 相机标定
    // 相机内参矩阵和相机畸变向量
    cv::Mat camera_matrix, dist_coeffs;
    // 向量中记录第 i 幅图像相机坐标系到世界坐标系的旋转向量和平移向量
    std::vector<cv::Mat> rvecs, tvecs;
    cv::calibrateCamera(object_points, image_points, gray_image.size(), camera_matrix, dist_coeffs, rvecs, tvecs);

    // 输出相机内参
    std::cout << "Camera matrix:" << "\n";
    std::cout << camera_matrix << "\n";

    // 输出畸变参数
    std::cout << "Distortion coefficients:" << "\n";
    std::cout << dist_coeffs << "\n";

    // 计算平均重投影误差
    double totalError = 0;
    for (int i = 0; i < object_points.size(); i++) {
        std::vector<cv::Point2f> projectedPoints;
        cv::projectPoints(object_points[i], rvecs[i], tvecs[i], camera_matrix, dist_coeffs, projectedPoints);

        double error = cv::norm(image_points[i], projectedPoints, cv::NORM_L2);
        error /= (double) object_points[i].size();

        totalError += error;
    }

    double meanError = totalError / (double) object_points.size();

    // 输出平均重投影误差
    std::cout << "Mean reprojection error: " << meanError << std::endl;

    // 定义误差阈值
    double threshold = 0.05;
    return meanError - threshold < 0.0001 ? 0 : 1;
}
