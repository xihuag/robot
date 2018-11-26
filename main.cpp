#include "global.h"
#include "GRobotConfig.h"
#include "tool.h"
#include "GRobotSetting.h"


using namespace std;
using namespace cv;

using namespace GRobot;

cv::VideoCapture camera1;
cv::VideoCapture camera2;
cv::VideoCapture camera3;

int main()
{

    cout << "What you want ? Please select:" << endl;
    cout << "[D] Do work" << endl;
    cout << "[L] Learn environment" << endl;
    cout << "[Q] Quit " << endl;

    char inputChar[256];
    cin.getline(inputChar,256);
    cout << "You chose : "<< inputChar << endl;

    char chSelected = inputChar[0];
    switch( chSelected ){
        case 'q':
        case 'Q':
            exit(0);
            break;
        case 'd':
        case 'D':
            //createTransparentImg();
            break;
        case '1': ;
        case 'L':
            GRobotSetting appSetter;
            appSetter.findCameras();//runGame();
            break;
    }

}


