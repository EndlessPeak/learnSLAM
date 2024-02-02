//
// Created by leesin on 2/2/24.
//

#ifndef LEARNSLAM_CAMERA_CALIBRATION_H
#define LEARNSLAM_CAMERA_CALIBRATION_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <filesystem>

int CalibrateCamera(int chessboard_width,int chessboard_height,float square_size);

#endif //LEARNSLAM_CAMERA_CALIBRATION_H
