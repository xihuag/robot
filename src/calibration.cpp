#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#include <cctype>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "tool.h"
#include "calibration.h"
#include "GRobotConfig.h"

using namespace cv;
using namespace std;


namespace GRobot{

    VideoCapture cap;
    int minValidFrameCnt = 10;

    void doCalibrate(){
        vector<MatAndCorners> validFrames;
        int validFramesCnt = 0;
        Mat frame,frame2;
        Mat grayImg;

        Size cornersOfChessboard(5,7);

        char actionChar='\0';

        if( false==openCamera(cap,GRobotConfig::instance().mainCamera)){
            return;
        }

        cout << "=======================================" << endl;
        cout << "Actions: " << endl;
        cout << "[c] check current frame valid or not" << endl;
        cout << "[y] use this valid frame" << endl;
        cout << "[n] next valid frame" << endl;
        cout << "[e] end" << endl;
        cout << "=======================================" << endl;

        while( true ){
            cap >> frame;

            imshow( "Calibrate camera", frame);
            char actionChar = waitKey(2);

            if( actionChar=='e'){
                break;
            }
            if( actionChar !='c' ){
                continue;
            }

            frame.copyTo(frame2);
            cvtColor(frame2,grayImg,cv::COLOR_BGR2GRAY);

            vector<Point2f> corners;
            bool ret = findChessboardCorners( grayImg,
                                            cornersOfChessboard,
                                            corners,
                                            CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);

            if( ret ){

                drawChessboardCorners(frame2, cornersOfChessboard, corners, true );
                imshow( "Calibrate camera", frame2);
                actionChar = waitKey(0);

                if( actionChar=='y'){
                    Mat frameOriginal,frameGray;
                    frame2.copyTo( frameOriginal);
                    grayImg.copyTo( frameGray );
                    MatAndCorners item;
                    item.frameOriginal=frameOriginal;
                    item.frameGray =frameGray;
                    item.corners = corners;
                    validFrames.push_back(item);
                    validFramesCnt++;
                    cout << "Total valid frames " << validFramesCnt << endl;
                }

            }

        }

        cap.release();

        cout << "Do you want to show all captured frames? [y/n]" << endl;
        actionChar = waitKey(0);

        cv::destroyAllWindows();

        if( actionChar=='y'){
            int n=0;
            vector<MatAndCorners>::const_iterator it = validFrames.begin();
            while( it!=validFrames.end() ){
                n++;
                char wname[16];
                sprintf( wname,"Frame %d",n);
                imshow( wname, (*it).frameOriginal );
                waitKey(1);
                it++;
            }
        }
        waitKey(0);
        cout << "finished!" << endl;
    }

}
