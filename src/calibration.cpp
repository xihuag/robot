#include "opencv2/core.hpp"

#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"



#include <time.h>
#include "tool.h"
#include "calibration.h"
#include "GRobotConfig.h"

using namespace cv;
using namespace std;

//https://blog.csdn.net/eric_e/article/details/79570688

namespace GRobot{

    Camera::Camera():
        imageSize(0,0),
        /*
        images(vector<Mat>(16)),
        imagePoints(vector<vector<Point2f> >(16)),
        cameraMatrix(),                                    //[5]摄像机的内参数矩阵
        distCoeffs(),                                      //[6]摄像机的畸变系数向量
        rvecs(vector<Mat>(16)),
        tvecs(vector<Mat>(16)),
        reprojErrs(vector<float>(16)),*/
        totalAvgErr(-1)
    {

    }

    Camera::~Camera(){

    }


    Calibrater::Calibrater(int cameraCount)
    {
        m_cameraCount = cameraCount;
        if( m_cameraCount<1 ){
            m_cameraCount = 1;
        }

    }

    Calibrater::~Calibrater()
    {
    //dtor
    }


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

    void Calibrater::doCalibrate(){

        VideoCapture cap;
        int minValidFrameCnt = 16;

        int validFramesCnt = 0;
        Mat frame;
        Mat grayImgA,grayImgB;

        Size boardSize(5,7);
        int nSameLineLastPointIndex = boardSize.width * (boardSize.height-1);


        //vector<vector<Point2f> > points;

        char actionChar='\0';

        if( false==openCamera(cap,GRobotConfig::instance().mainCamera)){
            return;
        }

        cout << "=======================================" << endl;
        cout << "Actions: " << endl;
        cout << "[c] check current frame valid or not" << endl;
        cout << "[y] use this valid frame" << endl;
        cout << "[n] next valid frame" << endl;
        cout << "[z] zoom in" << endl;
        cout << "[e] end" << endl;
        cout << "=======================================" << endl;

        while( true ){
            cap >> frame;

            if( m_cameraA.imageSize.width==0 ){

                if( m_cameraCount>1 ){
                    m_cameraA.imageSize.width  = frame.size().width/2;
                    m_cameraA.imageSize.height = frame.size().height;

                    m_cameraA.inParentPosition.x = 0;
                    m_cameraA.inParentPosition.y = 0;
                    m_cameraA.inParentPosition.width = m_cameraA.imageSize.width;
                    m_cameraA.inParentPosition.height = m_cameraA.imageSize.height;

                    m_cameraB.imageSize.width = m_cameraA.imageSize.width ;
                    m_cameraB.imageSize.height = m_cameraA.imageSize.height;

                    m_cameraB.inParentPosition.x = frame.size().width - m_cameraB.imageSize.width;
                    m_cameraB.inParentPosition.y = 0;
                    m_cameraB.inParentPosition.width = m_cameraB.imageSize.width;
                    m_cameraB.inParentPosition.height = m_cameraB.imageSize.height;
                }else{
                    m_cameraA.imageSize.width  = frame.size().width;
                    m_cameraA.imageSize.height = frame.size().height;
                }
            }

            imshow( "Calibrate camera", frame);
            char actionChar = waitKey(2);

            if( actionChar=='e'){
                break;
            }
            if( actionChar !='c' ){
                continue;
            }

            Mat frameA,frameB;

            if( m_cameraCount>1 ){
                Mat mA(frame, m_cameraA.inParentPosition );
                mA.copyTo( frameA );
                cvtColor(frameA,grayImgA,cv::COLOR_BGR2GRAY);

                Mat mB( frame, m_cameraB.inParentPosition  );
                mB.copyTo( frameB );
                cvtColor(frameB,grayImgB,cv::COLOR_BGR2GRAY);

                //imshow( "Frame A", frameA);
                //imshow( "Frame B", frameB);
                waitKey( 5 );
            }else{
                frame.copyTo( frameA );
                cvtColor(frameA,grayImgA,cv::COLOR_BGR2GRAY);
            }

            vector<Point2f> cornersA(64);
            vector<Point2f> cornersB(64);

            bool ret =cv::findChessboardCorners(grayImgA,boardSize,cornersA,CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
            bool retB = true;
            if( m_cameraCount>1 ){
                retB = cv::findChessboardCorners(grayImgB,boardSize,cornersB,CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
            }



            if( ret && retB){

                cornerSubPix( grayImgA, cornersA, Size(11,11),Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));

                Mat hasCornersA,hasCornersB;
                frameA.copyTo(hasCornersA);
                drawChessboardCorners(hasCornersA, boardSize, cornersA, true );
                //cv::line(frame2,corners[0],corners[nSameLineLastPointIndex],Scalar(0,0,255) );
                imshow( "Calibrate camera", hasCornersA );

                if( m_cameraCount>1 ){
                    cornerSubPix( grayImgB, cornersB, Size(11,11),Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
                    frameB.copyTo( hasCornersB );
                    drawChessboardCorners(hasCornersB, boardSize, cornersB, true );
                    //cv::line(frame2,corners[0],corners[nSameLineLastPointIndex],Scalar(0,0,255) );
                    imshow( "Calibrate camera B", hasCornersB);
                }

                actionChar = waitKey(0);

                if( actionChar=='z'){
                    Mat bigImg;
                    cv::resize(hasCornersA,bigImg,Size(frameA.size().width * 2, frameA.size().height*2 ) );
                    imshow( "Calibrate camera", bigImg);


                    if( m_cameraCount>1 ){
                        Mat bigImgB;
                        cv::resize(hasCornersB,bigImgB,Size(frameB.size().width * 2, frameB.size().height*2 ) );
                        imshow( "Calibrate camera B", bigImgB);
                    }
                    actionChar = waitKey(0);
                }

                if( actionChar=='y'){
                    m_cameraA.imagePoints.push_back( cornersA );
                    m_cameraA.images.push_back(frameA);
                    //points.push_back(cornersA);

                    if( m_cameraCount>1 ){
                        m_cameraB.imagePoints.push_back( cornersB );
                        m_cameraB.images.push_back(frameB);
                    }

                    validFramesCnt++;
                    cout << "Total valid frames " << validFramesCnt << endl;
                }

            }else{

                cout << "Failed to find corners " << endl;
            }

        }

        cap.release();
        cv::destroyAllWindows();

        float squareSize  = 1.f;                                 //[3]棋盘格角点之间的距离
        float aspectRatio = 1.f;                                 //[4]长宽比



        runCalibration( m_cameraA.imagePoints,m_cameraA.imageSize,boardSize,CHESSBOARD,squareSize,aspectRatio,0,
                        m_cameraA.cameraMatrix,m_cameraA.distCoeffs,
                        m_cameraA.rvecs,m_cameraA.tvecs,
                        m_cameraA.reprojErrs,m_cameraA.totalAvgErr);
        if( m_cameraCount>1 ){
            runCalibration( m_cameraB.imagePoints,m_cameraB.imageSize,boardSize,CHESSBOARD,squareSize,aspectRatio,0,
                        m_cameraB.cameraMatrix,m_cameraB.distCoeffs,
                        m_cameraB.rvecs,m_cameraB.tvecs,
                        m_cameraB.reprojErrs,m_cameraB.totalAvgErr);
        }
        cout << "Do you want to show all captured frames? [y/n]" << endl;
        char inputChar[256];
        cin.getline(inputChar,256);

        bool reShowAll=false;
        if( inputChar[0]=='y'){
            reShowAll =true;
        }

        if( reShowAll ){
            for( int idx=0; idx<m_cameraA.images.size(); idx++ ){

                char wname[16];
                Mat undistortedMat;

                sprintf( wname,"Frame %d",idx);
                Mat originMat;
                m_cameraA.images[idx].copyTo( originMat );
                //cv::cvtColor(originMat,originMat,COLOR_GRAY2BGR );

                cv::undistort( originMat,undistortedMat,m_cameraA.cameraMatrix,m_cameraA.distCoeffs);

                vector<Point2f> cornersOneImage = m_cameraA.imagePoints[idx];
                Point p1;
                p1.x = (int)(cornersOneImage[0].x);
                p1.y = (int)(cornersOneImage[0].y);

                Point p2;

                p2.x = (int)(cornersOneImage[nSameLineLastPointIndex].x);
                p2.y = (int)(cornersOneImage[nSameLineLastPointIndex].y);

                Scalar color(0,255,0);
                cv::line( undistortedMat,p1,p2, color ,1);

                imshow( wname, undistortedMat );
            }
            cout << "Press any key to quit" << endl;
            waitKey(0);
        }
        else{
            cout << "Press any key to quit" << endl;
            waitKey(0);
        }

        cv::destroyAllWindows();
        cout << "finished!" << endl;
    }

}
