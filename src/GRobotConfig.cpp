#include "GRobotConfig.h"
#include "ConfigLoader.h"
#include <iostream>

namespace GRobot{
//config file name
const char GRobotConfig::APP_CONFIG_FILE[]="./grobot.cnf";

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

GRobotConfig GRobotConfig::locdConfig(){
    GRobotConfig robotConfig;

    ConfigLoader loader( APP_CONFIG_FILE );

    robotConfig.mainCamera = loader.getInt("MainCameraIndex",-1);
    robotConfig.secondCamera = loader.getInt("SecondCameraIndex",-1);
    robotConfig.thirdCamera = loader.getInt("ThirdCameraIndex",-1);

    return robotConfig;

}
}
