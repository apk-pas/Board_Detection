#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat origin_Process(Mat img);
int Match_Image(Mat img);
void find(Mat edge,Mat img);
void capture(VideoCapture video);
void tests(VideoCapture video);

int main()
{
    //打开摄像头并进行帧处理
    VideoCapture video(0);
    capture(video);
    return 0;
}