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

bool inputInt(int * ptValue){
    int nRet =0;
    bool isNegative = false;
    char inputChar[256];
    for(int i=0;i<256;i++ ){
        inputChar[i]='\0';
    }

    cin.getline(inputChar,256);
    if( inputChar[0]=='\0'){
        return false;
    }
    for(int i=0;i<256;i++ ){
        if( inputChar[i]=='\0' ){
            break;
        }else if( inputChar[i]=='-' ){
            isNegative = true;
        }else if( inputChar[i]>='0' && inputChar[i] <='9' ){
            nRet+=nRet*10 + inputChar[i]-'0';
        }else{
            break;
        }
    }

    if(isNegative){
        *ptValue=-(nRet);

    }else{
        *ptValue= nRet;
    }

    return true;
}
}
