#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
#include<iostream>
using namespace cv;
using namespace std;

Mat origin_Process(Mat img);
void find(Mat edge,Mat img);
void processImage(Mat image);

void capture(VideoCapture video)
{
    Mat frame;
    while (1)
    {
        video >> frame;
        Mat edge = origin_Process(frame);

        find(edge,frame);
        //imshow("frame",frame);

        if(waitKey(80)==27)
        {
            break;
        }
        //processImage(frame);

    }
}