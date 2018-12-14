#ifndef GROBOTCONFIG_H
#define GROBOTCONFIG_H

namespace GRobot{
class GRobotConfig
{
    public:
        GRobotConfig();
        virtual ~GRobotConfig();

        int mainCamera;    //index of the main camera
        int secondCamera;  //index of the second camera
        int thirdCamera;


        int totalCameraCnt;

        static GRobotConfig* m_Config;

        static void locdConfig(GRobotConfig* config);
        static const char APP_CONFIG_FILE[];
        static GRobotConfig& instance();

        bool save(void);

        GRobotConfig clone(void);
    protected:
    private:

};
}
#endif // GROBOTCONFIG_H
