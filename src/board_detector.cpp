#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
#include<iostream>
using namespace cv;
using namespace std;

void pnp(Point2f* corners,Mat& tvec,Mat& rvec);

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
        //circle(result,ctr,4,Scalar(0,255,0),-1);
        // circle(result,corners[0],4,Scalar(0,0,0),-1);
        // circle(result,corners[1],4,Scalar(255,0,0),-1);
        // circle(result,corners[2],4,Scalar(0,0,255),-1);
        // circle(result,corners[3],4,Scalar(255,255,255),-1);
        // circle(result,Point(0,0),8,Scalar(0,255,0),-1);
        
        Mat rvec;
        Mat tvec;
        pnp(corners,tvec,rvec);

        string tvecText = "tvec = [" + to_string((tvec).at<double>(0)) + ", " +
                      to_string((tvec).at<double>(1)) + ", " +
                      to_string((tvec).at<double>(2)) + "]";
        
        int fontFace = FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.8;
        int thickness = 2;

        // 设置文字颜色（BGR格式）
        Scalar color(255, 255, 255);  // 白色

        // 设置文本位置（左上角）
        Point position(10, 30);  // (x=10, y=30)

        // 在图像上绘制平移向量
        putText(result, tvecText, position, fontFace, fontScale, color, thickness);
        }

        imshow("result",result);
}