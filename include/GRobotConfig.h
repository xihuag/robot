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

        static GRobotConfig locdConfig();
        static const char APP_CONFIG_FILE[];
    protected:
    private:
};
}
#endif // GROBOTCONFIG_H
