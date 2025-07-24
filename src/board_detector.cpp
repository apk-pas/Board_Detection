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
    inRange(hsv,Scalar(24,50,140),Scalar(43,255,255),hsv);

    //腐蚀膨胀去噪点
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    for(int i=0;i<2;i++){ 
        dilate(hsv,hsv,kernel);
    }
    for(int i=0;i<2;i++){
        erode(hsv,hsv,kernel);
    }

    //闭运算和开运算去噪
    morphologyEx(hsv,hsv,MORPH_CLOSE,kernel);
    morphologyEx(hsv,hsv,MORPH_OPEN,kernel);

    //利用高斯模糊降噪
    Mat edge;
    GaussianBlur(hsv, hsv, Size(3,3), 2, 2);
    imshow("hsv",hsv);

    //Canny边缘检测
    Canny(hsv,edge,150,100);
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
            key++;  //计算六边形数量，便于后续pnp处理
            allHexPoints.insert(allHexPoints.end(),approx.begin(),approx.end());
        }
        a++;
    }
    
    if(!allHexPoints.empty())
    {
        //取四个六边形点的最小外接矩形
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
        
        //pnp提取平移向量和旋转向量
        Mat rvec;
        Mat tvec;
        pnp(corners,tvec,rvec);

        //标出平移向量tvec
        string tvecText = "tvec = [" + to_string((tvec).at<double>(0)) + ", " +
                      to_string((tvec).at<double>(1)) + ", " +
                      to_string((tvec).at<double>(2)) + "]";
        
        int fontFace = FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.8;
        int thickness = 2;
        Scalar color(255, 255, 255);
        Point position(10, 30);

        // 在图像上绘制平移向量
        putText(result, tvecText, position, fontFace, fontScale, color, thickness);
        }

        //显示结果
        imshow("result",result);
}