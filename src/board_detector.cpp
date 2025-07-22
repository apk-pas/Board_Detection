#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
#include<iostream>
using namespace cv;
using namespace std;

void origin_Process(Mat img)
{
    Mat gray;
    //图像转为灰度图
    cvtColor(img,gray,COLOR_BGR2GRAY);
    //imshow("gray",gray);

    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    for(int i=0;i<3;i++){
        //morphologyEx(gray,gray,MORPH_OPEN,kernel);
        erode(gray,gray,kernel);
    }
    for(int i=0;i<3;i++){
        //morphologyEx(gray,gray,MORPH_CLOSE,kernel);s
        dilate(gray,gray,kernel);
    }
    imshow("mor",gray);

    Mat edge;
    GaussianBlur(gray, gray, Size(5,5), 2, 2);
    Canny(gray,edge,150,100);
    // vector<vector<Point>> contours;//find contours
    // findContours(edge,contours,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
    // Mat temp = Mat::zeros(edge.size(),CV_8UC1);// build new Mat

    // for(size_t i = 0;i<contours.size();i++)
    // {
    //     double leng = arcLength(contours[i],false);
    //     if(leng>2)
    //     {
    //         drawContours(temp,contours,static_cast<int>(i),Scalar(255),1);
    //     }
    // }    
    imshow("edge",edge);

}
