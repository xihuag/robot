#include <iostream>
#include "tool.h"

using namespace cv;
using namespace std;

namespace GRobot{
bool openCamera(VideoCapture& cap, int index){
    cap.open(index);
    if(!cap.isOpened() ){
        cout << "Open camera [" << index << "] failed!" << endl;
        return false;
    }
    cout << "width:  " << cap.get( CAP_PROP_FRAME_WIDTH ) << endl;
    cout << "height: " << cap.get( CAP_PROP_FRAME_HEIGHT ) << endl;
    return true;
}
}
