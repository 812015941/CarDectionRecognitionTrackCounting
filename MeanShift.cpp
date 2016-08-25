#include "OPENCV_TYC_FORM.h"
#include "Merge.h"
/************************************************************************/
/*Author:����� 2016.8
/*��Դ���ı���
/*��ҫ��meanShift��Ŀ�����                                               */
/************************************************************************/
//���Ƕ��ͬʱ���٣�����ʱ�����ֵ�ʶ���Ѿ�ʶ������ˣ�׼������ִ�����������.
vector<Rect> selectionLY1,selectionLY2,selectionLY3;//���ܷ�����ÿ֡��ZhenMain,�����ͬһ��������ô�滻ԭ����Ԫ�أ�
//vector<Rect> trackWindow;//��ͼ�õģ����յĽ��
int i=0;//��ʱ�����еĿ�ĸ�������trackWindowԪ�ظ������Ӧ��
///------------------------------����----------
const int size=10;
int trackObject1 = 0,trackObject2 = 0,trackObject3 = 0;//����Ҫ��hist_change[]�����ҹ���
int vmin = 10, vmax = 256, smin = 30;//��hsvͨ����Χ�޶�
bool paused = false;
int hsize = 16;
float hranges[] = {0,180};//hranges�ں���ļ���ֱ��ͼ������Ҫ�õ�
const float* phranges = hranges;
double ThresholdGENZONG=0.9;
Mat frame1, hsv1, hue1, mask1[size], hist1[size],hist_change1[size], histimg1 = Mat::zeros(200, 320, CV_8UC3), backproj1[size],back1;
Mat frame2, hsv2, hue2, mask2[size], hist2[size],hist_change2[size], histimg2 = Mat::zeros(200, 320, CV_8UC3), backproj2[size],back2;
Mat frame3, hsv3, hue3, mask3[size], hist3[size],hist_change3[size], histimg3 = Mat::zeros(200, 320, CV_8UC3), backproj3[size],back3;

vector<Rect> meanshiftTYC(vector<Rect> ZhenMain,Mat &sequence2,Mat &sequence21,int typeXL)

