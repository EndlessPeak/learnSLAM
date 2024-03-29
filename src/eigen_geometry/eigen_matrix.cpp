//
// Created by leesin on 1/20/24.
//
#include "eigen_matrix.h"

int eigen_matrix_display() {
    // Eigen 中所有向量和矩阵都是Eigen::Matrix，它是一个模板类。它的前三个参数为：数据类型，行，列
    // 声明一个2*3的float矩阵
    Eigen::Matrix<float, 2, 3> matrix_23;

    // 同时，Eigen 通过 typedef 提供了许多内置类型，不过底层仍是Eigen::Matrix
    // 例如 Vector3d 实质上是 Eigen::Matrix<double, 3, 1>，即三维向量
    Eigen::Vector3d v_3d;
    // 这是一样的
    Eigen::Matrix<float, 3, 1> vd_3d;

    // Matrix3d 实质上是 Eigen::Matrix<double, 3, 3>
    Eigen::Matrix3d matrix_33 = Eigen::Matrix3d::Zero(); //初始化为零
    // 如果不确定矩阵大小，可以使用动态大小的矩阵
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> matrix_dynamic;
    // 更简单的
    Eigen::MatrixXd matrix_x;
    // 这种类型还有很多，我们不一一列举

    // 下面是对Eigen阵的操作
    // 输入数据（初始化）
    matrix_23 << 1, 2, 3, 4, 5, 6;
    // 输出
    std::cout << "matrix 2x3 from 1 to 6: \n" << matrix_23 << "\n";

    // 用()访问矩阵中的元素
    std::cout << "print matrix 2x3: " << "\n";
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) std::cout << matrix_23(i, j) << "\t";
        std::cout << "\n";
    }

    // 矩阵和向量相乘（实际上仍是矩阵和矩阵）
    v_3d << 3, 2, 1;
    vd_3d << 4, 5, 6;

    // 但是在Eigen里你不能混合两种不同类型的矩阵，像这样是错的
    // Matrix<double, 2, 1> result_wrong_type = matrix_23 * v_3d;
    // 应该显式转换
    Eigen::Matrix<double, 2, 1> result = matrix_23.cast<double>() * v_3d;
    std::cout << "[1,2,3;4,5,6]*[3,2,1]=" << result.transpose() << "\n";

    Eigen::Matrix<float, 2, 1> result2 = matrix_23 * vd_3d;
    std::cout << "[1,2,3;4,5,6]*[4,5,6]: " << result2.transpose() << "\n";

    // 同样你不能搞错矩阵的维度
    // 试着取消下面的注释，看看Eigen会报什么错
    // Eigen::Matrix<double, 2, 3> result_wrong_dimension = matrix_23.cast<double>() * v_3d;

    // 一些矩阵运算
    // 四则运算就不演示了，直接用+-*/即可。
    matrix_33 = Eigen::Matrix3d::Random();                        // 随机数矩阵
    std::cout << "random matrix: \n" << matrix_33 << "\n";        // 矩阵本身
    std::cout << "transpose: \n" << matrix_33.transpose() << "\n";// 转置
    std::cout << "sum: " << matrix_33.sum() << "\n";              // 各元素和
    std::cout << "trace: " << matrix_33.trace() << "\n";          // 迹
    std::cout << "times 10: \n" << 10 * matrix_33 << "\n";        // 数乘
    std::cout << "inverse: \n" << matrix_33.inverse() << "\n";    // 逆
    std::cout << "det: " << matrix_33.determinant() << "\n";      // 行列式

    // 特征值
    // 实对称矩阵可以保证对角化成功
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigen_solver(matrix_33.transpose() * matrix_33);
    std::cout << "Eigen values = \n" << eigen_solver.eigenvalues() << "\n";
    std::cout << "Eigen vectors = \n" << eigen_solver.eigenvectors() << "\n";

    // 解方程
    // 我们求解 matrix_NN * x = v_Nd 这个方程
    // N的大小在前边的宏里定义，它由随机数生成
    // 直接求逆自然是最直接的，但是求逆运算量大

    Eigen::Matrix<double, matrix_size, matrix_size> matrix_NN
            = Eigen::MatrixXd::Random(matrix_size, matrix_size);
    matrix_NN = matrix_NN * matrix_NN.transpose();  // 保证半正定
    Eigen::Matrix<double, matrix_size, 1> v_Nd = Eigen::MatrixXd::Random(matrix_size, 1);

    clock_t time_stt = clock(); // 计时
    // 直接求逆
    Eigen::Matrix<double, matrix_size, 1> x = matrix_NN.inverse() * v_Nd;
    std::cout << "time of normal inverse is "
         << 1000 * (clock() - time_stt) / (double) CLOCKS_PER_SEC << "ms" << "\n";
    std::cout << "x = " << x.transpose() << "\n";

    // 通常用矩阵分解来求，例如QR分解，速度会快很多
    time_stt = clock();
    x = matrix_NN.colPivHouseholderQr().solve(v_Nd);
    std::cout << "time of Qr decomposition is "
         << 1000 * (clock() - time_stt) / (double) CLOCKS_PER_SEC << "ms" << "\n";
    std::cout << "x = " << x.transpose() << "\n";

    // 对于正定矩阵，还可以用cholesky分解来解方程
    time_stt = clock();
    x = matrix_NN.ldlt().solve(v_Nd);
    std::cout << "time of ldlt decomposition is "
         << 1000 * (clock() - time_stt) / (double) CLOCKS_PER_SEC << "ms" << "\n";
    std::cout << "x = " << x.transpose() << std::endl;

    return 0;
}