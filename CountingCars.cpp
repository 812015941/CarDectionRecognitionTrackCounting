#include "OPENCV_TYC_FORM.h"
/************************************************************************/
/*Author:����� 2016.8
/*��Դ���ı���
/*����ҵ���Ƽ������27��7�� �����ڳ�������������Ƶ��������⡷                                                                   */
/************************************************************************/

int getD(int rectny,bool selectObject,Rect selection,Point origin)
{
	int dd;
	if(!selectObject&&selection.width>0&&selection.height>0)
	{
		int local_y=origin.y;
		dd=local_y-rectny;

	}else 
	{
		dd=0;
	}

	return dd;

}
//����ֵ������ҵ�����ǰ�ľͷ���λ��i�����û�У��ͷ���-1
int isSameCar(Point pnew,vector<Point> rectps,int Threshold)
{
	size_t len = rectps.size();

	for (size_t i =0; i < len; i ++) {

		Point tmp = rectps[i];
		if (abs(pnew.x-tmp.x)<Threshold&&pnew.y-tmp.y>0)//��һ���������⣡��Щ���ͻȻ����
		{
			return i;
		}
	}
	return -1;
}

void savePoint(Point &pnew,Rect rect,int d,vector<Point> &rectps,int Threshold,int &countNum,int typeXL)
{
	if(0<d&&d<rect.height)
	{
		if (rectps.size()==0)
		{
			rectps.push_back(pnew);
		}
		else
		{
			int issamecarInt=isSameCar(pnew,rectps,Threshold);

			//
			
			//
			if(issamecarInt!=-1)//��֮ǰͬһ����
			{
				vector<Point>::const_iterator itr=rectps.begin()+issamecarInt;
				
				rectps[issamecarInt]=pnew;
				//���rectps[countNum]
			}
			else if(issamecarInt==-1)
			{
				//�µģ�
				rectps.push_back(pnew);
			}
		}


	}
	countNum=rectps.size();
	if (typeXL==1)
	{
		cout<<"countNum1:"<<countNum<<endl;
	}
	else if (typeXL==2)
	{
		cout<<"countNum2:"<<countNum<<endl;
	}
	else if (typeXL==3)
	{
		cout<<"countNum3:"<<countNum<<endl;
	}
	else cout<<"countNumȫ��:"<<countNum<<endl;
}