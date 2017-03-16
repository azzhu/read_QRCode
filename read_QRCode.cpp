// read_QRCode.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

using namespace std;
using namespace cv;

void doitall(Mat* src, float th, vector<Point>& p3);

void ����ͷ����ά��()
{
	Mat src;
	Mat tem(150, 150, CV_8UC3);
	tem.setTo(Vec3b(0, 0, 0));
	vector<Point> p3;
	VideoCapture cap(0);
	while (1)
	{
		cap >> src;
		Mat warp_src;
		Mat ress;
		src.copyTo(warp_src);
		doitall(&src, 0.3, p3);
		if (p3.size() != 0)
		{
			cout << "(" << p3[0].x << " " << p3[0].y << ") ";
			cout << "(" << p3[1].x << " " << p3[1].y << ") ";
			cout << "(" << p3[2].x << " " << p3[2].y << ") " << endl;;
			circle(src, p3[0], 25, Scalar(0, 0, 255), 2);
			circle(src, p3[1], 25, Scalar(0, 255, 0), 2);
			circle(src, p3[2], 25, Scalar(255, 0, 0), 2);

			////����任
			int param = 50;	//��������
			Point2f srcTri[3];
			Point2f dstTri[3];
			Mat warp_mat(2, 3, CV_32FC1);
			Mat warp_dst;
			warp_dst = Mat::zeros(warp_src.rows, warp_src.cols, warp_src.type());
			// ��3����ȷ������任  
			srcTri[0] = Point2f(p3[0].x*1., p3[0].y*1.);	//zhong
			srcTri[1] = Point2f(p3[1].x*1., p3[1].y*1.);	//you
			srcTri[2] = Point2f(p3[2].x*1., p3[2].y*1.);	//zuo
			dstTri[0] = Point2f(120., 40.);
			dstTri[1] = Point2f(120., 440.);
			dstTri[2] = Point2f(520., 40.);
			warp_mat = getAffineTransform(srcTri, dstTri);
			warpAffine(warp_src, warp_dst, warp_mat, warp_dst.size());
			//imshow("����任", warp_dst/*(Rect(120,40,400,400))*/);
			ress = warp_dst(Rect(120 + param, 40 + param,400 - 2 * param, 400 - 2 * param));
			//imshow("res", ress);
			//imshow("  ", az::imgto(&ress, 100, 100));
			tem = az::imgto(&ress, 150, 150);
		}
		for (int i = 0; i < 150; i++)
		{
			//uchar* data0 = tem.ptr<uchar>(i);
			//uchar* data1 = src.ptr<uchar>(i);
			for (int j = 0; j < 150; j++)
			{
				src.at<Vec3b>(i, j)[0] = tem.at<Vec3b>(i, j)[0];
				src.at<Vec3b>(i, j)[1] = tem.at<Vec3b>(i, j)[1];
				src.at<Vec3b>(i, j)[2] = tem.at<Vec3b>(i, j)[2];
			}
		}
		imshow("dst", src);
		
		waitKey(35);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	����ͷ����ά��();
	return 0;

	//���ĵ㼯��
	vector<Point> p3;
		
	Mat src = imread("D:\\csimg\\erweimatest.bmp",0);
	imshow("src", src);
	doitall(&src, 0.3, p3);
	cout << p3.size() << endl;
	threshold(src, src, 127, 255, cv::ThresholdTypes::THRESH_OTSU);
	Mat src_3c;
	cvtColor(src, src_3c, CV_GRAY2BGR);
	vector<Point>::iterator itb = p3.begin(),
		ite = p3.end();
	imwrite("d:/csimg/temp0.bmp", src_3c);
	for (; itb != ite; itb++)
	{
		circle(src_3c, *itb, 3, Scalar(0, 0, 255), 3);
		//src_3c.at<cv::Vec3b>(*itb) = Vec3b(0, 0, 255);
	}
	imwrite("d:/csimg/temp1.bmp", src_3c);
	imshow("dst", src_3c);
	waitKey();

	return 0;
}

//����ͼ���ȡAB��,th:��Χ��ֵ
void get_abps(Mat* src, vector<pair<Point, Point>>& abps, float th)
{
	int nc = src->cols;
	int nr = src->rows;
	int ch = src->channels();
	Mat img;
	src->copyTo(img);
	if (ch != 1)
		cvtColor(*src, img, CV_BGR2GRAY);
	//otsu��ֵ��
	threshold(img, img, 127, 255, cv::ThresholdTypes::THRESH_OTSU);
	//medianBlur(img, img, 3);
	//imshow("otsu��ֵ����", img);

	int x, y;	//������ǵ�ǰ����
	int val;	//������ǵ�ǰ����ֵ
	queue<Point> p;		//6����,����
	queue<Point> pcp;	//һ�ݿ���
	Point p_[6];	//6����
	int l[5];		//5���߶�
	float r[4];		//4����ֵ
	for (int i = 0; i < nr; i++)
	{
		uchar* data = img.ptr<uchar>(i);
		x = 0;	y = i;
		queue<Point> tempp;
		p = tempp;
		p.push(Point(x, y));
		val = *data;
		for (int j = 0; j < nc; j++)
		{
			//��һ������ֵ�仯��
			if (*data != val)
			{
				val = *data;
				p.push(Point(x, y));
				if (p.size() > 6)
					p.pop();
				//�����е�ĸ�������6���ж��Ƿ����λ��̽��ͼ�εı���
				if (p.size() == 6)
				{
					pcp = p;	//����һ�ݣ���������
					//6����
					p_[0] = pcp.front(); pcp.pop();
					p_[1] = pcp.front(); pcp.pop();
					p_[2] = pcp.front(); pcp.pop();
					p_[3] = pcp.front(); pcp.pop();
					p_[4] = pcp.front(); pcp.pop();
					p_[5] = pcp.front(); pcp.pop();
					//5���߶�
					l[0] = p_[1].x - p_[0].x;
					l[1] = p_[2].x - p_[1].x;
					l[2] = p_[3].x - p_[2].x;
					l[3] = p_[4].x - p_[3].x;
					l[4] = p_[5].x - p_[4].x;
					//4����ֵ
					r[0] = (float)(l[0]) / (float)(l[1]);
					r[1] = (float)(l[1]) / (float)(l[2]);
					r[2] = (float)(l[2]) / (float)(l[3]);
					r[3] = (float)(l[3]) / (float)(l[4]);
					//�ж��Ƿ���ϱ���
					if (abs(r[0] - 1.0) <= th&&
						abs(r[1] - 0.333) <= th / 3.&&
						abs(r[2] - 3.0) <= th*3.&&
						abs(r[3] - 1.0) <= th)
					{
						abps.push_back(pair<Point, Point>(p_[0], p_[5]));
					}
				}
			}
			data++;
			x++;
		}
	}
}

//���ݵ�Լ������ĵ㼯
void abps2cps(vector<pair<Point, Point>> abps, vector<Point>& cps)
{
	cps.clear();
	vector<pair<Point, Point>>::iterator itb = abps.begin(),
		ite = abps.end();
	for (; itb != ite; itb++)
	{
		int x = (itb->first.x + itb->second.x) / 2;
		int y = (itb->first.y + itb->second.y) / 2;
		cps.push_back(Point(x, y));
	}
}

//�������ĵ㼯��3����
void cps2p3(Mat* img, vector<Point> cps, vector<Point>& p3)
{
	//param
	int kernel = 5;

	int h = img->rows;
	int w = img->cols;
	Mat temp(h, w, CV_8UC1);
	temp.setTo(0);

	vector<Point>::iterator itb, ite;
	itb = cps.begin();
	ite = cps.end();
	for (; itb != ite; itb++)
	{
		circle(temp, *itb, kernel, Scalar(255, 255, 255), kernel);
	}
	//imshow("1234", temp);

	std::vector<std::vector<cv::Point>> pss;
	az::findLTQY(&temp, pss);
	//�˴�Ӧ���ټ�һ�¹��˴���

	az::regions2points(pss, p3);
}

//�����������3���㣬ȷ��˳��ȷ��Ϊ���ĵ㡢�ҵ㡢��㣨������������˳�����������ս����������
void zhenglishunxu(Mat* src, vector<Point>& p3)
{
	if (p3.size() != 3)
		return;
	int w = src->cols;
	int h = src->rows;

	Point p_c, p_l, p_r;	//���ġ����ң�
	float l1, l2, l3;		//���������߳���
	float g1, g2, g3;		//������
	Point o1, o2, o3;		//������
	o1 = p3[0], o2 = p3[1]; o3 = p3[2];

	l1 = sqrt(pow(o2.x - o3.x, 2) + pow(o2.y - o3.y, 2));
	l2 = sqrt(pow(o1.x - o3.x, 2) + pow(o1.y - o3.y, 2));
	l3 = sqrt(pow(o2.x - o1.x, 2) + pow(o2.y - o1.y, 2));
	g1 = acos((pow(l2, 2) + pow(l3, 2) - pow(l1, 2)) / (2 * l2*l3));
	g2 = acos((pow(l1, 2) + pow(l3, 2) - pow(l2, 2)) / (2 * l1*l3));
	g3 = acos((pow(l2, 2) + pow(l1, 2) - pow(l3, 2)) / (2 * l2*l1));
	float maxval = max(g1, g2);
	maxval = max(g3, maxval);
	if (g1 == maxval)
	{
		p_c = o1;
		//�����㷨���̲μ��ʼǱ���¼�Ƶ�����
		int x0 = o1.x, y0 = h - o1.y;
		int x1 = o2.x, y1 = h - o2.y;
		int x2 = o3.x, y2 = h - o3.y;
		int x3 = (x1 + x2) / 2, y3 = (y1 + y2) / 2;
		int s = /*(y0 - y3)**/(x0*y3 - x3*y0 - (x0 - x3)*y1 + (y0 - y3)*x1);
		if (s > 0)
		{
			p_l = o3;
			p_r = o2;
		}
		else
		{
			p_l = o2;
			p_r = o3;
		}
	}
	else if (g2 == maxval)
	{
		p_c = o2;
		int x0 = o2.x, y0 = h - o2.y;
		int x1 = o1.x, y1 = h - o1.y;
		int x2 = o3.x, y2 = h - o3.y;
		int x3 = (x1 + x2) / 2, y3 = (y1 + y2) / 2;
		int s = /*(y0 - y3)**/(x0*y3 - x3*y0 - (x0 - x3)*y1 + (y0 - y3)*x1);
		if (s > 0)
		{
			p_l = o3;
			p_r = o1;
		}
		else
		{
			p_l = o1;
			p_r = o3;
		}
	}
	else
	{
		p_c = o3;
		int x0 = o3.x, y0 = h - o3.y;
		int x1 = o2.x, y1 = h - o2.y;
		int x2 = o1.x, y2 = h - o1.y;
		int x3 = (x1 + x2) / 2, y3 = (y1 + y2) / 2;
		int s = /*(y0 - y3)**/(x0*y3 - x3*y0 - (x0 - x3)*y1 + (y0 - y3)*x1);
		if (s > 0)
		{
			p_l = o1;
			p_r = o2;
		}
		else
		{
			p_l = o2;
			p_r = o1;
		}
	}
	p3.clear();
	p3.push_back(p_c);
	p3.push_back(p_l);
	p3.push_back(p_r);
}

//th��������
void doitall(Mat* src, float th, vector<Point>& p3)
{
	vector<pair<Point, Point>> abps;
	vector<Point> cps;

	get_abps(src, abps, th);
	abps2cps(abps, cps);
	cps2p3(src, cps, p3);
	if (p3.size() != 3)
		p3.clear();
	zhenglishunxu(src, p3);
}