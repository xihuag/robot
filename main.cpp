#include "global.h"
#include "GRobotConfig.h"
#include "tool.h"
#include "GRobotSetting.h"
#include "calibration.h"
#include "Tracker.h"


using namespace std;
using namespace cv;

using namespace GRobot;

cv::VideoCapture camera1;
cv::VideoCapture camera2;
cv::VideoCapture camera3;

int main()
{
    Tracker tracker;
    tracker.run();

    exit(0);

    //locad config
    GRobotConfig::instance();

    GRobotSetting appSetter;

    bool stop = false;
    while(true){
        cout << "What you want ? Please select:" << endl;
        cout << "[0] Do work" << endl;
        cout << "[1] Learn environment" << endl;
        cout << "[2] Calibration" << endl;
        cout << "[x] Quit " << endl;

        char inputChar[256];
        cin.getline(inputChar,256);
        cout << "You chose : "<< inputChar << endl;

        char chSelected = inputChar[0];
        switch( chSelected ){
            case 'x':
                stop = true;
                //exit(0);
                break;
            case '0':
                break;
            case '2':
                doCalibrate();
                break;
            case '1':
                appSetter.doSetting();
                break;

            default:
                ;
        }

        if( stop==true ){
            break;
        }
    }

}


