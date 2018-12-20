#ifndef CALIBRATION_H_INCLUDED
#define CALIBRATION_H_INCLUDED
#include <opencv2/core.hpp>
#include <cctype>
#include <opencv2/core/utility.hpp>
#include <stdio.h>
#include <string.h>

namespace GRobot{
    enum Pattern { CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };

    class Camera
    {
    public:
        Camera();
        ~Camera();
        cv::Rect inParentPosition;
        cv::Size imageSize;
        std::vector<cv::Mat> images;
        std::vector<std::vector<cv::Point2f> > imagePoints;
        cv::Mat   cameraMatrix;                                      //[5]摄像机的内参数矩阵
        cv::Mat   distCoeffs;                                        //[6]摄像机的畸变系数向量
        std::vector<cv::Mat> rvecs;
        std::vector<cv::Mat> tvecs;
        std::vector<float> reprojErrs;
        double totalAvgErr;
    };

    class Calibrater
    {
    public:
        Calibrater(int cameraCount);
        virtual ~Calibrater();
        void doCalibrate();
    protected:
    private:
        std::vector<cv::Mat> images;
        int m_cameraCount;
        Camera m_cameraA;
        Camera m_cameraB;
    };

}
#endif // CALIBRATION_H_INCLUDED
