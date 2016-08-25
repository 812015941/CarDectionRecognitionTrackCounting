#include <iostream>
#include <io.h>
#include <vector>
#include <string>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>
#include "BackMod.h"

using namespace std;
using namespace cv;
//�̳���CvSVM���࣬��Ϊ����setSVMDetector()���õ��ļ���Ӳ���ʱ����Ҫ�õ�ѵ���õ�SVM��decision_func������
//��ͨ���鿴CvSVMԴ���֪decision_func������protected���ͱ������޷�ֱ�ӷ��ʵ���ֻ�ܼ̳�֮��ͨ����������
class MySVM : public CvSVM
{
public:
	//���SVM�ľ��ߺ����е�alpha����
	double * get_alpha_vector()
	{
		return this->decision_func->alpha;
	}

	//���SVM�ľ��ߺ����е�rho����,��ƫ����
	float get_rho()
	{
		return this->decision_func->rho;
	}
};
MySVM svm1=MySVM();
MySVM svm2=MySVM();
MySVM svm3=MySVM();
int main()
{	
	cout<<"����xml�ļ�"<<endl;
	char *xmlPath1 = "E:/TestVideo/svm1.xml";
	char *xmlPath2 = "E:/TestVideo/svm2.xml";
	char *xmlPath3 = "E:/TestVideo/svm3.xml";
	///////�Ȳ�ִ��
	svm1.load(xmlPath1);
	svm2.load(xmlPath2);
	svm3.load(xmlPath3);
	cout<<"���سɹ�";

	char *filename = "E://TestVideo//1.nsf";
	BackModel m;
	m.setVideoPath(filename);
	m.BackMode();//��˹������ģ
}