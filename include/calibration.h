#ifndef CALIBRATION_H_INCLUDED
#define CALIBRATION_H_INCLUDED
#include <opencv2/core.hpp>

namespace GRobot{
    struct MatAndCorners
    {
        cv::Mat frameOriginal;
        cv::Mat frameGray;
        std::vector<cv::Point2f> corners;
    };
    void doCalibrate();
}
#endif // CALIBRATION_H_INCLUDED
