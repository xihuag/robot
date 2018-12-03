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

//https://blog.csdn.net/eric_e/article/details/79570688

namespace GRobot{

    VideoCapture cap;
    int minValidFrameCnt = 10;

    double computeReprojectionErrors(
        const vector<vector<Point3f> >& objectPoints,
        const vector<vector<Point2f> >& imagePoints,
        const vector<Mat>&              rvecs,
        const vector<Mat>&              tvecs,
        const Mat&                      cameraMatrix,
        const Mat&                      distCoeffs,
        vector<float>&                  perViewErrors )
    {
        vector<Point2f> imagePoints2;
        int i              = 0;
        int totalPoints    = 0;
        double totalErr    = 0;                                                    //【1】单幅图像的平均误差
        double err         = 0;
        perViewErrors.resize(objectPoints.size());
        /*投影函数--对应OpenCv1.0版本中的cvProjectPoints2()函数---CCS--->ICS*/
        for( i = 0; i < (int)objectPoints.size(); i++ )
        {
            projectPoints(Mat(objectPoints[i]),                                    //【1】将要投影的摄像机坐标系下的三位点的坐标
                              rvecs[i],                                            //【2】平移矩阵
                          tvecs[i],                                            //【3】旋转矩阵
                                          cameraMatrix,                                        //【4】摄像机内参数矩阵
                          distCoeffs,                                          //【5】摄像机畸变向量(径向畸变,切向畸变k1,k2,k3,p1,p2)
                              imagePoints2);                                       //【6】对于摄像机三维物理坐标框架下的位置,我们计算出来的该三维点在成像仪中的坐标(像素坐标)

            err              = norm(Mat(imagePoints[i]), Mat(imagePoints2), CV_L2);//【7】两个数组对应元素差值平方的累加和
            int n            = (int)objectPoints[i].size();                        //【8】Vector向量的成员函数--resize(),size(),push_back(),pop_back()
            perViewErrors[i] = (float)std::sqrt(err*err/n);                        //【9】单个三维点的投影误差

            totalErr    += err*err;
            totalPoints += n;
        }

        return std::sqrt(totalErr/totalPoints);                                    //【10】摄像机投影的总体误差
    }

