#ifndef GROBOTSETTING_H
#define GROBOTSETTING_H

namespace GRobot{
class GRobotSetting
{
    public:
        GRobotSetting();
        virtual ~GRobotSetting();

        bool saveConfig();
        void findCameras();
    protected:
    private:

};
}
#endif // GROBOTSETTING_H
