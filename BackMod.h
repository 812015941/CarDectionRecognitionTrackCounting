#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "opencv2/video/tracking.hpp"
#include <opencv2/ml/ml.hpp>
#include<opencv2/video/video.hpp>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>
#include <io.h>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>

using namespace std;
using namespace cv;
//---------------------��Ҫ���õĺ���
vector<Rect>  CarSift(vector<Rect> boundrect, Mat sequence,int requestType);
int CarDetection(Mat roi);
Mat sobelExectue(Mat &sequence1);
int getD(int rectny,bool selectObject,Rect selection,Point origin);//�������Ļ�þ���
void savePoint(Point &pnew,Rect rect,int d,vector<Point> &rectps,int Threshold,int 
	&countNum,int typeXL);
//ZhenMainΪ�����Rect���飬sequence2Ϊԭgrayͼ��sequence21ΪԭRGBͼ��typeΪ��������
vector<Rect> meanshiftTYC(vector<Rect> ZhenMain,Mat &sequence2,Mat &sequence21,int typeXL);
void compressiveOne(Rect selectionFromXL,Mat &sequence2);//compressivetracker����
//------------------------��Ҫʹ�õĹ��еĳ���

int type1=0,type2=0,type3=0;//���������ֳ���
bool selectObject = false;//�����Ƿ���ѡҪ���ٵĳ�ʼĿ�꣬true��ʾ���������ѡ��
int trackObjectB = 0; //�������Ŀ����Ŀ,trackObject������MeanShift�У�����������trackObjectB
//sequence1,sequence2,sequence3�ֱ����һ֡,sequence21��3ΪRGBͼ,sequence1��2Ϊ�Ҷ�ͼ
Mat sequence1, sequence2,sequence21,sequence3,dst;


Point origin;//���ڱ������ѡ���һ�ε���ʱ���λ��
Point result;//���ڱ������ѡ���Ϳ�ʱ���λ��
Rect selection;//���ڱ������ѡ��ľ��ο�
VideoCapture cap;
//------------------------compressive track----

/************************************************************************/
/*����������ش��롶��ҵ���Ƽ������27��7�� ���ڳ�������������Ƶ��������⡷                                                                   */
/************************************************************************/
int Threshold=50;//������࣡�����ֶ�����!
vector<Point> rectps1,rectps2,rectps3;
//map<int,Point> rectps2;
int countNum1=0,countNum2=0,countNum3=0;//////������
void on_mouse(int event, int x, int y, int flags, void* ustc)//event���������¼���x��y�����¼�����ʱ����λ��
{
	static Point pre_pt = (-1,-1);//��ʼ����  
	static Point cur_pt = (-1,-1);//ʵʱ����  
	if(selectObject)
	{
		selection.x = MIN(x, origin.x);//�������ϽǶ�������
		selection.y = MIN(y, origin.y);
		selection.width = std::abs(x - origin.x);//���ο�
		selection.height = std::abs(y - origin.y);//���θ�
		selection &= Rect(0, 0, sequence2.cols, sequence2.rows);//����ȷ����ѡ�ľ���������ͼƬ��Χ��
	}
	switch( event )
	{
	case CV_EVENT_LBUTTONDOWN:
		origin = Point(x,y);
		selection = Rect(x,y,0,0);//���հ���ȥʱ��ʼ����һ����������
		selectObject = true;
		break;
	case CV_EVENT_LBUTTONUP:
		selectObject = false;
		result=Point(x,y);
		if( selection.width > 0 && selection.height > 0 )
			trackObjectB = -1;
		break;
	}
	//if( selectObject && selection.width > 0 && selection.height > 0 )
	//{
	//	Mat roi(sequence2, selection);
	//	bitwise_not(roi, roi);//bitwise_notΪ��ÿһ��bitλȡ��
	//}
	
}  



const char* keys = 
{
	"{1|  | 0 | camera number}"
};

class BackModel
{
private:
	int serial;  //���������ڱ�ʾ��ǰ������Ƶ�����еĵڼ�֡
	char *video;  //�������Ƶλ��
	//Mat sequence3;  //��Ƶ����
	Mat foreGround;  //ǰ��ͼ
public:
	BackModel() { serial = 0; }
	void setVideoPath(char *path) { video = path; }
	void BackMode();  //��˹������ģ

	
};


