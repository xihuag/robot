#include "GRobotSetting.h"
#include "GRobotConfig.h"
#include "ConfigLoader.h"
#include "tool.h"

using namespace std;
using namespace cv;

namespace GRobot{
GRobotSetting::GRobotSetting()
{
    //ctor
}

GRobotSetting::~GRobotSetting()
{
    //dtor
}


void GRobotSetting::findCameras(){
    VideoCapture cap;
    Mat frame;
    int maxCapCount = 10;
    int n=0;

    cout << "Set the main camera." << endl;

    GRobotConfig config;

    bool jump = false;
    while( n<maxCapCount ){
        if( openCamera(cap,n)){
            cout << "Is this camera is the main Camera? [y/n]" << endl;

            while( true ){
                cap >> frame;
                imshow("Camera",frame);
                char ch = waitKey(5);
                if( ch=='y' ){
                    config.mainCamera = n;
                    jump = true;
                    break;
                }
                if( ch=='n' ){
                    break;
                }
            }
            cap.release();
        }
        if( jump ){
            break;
        }
        n++;
    }

    n=0;
    jump = false;
    cout << "Set the second camera." << endl;

    while( n<maxCapCount ){
        if( config.mainCamera!=n && openCamera(cap,n)){
            cout << "Is this camera is the second Camera? [y/n]" << endl;

            while( true ){
                cap >> frame;
                imshow("Camera",frame);
                char ch = waitKey(5);
                if( ch=='y' ){
                    config.secondCamera = n;
                    jump = true;
                    break;
                }
                if( ch=='n' ){
                    break;
                }
            }
            cap.release();
        }
        if( jump ){
            break;
        }
        n++;
    }

    n=0;
    jump = false;
    cout << "Set the third camera." << endl;

    while( n<maxCapCount ){
        if( config.mainCamera!=n && config.secondCamera!=n && openCamera(cap,n)){
            cout << "Is this camera is the third Camera? [y/n]" << endl;
            bool jump = false;
            while( true ){
                cap >> frame;
                imshow("Camera",frame);
                char ch = waitKey(5);
                if( ch=='y' ){
                    config.thirdCamera = n;
                    jump = true;
                    break;
                }
            }
            if( jump ){
                break;
            }
        }
        cap.release();
        n++;
    }

    destroyWindow("Camera");

    cout << "\n\n===============================" << endl;
    cout << "Main camera is   " << config.mainCamera << endl;
    cout << "Second camera is " << config.secondCamera << endl;
    cout << "Third camera is  " << config.thirdCamera << endl;
    cout << "===============================\n" << endl;
    cout << "Will you save the new camera setting! [y/n]" << endl;

    char inputChar[256];
    cin.getline(inputChar,256);
    cout << "You chose : "<< inputChar[0] << endl;

    if( 'y'==inputChar[0] ){
        ConfigLoader loader( GRobotConfig::APP_CONFIG_FILE );
        loader.setInt("MainCameraIndex",config.mainCamera);
        loader.setInt("SecondCameraIndex",config.secondCamera);
        loader.setInt("ThirdCameraIndex",config.thirdCamera);
        //loader.setInt("");
    }
}
}
