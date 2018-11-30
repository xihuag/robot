#ifndef GROBOTSETTING_H
#define GROBOTSETTING_H

namespace GRobot{
class GRobotSetting
{
    public:
        GRobotSetting();
        virtual ~GRobotSetting();

        bool saveConfig();

        void doSetting(void);
    protected:
    private:
        void findCameras();
        void getPhotoOfDesktop();

};
}
#endif // GROBOTSETTING_H
