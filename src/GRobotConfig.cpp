#include "GRobotConfig.h"
#include <iostream>
#include <opencv2/opencv.hpp>

namespace GRobot{

//config file name
const char GRobotConfig::APP_CONFIG_FILE[]="./grobot.yml";
GRobotConfig* GRobotConfig::m_Config = 0;

GRobotConfig::GRobotConfig()
{
    mainCamera=-1;
    secondCamera=-1;
    thirdCamera=-1;
    totalCameraCnt =0 ;
}

GRobotConfig::~GRobotConfig()
{
    //dtor
}

void GRobotConfig::locdConfig(GRobotConfig* robotConfig){


    cv::FileStorage fs( APP_CONFIG_FILE, cv::FileStorage::READ );

    robotConfig->totalCameraCnt = (int)fs["totalCameraCnt"];
    robotConfig->mainCamera = (int)fs["MainCameraIndex"];
    robotConfig->secondCamera = (int)fs["SecondCameraIndex"];
    robotConfig->thirdCamera = (int)fs["ThirdCameraIndex"];

    //return robotConfig;

}

bool GRobotConfig::save( void ){
    cv::FileStorage fs( APP_CONFIG_FILE, cv::FileStorage::WRITE );

    fs << "totalCameraCnt" << totalCameraCnt;
    fs << "mainCamera" << mainCamera;
    fs << "secondCamera" << secondCamera;
    fs << "thirdCamera" << thirdCamera;

    //fs << "" << ;
   // fs << "" << ;
    //fs << "" << ;
    //fs << "" << ;
}

GRobotConfig GRobotConfig::clone(void){
    GRobotConfig newConf;

    newConf.totalCameraCnt = totalCameraCnt;
    newConf.mainCamera = mainCamera;
    newConf.secondCamera = secondCamera;
    newConf.thirdCamera = thirdCamera;



    return newConf;
}
GRobotConfig& GRobotConfig::instance(){
    if( GRobotConfig::m_Config==0){
        m_Config = new GRobotConfig();
        GRobotConfig::locdConfig(m_Config);
    }
    return *m_Config;
}
}
