#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
#include<iostream>
using namespace cv;
using namespace std;

Mat origin_Process(Mat img)
{
    //转hsv图像并提取出黄色
    Mat hsv;
    cvtColor(img,hsv,COLOR_BGR2HSV);
    inRange(hsv,Scalar(20,80,80),Scalar(40,255,255),hsv);
    imshow("hsv",hsv);

    //三次腐蚀三次膨胀去噪点
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    for(int i=0;i<1;i++){
        //morphologyEx(gray,gray,MORPH_OPEN,kernel);
        erode(hsv,hsv,kernel);
    }
    for(int i=0;i<1;i++){
        //morphologyEx(gray,gray,MORPH_CLOSE,kernel);s
        dilate(hsv,hsv,kernel);
    }
    //imshow("mor",hsv);

    //利用高斯模糊降噪之后利用canny算子提取物体边缘
    Mat edge;
    GaussianBlur(hsv, hsv, Size(3,3), 2, 2);
    Canny(hsv,edge,150,50);
    imshow("edge",edge);
    //imwrite("/home/summer/Board_Detection/photo/exp.png",edge);
    return edge;
}

int Match_Image(Mat img)
{
    Mat example = imread("/home/summer/Board_Detection/photo/exp.png",0);

    //进行物体轮廓匹配s
    double value = matchShapes(example,img,CONTOURS_MATCH_I1,0.5);
    cout << value << endl;
    double a = 0.8 ;//匹配的阈值
    if(value < a)
    {
        return 1;
    }
        return 0;
}