{
	if (typeXL==1)
	{
		//ÿһ֡����ȥ�������з�����Ϊ�������жϣ��ǲ���ͬһ����

		size_t len = ZhenMain.size();
		for (size_t k =0; k < len; k ++) {

			Rect tmp = ZhenMain[k];
			if (!selectionLY1.empty())
			{
				selectionLY1.push_back(tmp);
				i++;
				trackObject1=-1;
			}
			else
			{
				selectionLY1.push_back(tmp);
				i++;
				trackObject1=-1;
			}


		}

		len=selectionLY1.size();
		for (size_t k =0; k < len; k ++)
		{
			if (selectionLY1[k].area()>0)
			{
				Point br=selectionLY1[k].br();
				if (br.y>=sequence2.rows)
				{
					selectionLY1[k]=selectionLY1[k+1];
				}
			}

		}


		i=selectionLY1.size();
		len=selectionLY1.size();
		if(i>0)
		{





			cvtColor(sequence21, hsv1, CV_BGR2HSV);//��rgb����ͷ֡ת����hsv�ռ��


			int _vmin = vmin, _vmax = vmax;

			//inRange�����Ĺ����Ǽ����������ÿ��Ԫ�ش�С�Ƿ���2��������ֵ֮�䣬�����ж�ͨ��,mask����0ͨ������Сֵ��Ҳ����h����
			//����������hsv��3��ͨ�����Ƚ�h,0~180,s,smin~256,v,min(vmin,vmax),max(vmin,vmax)�����3��ͨ�����ڶ�Ӧ�ķ�Χ�ڣ���
			//mask��Ӧ���Ǹ����ֵȫΪ1(0xff)������Ϊ0(0x00).
			for(int j=0;j<i;j++)
			{
				inRange(hsv1, Scalar(0, smin, MIN(_vmin,_vmax)),Scalar(180, 256, MAX(_vmin, _vmax)), mask1[j]);
			}	
			int ch[] = {0, 0};
			hue1.create(hsv1.size(), hsv1.depth());//hue��ʼ��Ϊ��hsv��С���һ���ľ���ɫ���Ķ������ýǶȱ�ʾ�ģ�������֮�����120�ȣ���ɫ���180��
			mixChannels(&hsv1, 1, &hue1, 1, ch, 1);//��hsv��һ��ͨ��(Ҳ����ɫ��)�������Ƶ�hue�У�0��������
			//������8�±���
			//�˴��Ĺ��캯��roi�õ���Mat hue�ľ���ͷ����roi������ָ��ָ��hue����������ͬ�����ݣ�selectΪ�����Ȥ������
			for(int j=0;j<selectionLY1.size();j++)
			{
				//trackWindow.push_back(selectionLY[j]);
			}
			//����mask
			for(int j=0;j<selectionLY1.size();j++)
			{
				Mat roi(hue1,selectionLY1[j]);
				Mat maskroi(mask1[j],selectionLY1[j]);//mask�����hsv����Сֵ


				//calcHist()������һ������Ϊ����������У���2��������ʾ����ľ�����Ŀ����3��������ʾ��������ֱ��ͼά��ͨ�����б���4��������ʾ��ѡ�����뺯��
				//��5��������ʾ���ֱ��ͼ����6��������ʾֱ��ͼ��ά������7������Ϊÿһάֱ��ͼ����Ĵ�С����8������Ϊÿһάֱ��ͼbin�ı߽�
				calcHist(&roi, 1, 0, maskroi, hist1[j], 1, &hsize, &phranges);//��roi��0ͨ������ֱ��ͼ��ͨ��mask����hist�У�hsizeΪÿһάֱ��ͼ�Ĵ�С
				if(trackObject1<0)
				{
					hist_change1[j]=hist1[j];
				}
				normalize(hist1[j], hist1[j], 0, 255, CV_MINMAX);//��hist����������鷶Χ��һ��������һ����0~255
			}
			if (selectionLY1.size()>=2)
			{
				WindowsMerge windowsMerge;
				selectionLY1=windowsMerge.merge(selectionLY1);

			}
			len=selectionLY1.size();
			for(int j=0;j<len;j++){
				calcBackProject(&hue1, 1, 0, hist1[j], backproj1[j], &phranges);//����ֱ��ͼ�ķ���ͶӰ������hueͼ��0ͨ��ֱ��ͼhist�ķ���ͶӰ��������backproj��
				backproj1[j] &= mask1[j];//�������mask�ĳߴ��ϡ�

				meanShift(backproj1[j], selectionLY1[j],               //trackWindowΪ���ѡ�������TermCriteriaΪȷ��������ֹ��׼��
					TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ));//CV_TERMCRIT_EPS��ͨ��forest_accuracy,CV_TERMCRIT_ITER

				if (selectionLY1[j].area()>0)
				{
					Point br=selectionLY1[j].br();
					if (br.y>=sequence2.rows)
					{
						vector<Rect>::const_iterator itr=selectionLY1.begin()+j;
						selectionLY1.erase(itr);
					}
				}
				len=selectionLY1.size();
			}
		}
		len=selectionLY1.size();
		if (len>0)
		{
			for (int j=0;j<len;j++)
			{
				rectangle(sequence2,selectionLY1[j],Scalar(0),2);
			}
		}
		return selectionLY1;
	}




	else if (typeXL==2)
	{
		//ÿһ֡����ȥ�������з�����Ϊ�������жϣ��ǲ���ͬһ����

		size_t len = ZhenMain.size();
		for (size_t k =0; k < len; k ++) {

			Rect tmp = ZhenMain[k];
			selectionLY2.push_back(tmp);
			trackObject2=-1;
		


		}

		


		i=selectionLY2.size();
		len=selectionLY2.size();
		if(i>0)
		{





			cvtColor(sequence21, hsv2, CV_BGR2HSV);//��rgb����ͷ֡ת����hsv�ռ��


			int _vmin = vmin, _vmax = vmax;

			//inRange�����Ĺ����Ǽ����������ÿ��Ԫ�ش�С�Ƿ���2��������ֵ֮�䣬�����ж�ͨ��,mask����0ͨ������Сֵ��Ҳ����h����
			//����������hsv��3��ͨ�����Ƚ�h,0~180,s,smin~256,v,min(vmin,vmax),max(vmin,vmax)�����3��ͨ�����ڶ�Ӧ�ķ�Χ�ڣ���
			//mask��Ӧ���Ǹ����ֵȫΪ1(0xff)������Ϊ0(0x00).
			for(int j=0;j<i;j++)
			{
				inRange(hsv2, Scalar(0, smin, MIN(_vmin,_vmax)),Scalar(180, 256, MAX(_vmin, _vmax)), mask2[j]);
			}	
			int ch[] = {0, 0};
			hue2.create(hsv2.size(), hsv2.depth());//hue��ʼ��Ϊ��hsv��С���һ���ľ���ɫ���Ķ������ýǶȱ�ʾ�ģ�������֮�����120�ȣ���ɫ���180��
			mixChannels(&hsv2, 1, &hue2, 1, ch, 1);//��hsv��һ��ͨ��(Ҳ����ɫ��)�������Ƶ�hue�У�0��������
			//������8�±���
			//�˴��Ĺ��캯��roi�õ���Mat hue�ľ���ͷ����roi������ָ��ָ��hue����������ͬ�����ݣ�selectΪ�����Ȥ������
			for(int j=0;j<selectionLY2.size();j++)
			{
				//trackWindow.push_back(selectionLY[j]);
			}
			//����mask
			for(int j=0;j<selectionLY2.size();j++)
			{
				Mat roi(hue2,selectionLY2[j]);
				Mat maskroi(mask2[j],selectionLY2[j]);//mask�����hsv����Сֵ


				//calcHist()������һ������Ϊ����������У���2��������ʾ����ľ�����Ŀ����3��������ʾ��������ֱ��ͼά��ͨ�����б���4��������ʾ��ѡ�����뺯��
				//��5��������ʾ���ֱ��ͼ����6��������ʾֱ��ͼ��ά������7������Ϊÿһάֱ��ͼ����Ĵ�С����8������Ϊÿһάֱ��ͼbin�ı߽�
				calcHist(&roi, 1, 0, maskroi, hist2[j], 1, &hsize, &phranges);//��roi��0ͨ������ֱ��ͼ��ͨ��mask����hist�У�hsizeΪÿһάֱ��ͼ�Ĵ�С
				if(trackObject2<0)
				{
					hist_change2[j]=hist2[j];
				}
				normalize(hist2[j], hist2[j], 0, 255, CV_MINMAX);//��hist����������鷶Χ��һ��������һ����0~255
			}
			if (selectionLY2.size()>=2)
			{
				WindowsMerge windowsMerge;
				selectionLY2=windowsMerge.merge(selectionLY2);

			}
			len=selectionLY2.size();
			for(int j=0;j<len;j++){
				calcBackProject(&hue2, 1, 0, hist2[j], backproj2[j], &phranges);//����ֱ��ͼ�ķ���ͶӰ������hueͼ��0ͨ��ֱ��ͼhist�ķ���ͶӰ��������backproj��
				backproj2[j] &= mask2[j];//�������mask�ĳߴ��ϡ�

				meanShift(backproj2[j], selectionLY2[j],               //trackWindowΪ���ѡ�������TermCriteriaΪȷ��������ֹ��׼��
					TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ));//CV_TERMCRIT_EPS��ͨ��forest_accuracy,CV_TERMCRIT_ITER

				//if( trackWindow[j].area() <= 1 ){
				//	int cols = backproj[j].cols, rows = backproj[j].rows, r = (MIN(cols, rows) + 5)/6;
				//	trackWindow[j] = Rect(trackWindow[j].x - r, trackWindow[j].y - r,
				//		trackWindow[j].x + r, trackWindow[j].y + r) &
				//		Rect(0, 0, cols, rows);//Rect����Ϊ�����ƫ�ƺʹ�С������һ��������Ϊ��������Ͻǵ����꣬�����ĸ�����Ϊ����Ŀ�͸�
				//}
				//����Ѿ���������߿�ĳ�����Rect!
				if (selectionLY2[j].area()>0)
				{
					Point br=selectionLY2[j].br();
					if (br.y>=sequence2.rows)
					{
						vector<Rect>::const_iterator itr=selectionLY2.begin()+j;
						selectionLY2.erase(itr);
					}
				}
				len=selectionLY2.size();
			}
		}
		len=selectionLY2.size();
		if (len>0)
		{
			for (int j=0;j<len;j++)
			{
				rectangle(sequence2,selectionLY2[j],Scalar(0),2);
			}
		}
		return selectionLY2;

	}

	else if(typeXL==3)
	{
		//ÿһ֡����ȥ�������з�����Ϊ�������жϣ��ǲ���ͬһ����
		size_t len = ZhenMain.size();
		for (size_t k =0; k < len; k ++) {

			Rect tmp = ZhenMain[k];
			if (!selectionLY3.empty())
			{
				selectionLY3.push_back(tmp);
				i++;
				trackObject3=-1;
			}
			else//��ʼ��
			{
				selectionLY3.push_back(tmp);
				i++;
				trackObject3=-1;
			}


		}

		len=selectionLY3.size();
		for (size_t k =0; k < len; k ++)
		{
			if (selectionLY3[k].area()>0)
			{
				Point br=selectionLY3[k].br();
				if (br.y>=sequence2.rows)
				{
					selectionLY3[k]=selectionLY3[k+1];
				}
			}

		}


		i=selectionLY3.size();
		len=selectionLY3.size();
		if(i>0)
		{





			cvtColor(sequence21, hsv3, CV_BGR2HSV);//��rgb����ͷ֡ת����hsv�ռ��


			int _vmin = vmin, _vmax = vmax;

			//inRange�����Ĺ����Ǽ����������ÿ��Ԫ�ش�С�Ƿ���2��������ֵ֮�䣬�����ж�ͨ��,mask����0ͨ������Сֵ��Ҳ����h����
			//����������hsv��3��ͨ�����Ƚ�h,0~180,s,smin~256,v,min(vmin,vmax),max(vmin,vmax)�����3��ͨ�����ڶ�Ӧ�ķ�Χ�ڣ���
			//mask��Ӧ���Ǹ����ֵȫΪ1(0xff)������Ϊ0(0x00).
			for(int j=0;j<i;j++)
			{
				inRange(hsv3, Scalar(0, smin, MIN(_vmin,_vmax)),Scalar(180, 256, MAX(_vmin, _vmax)), mask3[j]);
			}	
			int ch[] = {0, 0};
			hue3.create(hsv3.size(), hsv3.depth());//hue��ʼ��Ϊ��hsv��С���һ���ľ���ɫ���Ķ������ýǶȱ�ʾ�ģ�������֮�����120�ȣ���ɫ���180��
			mixChannels(&hsv3, 1, &hue3, 1, ch, 1);//��hsv��һ��ͨ��(Ҳ����ɫ��)�������Ƶ�hue�У�0��������
			//������8�±���
			//�˴��Ĺ��캯��roi�õ���Mat hue�ľ���ͷ����roi������ָ��ָ��hue����������ͬ�����ݣ�selectΪ�����Ȥ������
			for(int j=0;j<selectionLY3.size();j++)
			{
				//trackWindow.push_back(selectionLY[j]);
			}
			//����mask
			for(int j=0;j<selectionLY3.size();j++)
			{
				Mat roi(hue3,selectionLY3[j]);
				Mat maskroi(mask3[j],selectionLY3[j]);//mask�����hsv����Сֵ


				//calcHist()������һ������Ϊ����������У���2��������ʾ����ľ�����Ŀ����3��������ʾ��������ֱ��ͼά��ͨ�����б���4��������ʾ��ѡ�����뺯��
				//��5��������ʾ���ֱ��ͼ����6��������ʾֱ��ͼ��ά������7������Ϊÿһάֱ��ͼ����Ĵ�С����8������Ϊÿһάֱ��ͼbin�ı߽�
				calcHist(&roi, 1, 0, maskroi, hist3[j], 1, &hsize, &phranges);//��roi��0ͨ������ֱ��ͼ��ͨ��mask����hist�У�hsizeΪÿһάֱ��ͼ�Ĵ�С
				if(trackObject3<0)
				{
					hist_change3[j]=hist3[j];
				}
				normalize(hist3[j], hist3[j], 0, 255, CV_MINMAX);//��hist����������鷶Χ��һ��������һ����0~255
			}
			if (selectionLY3.size()>=2)
			{
				WindowsMerge windowsMerge;
				selectionLY3=windowsMerge.merge(selectionLY3);

			}
			len=selectionLY3.size();
			for(int j=0;j<len;j++){
				calcBackProject(&hue3, 1, 0, hist3[j], backproj3[j], &phranges);//����ֱ��ͼ�ķ���ͶӰ������hueͼ��0ͨ��ֱ��ͼhist�ķ���ͶӰ��������backproj��
				backproj3[j] &= mask3[j];//�������mask�ĳߴ��ϡ�

				meanShift(backproj3[j], selectionLY3[j],               //trackWindowΪ���ѡ�������TermCriteriaΪȷ��������ֹ��׼��
					TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ));//CV_TERMCRIT_EPS��ͨ��forest_accuracy,CV_TERMCRIT_ITER

				//if( trackWindow[j].area() <= 1 ){
				//	int cols = backproj[j].cols, rows = backproj[j].rows, r = (MIN(cols, rows) + 5)/6;
				//	trackWindow[j] = Rect(trackWindow[j].x - r, trackWindow[j].y - r,
				//		trackWindow[j].x + r, trackWindow[j].y + r) &
				//		Rect(0, 0, cols, rows);//Rect����Ϊ�����ƫ�ƺʹ�С������һ��������Ϊ��������Ͻǵ����꣬�����ĸ�����Ϊ����Ŀ�͸�
				//}
				//����Ѿ���������߿�ĳ�����Rect!
				if (selectionLY3[j].area()>0)
				{
					Point br=selectionLY3[j].br();
					if (br.y>=sequence2.rows)
					{
						vector<Rect>::const_iterator itr=selectionLY3.begin()+j;
						selectionLY3.erase(itr);
					}
				}
				len=selectionLY3.size();
			}
		}
		len=selectionLY3.size();
		if (len>0)
		{
			for (int j=0;j<len;j++)
			{
				rectangle(sequence2,selectionLY3[j],Scalar(0),2);
			}
		}
		return selectionLY3;
	}
	

}





