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

void GRobotSetting::doSetting(){
    bool stop = false;
    while(true){
        cout << "What you want ? Please select:" << endl;
        cout << "[0] Set camera index" << endl;
        cout << "[1] Take a photo of clean desktop (without any thing)" << endl;
        cout << "[x] End setting " << endl;

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
                findCameras();
                break;
            case '1':
                getPhotoOfDesktop();
                break;
        }

        if( stop==true ){
            break;
        }
    }
}


/*
  Photo of DeskTop is important.
  By comparing the current photo and Clean desktop photo,
  I can recognized thing be added to desktop easily.
*/
void GRobotSetting::getPhotoOfDesktop(){
   // GRobotSetting::
}

//Tell me which is the main camera, which is the seconde camera ...
void GRobotSetting::findCameras(){
    VideoCapture cap;
    Mat frame;
    int maxCapCount = 10;
    int n=0;

    cout << "Set the main camera." << endl;

    GRobotConfig& configSingleton = GRobotConfig::instance();
    GRobotConfig config = configSingleton.clone();

    bool jump = false;
    while( n<maxCapCount ){
        if( openCamera(cap,n)){
            cout << "Is this camera is the main Camera? [y/n]" << endl;

            while( true ){
                cap >> frame;
                imshow("Camera",frame);
                char ch = waitKey(5);
                //cout << ch << endl;
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
            destroyWindow("Camera");
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
            destroyWindow("Camera");
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
                if( ch=='n' ){
                    break;
                }
            }

            cap.release();
            destroyWindow("Camera");

        }

        if( jump ){
            break;
        }

        n++;
    }



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
        config.save();
        cout << "Camera index setting saved! " << endl;
    }
}
}