    void calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners, Pattern patternType = CHESSBOARD)
    {
        corners.resize(0);                                               //【1】Vector向量的成员函数,重置Vector向量的长度为0

        switch(patternType)                                              //【2】判断标定板的类型
        {
          case CHESSBOARD:                                               //【3】棋盘格类型的标定板
          case CIRCLES_GRID:                                             //【4】圆心型风格的标定板
            for( int i = 0; i < boardSize.height; i++ )
                for( int j = 0; j < boardSize.width; j++ )
                    corners.push_back(Point3f(float(j*squareSize),       //【5】将摄像机坐标系下的三维世界实际的标定板的物理坐标存储在corners这个vector向量容器类
                                              float(i*squareSize), 0));
            break;

          case ASYMMETRIC_CIRCLES_GRID:
            for( int i = 0; i < boardSize.height; i++ )
                for( int j = 0; j < boardSize.width; j++ )
                    corners.push_back(Point3f(float((2*j + i % 2)*squareSize),
                                              float(i*squareSize), 0));
            break;

          default:
            CV_Error(CV_StsBadArg, "Unknown pattern type\n");
        }
    }

    bool runCalibration( vector<vector<Point2f> > imagePoints,
                            Size                     imageSize,
                            Size                     boardSize,
                            Pattern                  patternType,
                            float                    squareSize,
                            float                    aspectRatio,
                            int                      flags,
                            Mat&                     cameraMatrix,
                            Mat&                     distCoeffs,
                            vector<Mat>&             rvecs,
                            vector<Mat>&             tvecs,
                            vector<float>&           reprojErrs,
                            double&                  totalAvgErr)
    {
        cameraMatrix = Mat::eye(3, 3, CV_64F);                                         //【1】摄像机内部参数矩阵,创建一个3*3的单位矩阵

        if( flags & CV_CALIB_FIX_ASPECT_RATIO ){
            cameraMatrix.at<double>(0,0) = aspectRatio;
        }
        distCoeffs = Mat::zeros(8, 1, CV_64F);                                         //【2】摄像机的畸变系数向量,创建一个8*1的行向量

        vector<vector<Point3f> > objectPoints(1);

        calcChessboardCorners(boardSize, squareSize, objectPoints[0], patternType);    //【3】计算棋盘格角点世界坐标系下的三维物理坐标

        objectPoints.resize(imagePoints.size(),objectPoints[0]);                       //【4】对objectPoints的Vector容器进行扩容,并且扩充的内存空间用元素objectPoints[0]填充
                                                                                       //【5】摄像机标定函数----计算摄像机的内部参数和外部参数
        double rms = calibrateCamera(objectPoints,      //【1】世界坐标系下*每张标定图片中的角点的总数k*图片的个数M---N*3矩阵(N=k*M)------物理坐标
                                     imagePoints,       //【2】imagePoints是一个N*2的矩阵,它由objectPoints所提供的所有点所对应点的像素坐标构成,
                                                              //如果使用棋盘格进行标定,那么，这个变量简单的由M次调用cvFindChessboardCorners()的返回值构成
                                     imageSize,         //【3】imageSize是以像素衡量的图像的尺寸Size,图像点就是从该图像中提取的
                                     cameraMatrix,      //【4】摄像机内部参数矩阵--------定义了理想摄像机的摄像机行为
                                     distCoeffs,        //【5】畸变系数行向量5*1---8*1---更多的表征了摄像机的非理想行为
                                     rvecs,             //【6】rotation_vectors----------旋转矩阵
                                     tvecs,             //【7】tanslation_vectors--------平移矩阵
                                     flags|CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);          ///*|CV_CALIB_FIX_K3*/|CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);

        printf("RMS error reported by calibrateCamera: %g\n", rms);

        bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);                  //【6】checkRange()函数---用于检查矩阵中的每一个元素是否在指定的一个数值区间之内

        totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,             //【7】完成摄像机标定后，对标定进行评价，计算重投影误差/摄像机标定误差
                    rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);               //【8】函数的返回值是摄像机标定/投影的总体平均误差

        return ok;
    }

    void doCalibrate(){
        vector<MatAndCorners> validFrames;
        int validFramesCnt = 0;
        Mat frame,frame2;
        Mat grayImg;

        Size boardSize(5,7);
        //Size boardSize(6,9);
        Size imageSize(0,0);

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

            if( imageSize.width==0 ){
                imageSize.width = frame.size().width;
                imageSize.height = frame.size().height;
            }

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
            bool ret =cv::findChessboardCorners(grayImg,boardSize,corners,CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
            /* findChessboardCorners( grayImg,
                                            cornersOfChessboard,
                                            corners,
                                            CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);*/

            if( ret ){

                cornerSubPix( grayImg, corners, Size(11,11),Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));

                drawChessboardCorners(frame2, boardSize, corners, true );
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
        cv::destroyAllWindows();

        cout << "Do you want to show all captured frames? [y/n]" << endl;
        char inputChar[256];
        cin.getline(inputChar,256);

        vector<vector<Point2f> > imagePoints;

        bool reShowAll=false;
        if( inputChar[0]=='y'){
            reShowAll =true;
        }

        int n=0;
        vector<MatAndCorners>::const_iterator it = validFrames.begin();
        while( it!=validFrames.end() ){
            imagePoints.push_back( (*it).corners );
            n++;
            if( reShowAll ){
                char wname[16];
                sprintf( wname,"Frame %d",n);
                imshow( wname, (*it).frameOriginal );
                waitKey(1);
            }
            it++;
        }

        float squareSize  = 1.f;                                 //[3]棋盘格角点之间的距离
        float aspectRatio = 1.f;                                 //[4]长宽比

        Mat   cameraMatrix;                                      //[5]摄像机的内参数矩阵
        Mat   distCoeffs;                                        //[6]摄像机的畸变系数向量
        vector<Mat> rvecs,tvecs;
        vector<float> reprojErrs;
        double totalAvgErr = 0;
        runCalibration( imagePoints,imageSize,boardSize,CHESSBOARD,squareSize,aspectRatio,0,
                        cameraMatrix,distCoeffs,
                        rvecs,tvecs,
                        reprojErrs,totalAvgErr);

        Mat undistortedMat;
        Mat originMat = (validFrames.begin())->frameGray;

        cv::undistort( originMat,undistortedMat,cameraMatrix,distCoeffs);

        vector<Point2f> cornersOneImage = imagePoints[0];
        Point p1;
        p1.x = (int)(cornersOneImage[0].x);

        Point p2;
        Scalar color(128,128,128);
        cv::line(undistortedMat,cornersOneImage[0],cornersOneImage[5], color );

        imshow( "undistorted ", undistortedMat );


        waitKey(0);


        cout << "Press any key to quit" << endl;
        cv::destroyAllWindows();
        cout << "finished!" << endl;
    }

}
