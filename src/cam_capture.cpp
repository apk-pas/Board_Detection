#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
#include<iostream>
using namespace cv;
using namespace std;

extern bool isDetected;
extern Point2f corners[4];

Mat origin_Process(Mat img);
void find(Mat edge,Mat img);
void processImage(Mat image);

void capture(VideoCapture video)
{
    Mat frame;
    while (1)
    {
        //从摄像头读取一帧图像
        video >> frame;
        Mat edge = origin_Process(frame);

        //找寻六边形
        find(edge,frame);

        //等待esc键退出
        if(waitKey(80)==27)
        {
            break;
        }
    }
}