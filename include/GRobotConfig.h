#ifndef GROBOTCONFIG_H
#define GROBOTCONFIG_H

namespace GRobot{
class GRobotConfig
{
    public:
        GRobotConfig();
        virtual ~GRobotConfig();

        int mainCamera;
        int secondCamera;
        int thirdCamera;

        int totalCameraCnt;

        static GRobotConfig* m_Config;

        static void locdConfig(GRobotConfig* config);
        static const char APP_CONFIG_FILE[];
        static const GRobotConfig& instance();
    protected:
    private:

};
}
#endif // GROBOTCONFIG_H
