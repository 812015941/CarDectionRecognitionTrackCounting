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
using namespace std;
using namespace cv;
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
extern MySVM svm1, svm2, svm3;

int CarDetection(Mat roi)
{
	//��������ж�������Ӧ�ĳ����������������
	/******************���뵥��64*128�Ĳ���ͼ������HOG�����ӽ��з���*********************/
	////��ȡ����ͼƬ(64*128��С)����������HOG������
	vector<float> descriptor;
	HOGDescriptor hog;
	resize(roi, roi, Size(64, 128));
	hog.compute(roi,descriptor,Size(8,8));//����HOG�����ӣ���ⴰ���ƶ�����(8,8)
	Mat testFeatureMat = Mat::zeros(1,3780,CV_32FC1);//����������������������
	//������õ�HOG�����Ӹ��Ƶ�testFeatureMat������
	for(int i=0; i<descriptor.size(); i++)
		testFeatureMat.at<float>(0,i) = descriptor[i];

	//��ѵ���õ�SVM�������Բ���ͼƬ�������������з���
	//int result1 = svm1.predict(testFeatureMat);//�������
	//int result2 = svm2.predict(testFeatureMat);
	//int result3 = svm3.predict(testFeatureMat);
	if(svm1.predict(testFeatureMat) == 1)
		return 1;
	else if(svm2.predict(testFeatureMat) == 1)
		return 2;
	else if(svm3.predict(testFeatureMat) == 1)
		return 3;
	else
		return 0;
}