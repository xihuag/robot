#include "GRobotConfig.h"
#include "ConfigLoader.h"
#include <iostream>

namespace GRobot{

//config file name
const char GRobotConfig::APP_CONFIG_FILE[]="./grobot.cnf";
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


    ConfigLoader loader( APP_CONFIG_FILE );

    robotConfig->mainCamera = loader.getInt("MainCameraIndex",-1);
    robotConfig->secondCamera = loader.getInt("SecondCameraIndex",-1);
    robotConfig->thirdCamera = loader.getInt("ThirdCameraIndex",-1);

    //return robotConfig;

}

const GRobotConfig& GRobotConfig::instance(){
    if( GRobotConfig::m_Config==0){
        m_Config = new GRobotConfig();
        GRobotConfig::locdConfig(m_Config);
    }
    return *m_Config;
}
}
