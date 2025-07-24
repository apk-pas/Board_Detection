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
    //GaussianBlur(hsv, hsv, Size(3,3), 0);
    inRange(hsv,Scalar(24,50,140),Scalar(43,255,255),hsv);
    //imshow("hsv",hsv);

    //腐蚀膨胀去噪点
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    for(int i=0;i<2;i++){ 
        dilate(hsv,hsv,kernel);
        //erode(hsv,hsv,kernel);
    }
    for(int i=0;i<2;i++){
        //dilate(hsv,hsv,kernel);
        erode(hsv,hsv,kernel);
    }
    morphologyEx(hsv,hsv,MORPH_CLOSE,kernel);
    morphologyEx(hsv,hsv,MORPH_OPEN,kernel);
    //imshow("mor",hsv);

    //利用高斯模糊降噪之后利用canny算子提取物体边缘
    Mat edge;
    GaussianBlur(hsv, hsv, Size(3,3), 2, 2);
    imshow("hsv",hsv);
    Canny(hsv,edge,150,100);
    //imshow("edge",edge);
    //imwrite("/home/summer/Board_Detection/photo/exp.png",edge);
    return edge;
}

int Match_Image(Mat img)
{
    Mat example = imread("/home/summer/Board_Detection/results/exp.png",0);

    //进行物体轮廓匹配
    double value = matchShapes(example,img,CONTOURS_MATCH_I1,0.1);
    cout << value << endl;
    double a = 0.1 ;//匹配的阈值
    if(value < a)
    {
        return 1;
    }
        return 0;
}

void find(Mat edge,Mat img)
{
    //找出所有轮廓
    vector<vector<Point>> contours;
    findContours(edge,contours,RETR_LIST,CHAIN_APPROX_SIMPLE);

    //涵盖所有六边形点
    vector<Point> allHexPoints;
    int key = 0;
    Mat result = img.clone();
    vector<vector<Point>> approxs;
    int a=0;

    for(size_t i = 0;i<contours.size();i++)
    {
        //检测轮廓面积大小
        double area = contourArea(contours[i]);
        if(area<300 || area>10000)
        continue;

        //对符合要求的轮廓进行拟合并检查是不是凹六边形
        vector<Point> cnt = contours[i];
        vector<Point> approx;
        approxPolyDP(cnt,approx,arcLength(cnt,true)*0.025,true);
        approxs.insert(approxs.end(),approx);
        drawContours(result,approxs,static_cast<int>(a),Scalar(255,0,0),4);
        if(approx.size()==6 && !isContourConvex(approx))
        {
            key++;
            allHexPoints.insert(allHexPoints.end(),approx.begin(),approx.end());
        }
        a++;
    }

    //cout << key << endl;
    
    if(!allHexPoints.empty())
    {
        //取最小外接矩形
        RotatedRect bounding = minAreaRect(allHexPoints);
        Point2f corners[4];
        Point2f ctr;
        bounding.points(corners);
        ctr = bounding.center;
        for(int i=0;i<4;i++)
        {
            if(i<3)
            line(result,corners[i],corners[i+1],Scalar(0,255,0),4);
            if(i==3)
            line(result,corners[i],corners[0],Scalar(0,255,0),4);
        }
        circle(result,ctr,4,Scalar(0,255,0),-1);
    }
    imshow("result",result);
}