void BackModel::BackMode()
{
	Mat dstimg1, dstimg2, dstimg3;
	BackgroundSubtractorMOG mog;
	cap.open(video);
	while (waitKey(1) != 27)
	{
		cap >> sequence1;
		if (!sequence1.data)
			break;
		int H=500;		
		int W = H*sequence1.cols/sequence1.rows;		
		resize(sequence1, sequence1, Size(W, H));//����ȷ�����ڴ�С
		cvtColor(sequence1, sequence1, CV_BGR2GRAY);
		cap >> sequence2;
		sequence2.copyTo(sequence21);
		resize(sequence21,sequence21,Size(W,H));
		if (!sequence2.data)
			break;
		resize(sequence2, sequence2, Size(W, H));
		cap>>sequence3;
		resize(sequence3, sequence3, Size(W, H));
		cvtColor(sequence2, sequence2, CV_BGR2GRAY);
		subtract(sequence1, sequence2, dstimg1);
		threshold(dstimg1, dstimg1, 20, 255, THRESH_BINARY);
		//��˹
		mog(sequence2, dstimg2, 0.01);
		threshold(dstimg2, dstimg2, 50, 255, THRESH_BINARY);
		foreGround = Mat::zeros(dstimg2.size(), CV_8UC1);
		//�ϲ����룩
		H=dstimg1.rows;
		W=dstimg1.cols;
		for (int i = 0;i < H;i++)
		{
			uchar *data = foreGround.ptr<uchar>(i);
			uchar *data1 = dstimg1.ptr<uchar>(i);
			uchar *data2 = dstimg2.ptr<uchar>(i);
			for (int j = 0;j < W;j++)
			{
				if (data1[j] ==255&&data2[j] == 255)//����֡��õ���ͼ�����ش���5�ұ�����ģ�õ�������Ϊ255ʱΪǰ��ͼ
					data[j] = 255;
				else
					data[j] = 0;
			}
		}
		//����5��
		Mat element = getStructuringElement(MORPH_RECT, Size(9, 9));
		dilate(foreGround, foreGround, element, Point(-1, -1), 5);
		erode(foreGround, foreGround, element, Point(-1, -1), 2);
		dilate(foreGround, foreGround, element, Point(-1, -1), 3);
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(foreGround, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
		vector<vector<Point>> contours_poly(contours.size());


		vector<Rect> boundrect(contours.size());
		int len=contours.size();
		for (size_t i = 0; i < len; i++)
		{
			approxPolyDP(Mat(contours[i]), contours_poly[i], 10, 1);  //�����ڶ�������Խ�����ľ��ο�Խ�١�����Ǻܴ󡣡�
			boundrect[i] = boundingRect(Mat(contours_poly[i]));
		}	
		///////////////���ֵļ�⣨�Ѿ�����Բ�㣩
		vector<Rect> boundrect1 = CarSift(boundrect, sequence2,1);//ɸѡ�г��ľ���,����Ϊ1
		vector<Rect> boundrect2 = CarSift(boundrect, sequence2,2);
		vector<Rect> boundrect3 = CarSift(boundrect, sequence2,3);
		vector<Rect> resultRectTYC1=meanshiftTYC(boundrect1,sequence2,sequence21,1);//boundrect1Ϊʶ���ÿһ֡�Ŀ򣨻���˸�� sequence2Ϊgreyͼ sequence3Ϊԭͼ
		vector<Rect> resultRectTYC2=meanshiftTYC(boundrect2,sequence2,sequence21,2);
		vector<Rect> resultRectTYC3=meanshiftTYC(boundrect3,sequence2,sequence21,3);
		//�ڻ��ߺ�Ž��м���
		if(!selectObject&&selection.width>0&&selection.height>0)
		{
		
			for (size_t i = 0; i < resultRectTYC1.size(); i++)
			{
				int rectny=resultRectTYC1[i].tl().y;
				int d=getD(rectny,selectObject,selection,origin);
				savePoint(resultRectTYC1[i].tl(),resultRectTYC1[i],d,rectps1,Threshold,countNum1,1);
			}

			for (size_t i = 0; i < resultRectTYC2.size(); i++)
			{
				int rectny=resultRectTYC2[i].tl().y;
				int d=getD(rectny,selectObject,selection,origin);
				savePoint(resultRectTYC2[i].tl(),resultRectTYC2[i],d,rectps2,Threshold,countNum2,2);
			}

			for (size_t i = 0; i < resultRectTYC3.size(); i++)
			{
				int rectny=resultRectTYC3[i].tl().y;
				int d=getD(rectny,selectObject,selection,origin);
				savePoint(resultRectTYC3[i].tl(),resultRectTYC3[i],d,rectps3,Threshold,countNum3,3);
			}
		}
		
		namedWindow("sequence2");
		setMouseCallback("sequence2",on_mouse,0);

		if(!selectObject&&selection.width>0&&selection.height>0)
		{
			int width = abs(origin.x - result.x);  
			int height = abs(origin.y - result.y);  
			if (width == 0 || height == 0)  
			{  

				printf("width == 0 || height == 0");  
				return;  
			}  
			dst = sequence2(Rect(min(result.x,origin.x),min(result.y,origin.y),width,height));  
			line(sequence2,origin,Point(result.x,origin.y),Scalar(30,70,150),2,8,0);
		}
		imshow("sequence2",sequence2);
	}
}



//�ж����޳�����ʲô��.
/************************************************************************/
/* sequenceΪԭͼ��boundrectΪ���ݳ�������������ľ��ο�                                                                     */
/************************************************************************/
vector<Rect>  CarSift(vector<Rect> boundrect, Mat sequence,int requestType)
{
	vector<Rect> boundrect1,boundrect2,boundrect3;
	/************************************************************************/
	/* �տ�ʼ��������                                                                     */
	/************************************************************************/
	for(int i = 0; i < boundrect.size(); i++)
	{
		if(boundrect[i].area() < 15000 )//|| boundrect[i].area() > 40000)//��ɸѡ��ȥ�������С���˶�����
		{
			boundrect.erase(boundrect.begin() + i);
			i--;
		}
	}
		for(int i = 0; i < boundrect.size(); i++)				//���ֲ�ͬ����
		{
			Mat roi = sequence(Rect(boundrect[i].x, boundrect[i].y, boundrect[i].width, boundrect[i].height));
			int result = CarDetection(roi); 
			if(result == 1)
				boundrect1.push_back(boundrect[i]);
			else if(result == 2)
				boundrect2.push_back(boundrect[i]);
			else if(result == 3)
				boundrect3.push_back(boundrect[i]);
			else
			{
				boundrect.erase(boundrect.begin() + i);
				i--;
			}
		}
		/************************************************************************/
		/* ���໭��                                                                     */
		/************************************************************************/
		if (requestType==1)
		{
			return boundrect1;

		}
		else if(requestType==2)
		{
			return boundrect2;

		}
		else if(requestType==3)
		{
			return boundrect3;
		}
}

