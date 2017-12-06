
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <ctime>
#include<SerialThread.h>

using namespace cv;
using namespace std;
void onMouse(int event, int x, int y, int flags, void* param);
void openCom(CSerialPort mySerialPort1);
//static void on_trackbar(int cThresh, Mat blurImg, Mat contours, void*);
Point2f corners[4];//= {0};
int corners_cnt = 0;
int board_w = 8;//8;
int board_h = 5;//8;
int key = 0;

int main( int _argc, char** _argv )
{
	// Images
	Mat inputImg, org_inputImg,inputImgGray;
	Mat outputImg;
	CSerialPort mySerialPort1;
	openCom(mySerialPort1);
	//_argc = 2;
	//_argv[0] = "IPM1.exe";
	//_argv[1] = "1.mp4";
	//if( _argc != 2 )
	//{
	//	cout << "Usage: ipm.exe <videofile>" << endl;
	//	return 1;
	//}

	//// Video
	//string videoFileName = _argv[1];	
	//cv::VideoCapture video;
	//if( !video.open(videoFileName) )
	//	return 1;

	// Show video information


	//// The 4-points at the input image	
	////vector<Point2f> origPoints;
	////origPoints.push_back( Point2f(0, height) );
	////origPoints.push_back( Point2f(width, height) );
	////origPoints.push_back( Point2f(width/2+30, 140) );
	////origPoints.push_back( Point2f(width/2-50, 140) );
	//vector<Point2f> origPoints;
	//origPoints.push_back(Point2f(width / 2 - 100, 180));//像点
	//origPoints.push_back(Point2f(width / 2 + 100, 180));
	//origPoints.push_back(Point2f(50, height));
	//origPoints.push_back(Point2f(width-50, height));
	//

	//// The 4-points correspondences in the destination image
	////vector<Point2f> dstPoints;
	////dstPoints.push_back( Point2f(0, height) );
	////dstPoints.push_back( Point2f(width, height) );
	////dstPoints.push_back( Point2f(width, 0) );
	////dstPoints.push_back( Point2f(0, 0) );
	//vector<Point2f> dstPoints;
	//dstPoints.push_back(Point2f(0, 0));//物点
	//dstPoints.push_back(Point2f(board_w, 0));
	//dstPoints.push_back(Point2f(0, board_h));
	//dstPoints.push_back(Point2f(board_w, board_h));



	// IPM object
//	IPM ipm( Size(width, height), Size(width, height), origPoints, dstPoints );
//==============================videoCap=====================================
	//VideoCapture cap(1);
	//if (!cap.isOpened())
	//{
	//	return -1;
	//}
	//int width = 0, height = 0, fps = 0, fourcc = 0;
	//width = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_WIDTH));
	//height = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	//fps = static_cast<int>(cap.get(CV_CAP_PROP_FPS));
	//fourcc = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC));

	//cout << "Input video: (" << width << "x" << height << ") at " << fps << ", fourcc = " << fourcc << endl;
	//Mat frame;

	//while (key!='s')
	//{
	//	cap >> frame;
	//	imshow("video", frame);
	//	key = waitKey(10);
	//	if (key == 's') imwrite("test.png", frame);
	//}
////=========================================================================
	Mat ipmImg;
	// Main loop
	int frameNum = 0;
	
	inputImg = imread("test.png", 1);
	//if(org_inputImg.channels() == 3)
	// cvtColor(org_inputImg, inputImg, CV_BGR2GRAY);
	Mat temp;
	namedWindow("temp", 1);
	inputImg.copyTo(temp);
	cv::setMouseCallback("temp", onMouse, reinterpret_cast<void*> (&temp));
	while (corners_cnt < 4)
	{
		//	int len = (sizeof(corners) / sizeof(Point2f));
		for(int i = 0; i <corners_cnt; i++)
		{
			circle(temp, corners[i], 3, Scalar(0, 0, 255), -1, 8);
		}
		imshow("temp", temp);
		if (waitKey(0) == 27) break;
	}
	Point2f objPts[4], imgPts[4];
	objPts[0].x = 0;		objPts[0].y = 0;
	objPts[1].x = board_w;	objPts[1].y = 0;
	objPts[2].x = 0;		objPts[2].y = board_h;
	objPts[3].x = board_w;	objPts[3].y = board_h;
	//imgPts[0] = Point2f(width / 2 - 100, 100);//
	//imgPts[1] = Point2f(width / 2 + 100, 100);//
	//imgPts[2] = Point2f(50, height);//  25
	//imgPts[3] = Point2f(width - 50, height);//  95
	imgPts[0] = corners[0];// imgPts[0] = Point2f(38, 24);//
	imgPts[1] = corners[1]; //imgPts[1] = Point2f(82, 24);//
	imgPts[2] = corners[2]; //imgPts[2] = Point2f(28, 49);//  25
	imgPts[3] = corners[3]; 
	Mat H;
	H = getPerspectiveTransform(objPts, imgPts);//物点 像点
	float z = 25;
	int cannyThresh = 175;//50;
	while (key != 27)
	{
	//	printf("FRAME #%6d ", frameNum);
	//	fflush(stdout);
	//	frameNum++;

	//  cap >> inputImg;//=frame;		

		//video >> inputImg;
		//if( inputImg.empty() )
		//	break;
		 //if(inputImg.channels() == 3)		 
			// cvtColor(inputImg, inputImgGray, CV_BGR2GRAY);				 		 
		 //else	 
			// inputImg.copyTo(inputImgGray);			 		 
		 ipmImg = inputImg.clone();
		 // Process
//		 clock_t begin = clock();
		 H.at<double>(2, 2) = z;
		 warpPerspective(inputImg, ipmImg, H, inputImg.size(), WARP_INVERSE_MAP 
			 +INTER_LINEAR //bilinear interpolation
			 + WARP_FILL_OUTLIERS
		 );
		// ipm.applyHomography( inputImg, outputImg );
//		 clock_t end = clock();
//		 double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		 //printf("%.2f (ms)\n", 1000*elapsed_secs);
	//	 imshow("Input", inputImg);
		 imshow("ipm", ipmImg);
		 Rect roi(0, ipmImg.rows / 3, ipmImg.cols - 1, 2*ipmImg.rows / 3);// (x, y, width, height)
		 Mat imgROI = ipmImg(roi);
		 imshow("roi", imgROI);
		 key=waitKey(1);
		 if (key == 'd') {
			 z -= 0.5;
			 cout << "z=" << z << endl;
		 }
		 else if (key == 'f') {
			 z += 0.5;
			 cout << "z=" << z << endl;
		 }
		 else if (key == 'a') {
			 cannyThresh -= 1;
			 cout << "thres=" << cannyThresh << endl;
		 }
		 else if (key == 's') {
			 cannyThresh += 1;
			 cout << "thres=" << cannyThresh << endl;
		 }
		 Mat contours,blurImg;
		 //Blur proc
		 blur(imgROI, blurImg, Size(2, 2));
		 imshow("blur", blurImg);
		 //Canny extract contours
		 Canny(blurImg, contours, cannyThresh, 3 * cannyThresh); //50, 150);
		 Mat contoursInv;
		 threshold(contours, contoursInv, 128,			255,      THRESH_BINARY_INV);
										//thresh,  maxValue to use
		 imshow("Contours", contoursInv);//black 0 white 255
//======================================findCenterLine================

//======================================houghLine=====================
  		 vector<Vec4i> lines;//储存着检测到的直线的参数对 (X1, Y1, X2, Y2) 的容器，也就是线段的两个端点  
		 HoughLinesP(contours,lines, 1, CV_PI / 180, 50, 50, 25);
		 //dst:边缘检测的输出图像. 它应该是个灰度图,作为我们的输入图像  
		 //１：参数极径 r 以像素值为单位的分辨率. 我们使用 1 像素.  
		 //CV_PI/180:参数极角 \theta 以弧度为单位的分辨率. 我们使用 1度 (即CV_PI/180)  
		 //50:要”检测” 一条直线所需最少的的曲线交点  
		 //50:能组成一条直线的最少点的数量. 点数量不足的直线将被抛弃.  
		 //10:线段上最近两点之间的阈值,也就是能被认为在一条直线上的亮点的最大距离.  

		 Mat contoursH;
		 cvtColor(contours,contoursH, CV_GRAY2BGR);
		 for (size_t i = 0; i < lines.size(); i++)
		 {
			 Vec4i l = lines[i];//Vec4i 就是Vec<int, 4>，里面存放４个int  
			 line(contoursH, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
//要划的线所在的图像：cdst， 起点：Point(l[0], l[1])，　终点：Point(l[2], l[3])，　颜色：Scalar(0,0,255)，  
		 }
		 imshow("hough", contoursH);
		// createTrackbar("threshold", "Contours", &cannyThresh, 255, on_trackbar);
		// on_trackbar(cannyThresh, blurImg,contours,0);//轨迹条回调函数
	}

	return 0;	
}		
//static void on_trackbar(int cThresh,Mat blurImg,Mat contours, void*)
//{
//	Canny(blurImg, contours, cThresh, cThresh*3);
//	Mat contoursInv;
//	threshold(contours, contoursInv, 128, 255, THRESH_BINARY_INV);
//	imshow("Contours", contoursInv);
//}
void openCom(CSerialPort mySerialPort1) {
//	String status = "";
	mySerialPort1.CloseListenTread();
	//ComName = ui.ck_edit->text();
	//ComName2 = ui.ck_edit2->text();
	//int a = ComName.toInt();
	//int b = ComName2.toInt();
	if ( !mySerialPort1.InitPort(3)//a 
		|| !mySerialPort1.OpenListenThread() )
	{
		cout << "fail to open com" << endl;
		return;
	}
	else
	{
		mySerialPort1.comName = 3;
		cout<<"succeed"<<endl;
		return;
	}
}
void onMouse(int event, int x, int y, int flags, void* param)
{
	Mat *im = reinterpret_cast<Mat*>(param);
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		if (corners_cnt <= 4)
		{
			corners[corners_cnt].x = x;
			corners[corners_cnt].y = y;
			corners_cnt++;
			std::cout << "at(" << x << "," << y << ")value is:" << static_cast<int>(im->at<uchar>(cv::Point(x, y))) << std::endl;
		}
		else corners_cnt = 0;
	}
}