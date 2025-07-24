#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <fstream>
#include<sstream>
#include<opencv2/imgproc/types_c.h> 
#include <opencv2/core/utils/logger.hpp> //隐藏日志
 
using namespace std;
using namespace cv;
int main()
{
	cv::utils::logging::setLogLevel(utils::logging::LOG_LEVEL_SILENT);//不再输出日志
 
	//或
	//utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);//只输出错误日志
	int image_count = 12;//检测的图片数量
	Size image_size;
	Size board_size = Size(11, 8);//图片上棋盘格（标定板）的内角点个数（行、列的角点数）
	vector<Point2f> image_corners;//缓存每幅图像上检测到的角点
	vector<vector<Point2f>> corners_Seq;//保存检测到的所有角点
	vector<Mat> image_Seq;
	int count = 0;
 
	/*********************读入图像，检测角点********************************************/
	for (int i = 0; i < image_count; i++)
	{
		//读入一系列图片
		string imageFileName, imageFileName1;
		stringstream StrStm;
		StrStm << "/home/summer/Pictures/Webcam/right";
		//StrStm << "F:/vsprojects2022/ConsoleApplication6/ConsoleApplication6/20241022/right";
		StrStm << i + 1;
		StrStm >> imageFileName;
		StrStm.str("");//清除数据流，以便下次使用
		imageFileName += ".jpg";
 
		cout << imageFileName << endl;
		Mat image = imread(imageFileName);
		image_size = image.size();
		Mat image_gray;
		Mat dstImage1;
		//cvtColor(image, image_gray, CV_RGB2GRAY); // opencv4版本中应改为 COLOR_RGB2GRAY
		cvtColor(image, image_gray, COLOR_RGB2GRAY);
 
		/*imshow("1",image_gray);
		waitKey(0);*/
 
		//检测标定板的角点
		bool patternfound = findChessboardCorners(image, board_size, image_corners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE);
		//+ CALIB_CB_FAST_CHECK);
		if (!patternfound)
		{
			cout << "Can not find chessboard corners!" << endl;
			return -1;
		}
		else
		{
			/* 亚像素精确化 */
			cornerSubPix(image_gray, image_corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
			/* 绘制检测到的角点并保存 */
			/*drawChessboardCorners(image, Size(7, 7), image_corners, patternfound);//红色为先检测的点
			resize(image, dstImage1, Size(image.cols / 5, image.rows / 5), 0, 0, INTER_LINEAR);
			imshow("1", dstImage1);
			waitKey(0);*/
			Mat imageTemp = image.clone();
			for (int j = 0; j < image_corners.size(); j++)
			{
				circle(imageTemp, image_corners[j], 3, Scalar(0, 0, 255), -1, 8, 0);
			}
			string imageFileName;
			std::stringstream StrStm;
			StrStm << i + 1;
			StrStm >> imageFileName;
			imageFileName += "_corner.jpg";
			imwrite(imageFileName, imageTemp);//保存角点检测结果图
			count = count + image_corners.size();
			corners_Seq.push_back(image_corners);
		}
		image_Seq.push_back(image);
	}
 
	/****************************摄像机标定****************************************/
	Size square_size = Size(50, 50);                                      /**** 实际测量得到的标定板上每个棋盘格的大小   ****/
	vector<vector<Point3f>>  object_Points;                                      /****  保存标定板上角点的三维坐标   ****/
 
 
	Mat image_points = Mat(1, count, CV_32FC2, Scalar::all(0));          /*****   保存提取的所有角点   *****/
	vector<int>  point_counts;                                          /*****    每幅图像中角点的数量    ****/
	Mat intrinsic_matrix = Mat(3, 3, CV_32FC1, Scalar::all(0));                /*****    摄像机内参数矩阵    ****/
	//Mat distortion_coeffs = Mat(1, 4, CV_32FC1, Scalar::all(0));            /* 摄像机的4个畸变系数：k1,k2,p1,p2 */
	Mat distortion_coeffs = Mat(1, 5, CV_32FC1, Scalar::all(0));            /* 摄像机的4个畸变系数：k1,k2,p1,p2 */
	vector<cv::Mat> rotation_vectors;                                      /* 每幅图像的旋转向量 */
	vector<cv::Mat> translation_vectors;                                  /* 每幅图像的平移向量 */
 
	/* 初始化标定板上角点的三维坐标 */
	for (int t = 0; t < image_count; t++)
	{
		vector<Point3f> tempPointSet;
		for (int i = 0; i < board_size.height; i++)
		{
			for (int j = 0; j < board_size.width; j++)
			{
				/* 假设标定板放在世界坐标系中z=0的平面上 */
				Point3f tempPoint;
				tempPoint.x = i * square_size.width;
				tempPoint.y = j * square_size.height;
				tempPoint.z = 0;
				tempPointSet.push_back(tempPoint);
			}
		}
		object_Points.push_back(tempPointSet);
	}
 
	/* 初始化每幅图像中的角点数量，这里我们假设每幅图像中都可以看到完整的标定板 */
	for (int i = 0; i < image_count; i++)
	{
		point_counts.push_back(board_size.width * board_size.height);
	}
 
	/*************************开始标定 *******************************/
	//根据角点的世界坐标和像素坐标求参数
	calibrateCamera(object_Points, corners_Seq, image_size, intrinsic_matrix, distortion_coeffs, rotation_vectors, translation_vectors, 0);
	/***********************显示标定结果***************************/
	Mat rotation_matrix = Mat(3, 3, CV_32FC1, Scalar::all(0)); /* 保存每幅图像的旋转矩阵 */
	cout << "相机的内参矩阵" << intrinsic_matrix << endl;
	cout << "相机的畸变系数" << distortion_coeffs << endl;
	ofstream outfile;
	outfile.open("内参标定结果.txt");
	outfile << "相机的内参矩阵为：" << endl << intrinsic_matrix << endl;
	outfile << "相机的畸变系数为：" << endl << distortion_coeffs << endl;
	outfile.close();
	cout << "输出每幅图的旋转向量和平移向量" << endl;
	for (int i = 0; i < image_count; i++)
	{
		cout << "第" << i + 1 << "幅图的旋转向量" << rotation_vectors[i] << endl;
 
		/* 将旋转向量转换为相对应的旋转矩阵 */
		Rodrigues(rotation_vectors[i], rotation_matrix);
		cout << "第" << i + 1 << "幅图像的旋转矩阵：" << endl;
		cout << rotation_matrix << endl;
		cout << "第" << i + 1 << "幅图像的平移向量：" << endl;
		cout << translation_vectors[i] << endl;
	}
 
	/******************对标定结果进行评价**********************************/
	cout << "开始评价标定结果………………" << endl;
	double total_err = 0.0;                   /* 所有图像的平均误差的总和 */
	double err = 0.0;                        /* 每幅图像的平均误差 */
	vector<Point2f>  image_points2;             /****   保存重新计算得到的投影点    ****/
 
	for (int i = 0; i < image_count; i++)
	{
		vector<Point3f> tempPointSet = object_Points[i];
		/****    通过得到的摄像机内外参数，对空间的三维点进行重新投影计算，得到新的投影点     ****/
		projectPoints(tempPointSet, rotation_vectors[i], translation_vectors[i], intrinsic_matrix, distortion_coeffs, image_points2);
		/* 计算新的投影点和旧的投影点之间的误差*/
		vector<Point2f> tempImagePoint = corners_Seq[i];
		Mat tempImagePointMat = Mat(1, tempImagePoint.size(), CV_32FC2);
		Mat image_points2Mat = Mat(1, image_points2.size(), CV_32FC2);
		for (size_t i = 0; i != tempImagePoint.size(); i++)
		{
			image_points2Mat.at<Vec2f>(0, i) = Vec2f(image_points2[i].x, image_points2[i].y);
			tempImagePointMat.at<Vec2f>(0, i) = Vec2f(tempImagePoint[i].x, tempImagePoint[i].y);
		}
		err = norm(image_points2Mat, tempImagePointMat, NORM_L2);
		total_err += err /= point_counts[i];
		cout << "第" << i + 1 << "幅图像的平均误差：" << err << "像素" << endl;
	}
	cout << "总体平均误差：" << total_err / image_count << "像素" << endl;
 
	Mat mapx = Mat(image_size, CV_32FC1);
	Mat mapy = Mat(image_size, CV_32FC1);
	Mat R = Mat::eye(3, 3, CV_32F);
	cout << "保存校正图像" << endl;
	for (int i = 0; i != image_count; i++)
	{
		Mat newCameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));
		//根据标定结果进行图像的修正
		initUndistortRectifyMap(intrinsic_matrix, distortion_coeffs, R, getOptimalNewCameraMatrix(intrinsic_matrix, distortion_coeffs, image_size, 1, image_size, 0), image_size, CV_32FC1, mapx, mapy);
		Mat t = image_Seq[i].clone();
		cv::remap(image_Seq[i], t, mapx, mapy, INTER_LINEAR);
		string imageFileName;
		std::stringstream StrStm;
		StrStm << i + 1;
		StrStm >> imageFileName;
		imageFileName += "_校正后图像.jpg";
		imwrite(imageFileName, t);//保存畸变校正结果图
	}
	cout << "保存结束" << endl;
 
	//waitKey(0);
	system("pause");
	return 0;
}