#ifndef TOOL_H_INCLUDED
#define TOOL_H_INCLUDED
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

namespace GRobot{

bool openCamera(cv::VideoCapture& cap, int index);
bool inputInt(int * ptValue);
}
#endif // TOOL_H_INCLUDED
