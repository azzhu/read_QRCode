//#include "stdafx.h"
#include "zqjtools.hpp"

//��ʾ���̵�ǰʹ���ڴ����ʷ��ֵʹ���ڴ棻
void az::showMemoryInfo(void)
{
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	
	cout << "********************************\n\n"
		<< "�ڴ�ʹ��:\n\n"
		<< "��ǰ�ڴ棺"<<pmc.WorkingSetSize / 1000000 << "MB\n\n" 
		<< "��ֵ�ڴ棺"<< pmc.PeakWorkingSetSize / 1000000 << "MB\n\n" 
		<< "********************************"<< endl;
}

//�������������
void az::random_sort(int* x, int size)	//�������������
{
	for (int i = size; i >= 2; i--)
	{
		int index = rand() % i;
		int temp = x[index];
		x[index] = x[i - 1];
		x[i - 1] = temp;
	}
}

//��ȡ��ǰʱ��,���غ���
long az::getTime()
{
	return clock();
}

#ifdef USE_OPENCV
//ͼ���Сת��
cv::Mat az::imgto64(cv::Mat *img)
{
	cv::Mat temp(64, 64, CV_32S);
	cv::Size s20(64, 64);
	cv::resize(*img, temp, s20);
	return temp;
}
cv::Mat az::imgto(cv::Mat *img, int w, int h)
{
	cv::Mat temp(h, w, CV_32S);
	cv::Size s20(w, h);
	cv::resize(*img, temp, s20);
	return temp;
}
#endif

az::ZLJG::ZLJG()
{
	size_now = 0;
}
void az::ZLJG::push(std::string ss)
{
	if (size_now < 5)
	{
		s[size_now] = ss;
		size_now++;
	}
	else
	{
		s[0] = s[1];
		s[1] = s[2];
		s[2] = s[3];
		s[3] = s[4];
		s[4] = ss;
	}
}
std::string az::ZLJG::get()
{
	if (size_now != 5)
	{
		return "������...";
	}
	std::vector<std::string> strtemp;
	for (int i = 0; i < 5; i++)
	{
		strtemp.push_back(s[i]);
	}
	//��ȡÿ��Ԫ�صĳ��ִ���
	std::vector<int> a_per;
	for (int r = 0; r < strtemp.size(); r++)
	{
		std::string stp = strtemp[r];
		int count = 0;
		for (int c = 0; c < strtemp.size(); c++)
		{
			if (stp == strtemp[c])
			{
				count++;
			}
		}
		a_per.push_back(count);
	}
	//�ҳ����ִ�������Ԫ�ص�ID
	int id;
	int itemp = a_per[0];
	id = 0;
	for (int i = 0; i < a_per.size(); i++)
	{
		if (a_per[i] >= itemp)
		{
			itemp = a_per[i];
			id = i;
		}
	}

	return strtemp[id];
}

std::string az::ZLJG::doit(std::string str)
{
	push(str);
	return get();
}

//��ȡ�ļ����������ļ����������ļ����µ��ļ����ľ���·��,����浽vector<string>& files��
//�ٶȺܿ죬һ�����ļ�Ҳ�����Ҳ�ܽ�׳���ļ��������к��е㡰.��Ҳ��ͨ����
void az::getAllFilesPath(string path, vector<string>& files)
{
	//�ļ����
	long   hFile = 0;
	//�ļ���Ϣ
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮
			//�������,�����б�
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getAllFilesPath(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}


//�������ֵ,����Ҷ�ͼ��Ĭ�ϻҽ�256
int az::get_th_useOtsu(cv::Mat *img, int gray_scale, bool mask)
{
#define GrayScale gray_scale   //frame�Ҷȼ�  
	if (img->channels() != 1)
	{
		return 0;
	}

	int nr = img->rows;
	int nc = img->cols;
	int *pixelCount = new int[GrayScale];
	float *pixelPro = new float[GrayScale];
	for (int i = 0; i < GrayScale; i++)
	{
		pixelCount[i] = 0;
		pixelPro[i] = 0;
	}
	int i, j, pixelSum = 0, threshold = 0;

	//ͳ��ÿ���Ҷȼ������صĸ���
	if (mask)
	{
		for (i = 0; i < nr; i++)
		{
			uchar* data = img->ptr<uchar>(i);
			for (j = 0; j < nc; j++)
			{
				int data_v = (int)*data++;
				if (data_v > 3)
				{
					pixelCount[data_v]++;
					pixelSum++;
				}
			}
		}
	}
	else
	{
		for (i = 0; i < nr; i++)
		{
			uchar* data = img->ptr<uchar>(i);
			for (j = 0; j < nc; j++)
			{
				pixelCount[(int)*data++]++;
				pixelSum++;
			}
		}
	}

	//����ÿ���Ҷȼ���������Ŀռ����ͼ��ı���  
	for (i = 0; i < GrayScale; i++)
	{
		pixelPro[i] = (float)pixelCount[i] / pixelSum;
	}

	//�����Ҷȼ�,Ѱ�Һ��ʵ�threshold  
	float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp, deltaMax = 0;
	for (i = 0; i < GrayScale; i++)
	{
		w0 = w1 = u0tmp = u1tmp = u0 = u1 = deltaTmp = 0;
		for (j = 0; j < GrayScale; j++)
		{
			if (j <= i)   //��������  
			{
				w0 += pixelPro[j];
				u0tmp += j * pixelPro[j];
			}
			else   //ǰ������  
			{
				w1 += pixelPro[j];
				u1tmp += j * pixelPro[j];
			}
		}
		u0 = u0tmp / w0;
		u1 = u1tmp / w1;
		deltaTmp = (float)(w0 *w1* pow((u0 - u1), 2));
		if (deltaTmp > deltaMax)
		{
			deltaMax = deltaTmp;
			threshold = i;
		}
	}

	delete[] pixelCount;
	delete[] pixelPro;
	return threshold;
}


//����HSV�ռ䣬ɸѡĳһͨ����ĳ��Χ�����أ�Ĭ��hͨ��������һ����ֵͼ��
cv::Mat az::sel_fromhsv(cv::Mat* img, int min, int max, char flg)
{
	if (img->channels() != 3)
		return *img;
	cv::Mat pic;
	cv::Mat res(img->size(), CV_8UC1);
	img->copyTo(pic);	//����ת���ռ�
	cv::cvtColor(pic, pic, CV_BGR2HSV);
	std::vector<cv::Mat> picsplit;
	split(pic, picsplit);
	int nr = img->rows;
	int nc = img->cols;
	if (flg == 'h')
	{
		for (int i = 0; i < nr; i++)
		{
			uchar* data_p = pic.ptr<uchar>(i);
			uchar* data_r = res.ptr<uchar>(i);
			for (int j = 0; j < nc; j++)
			{
				int dp = *data_p;
				if (dp >= min&&dp <= max)
					*data_r = 255;
				else
					*data_r = 0;
				data_p += 3;
				data_r++;
			}
		}
	}
	if (flg == 's')
	{
		for (int i = 0; i < nr; i++)
		{
			uchar* data_p = pic.ptr<uchar>(i)+1;
			uchar* data_r = res.ptr<uchar>(i);
			for (int j = 0; j < nc; j++)
			{
				int dp = *data_p;
				if (dp >= min&&dp <= max)
					*data_r = 255;
				else
					*data_r = 0;
				data_p += 3;
				data_r++;
			}
		}
	}
	if (flg == 'v')
	{
		for (int i = 0; i < nr; i++)
		{
			uchar* data_p = pic.ptr<uchar>(i)+2;
			uchar* data_r = res.ptr<uchar>(i);
			for (int j = 0; j < nc; j++)
			{
				int dp = *data_p;
				if (dp >= min&&dp <= max)
					*data_r = 255;
				else
					*data_r = 0;
				data_p += 3;
				data_r++;
			}
		}
	}

	//�����ض�ֵͼ�񣬷��ز�ɫͼ��
	//��res����mask�����±�����һ��ͼ��
	cv::Mat res3C(img->size(), CV_8UC3);
	for (int i = 0; i < nr; i++)
	{
		uchar* data_p = img->ptr<uchar>(i);
		uchar* data_r = res.ptr<uchar>(i);
		uchar* data_r3 = res3C.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			if (*data_r == 255)
			{
				*data_r3		= *data_p;
				*(data_r3 + 1)	= *(data_p + 1);
				*(data_r3 + 2)	= *(data_p + 2);
			}
			else
			{
				*data_r3		= 0;
				*(data_r3 + 1)	= 0;
				*(data_r3 + 2)	= 0;
			}
			data_p += 3;
			data_r3 += 3;
			data_r++;
		}
	}
	return res3C;

	return res;
}

//�ж�һ�����Ƿ���һ��ͼ����
bool az::p_isinimg(CvPoint p, cv::Mat* m)
{
	if (p.x >= 0 && p.x < m->size().width &&
		p.y >= 0 && p.y < m->size().height)
		return true;
	else
		return false;
}
//�ܾ�һ���㣬�󸽽�����ͨ���򣬷���һ������
std::vector<CvPoint> az::connectedregion(cv::Mat& im, CvPoint p)
{
	cv::Mat m;
	im.copyTo(m);
	std::vector<CvPoint> ps;
	CvPoint *pss = new CvPoint[310000];	//����ͨ��������
	int p_ = 1;		//��¼ָ��λ��
	int numb = 1;		//�¼�������صĸ���

	pss[0] = p;
	while (1)
	{
		int k = numb;	//��������
		numb = 0;		//����
		for (int i = p_ - 1; i >= p_ - k; i--)	//���¼���ĵ�
		{
			p = pss[i];
			CvPoint p1;
			p1 = CvPoint(p.x, p.y + 1);
			if (az::p_isinimg(p1, &m))
			{
				//std::cout << p1.x << " " << p1.y << std::endl;
				if (m.at<uchar>(p1) >=100)
				{
					pss[p_ + numb] = p1;
					numb++;
					m.at<uchar>(p1) = 0;
				}
			}
			p1 = CvPoint(p.x, p.y - 1);
			if (az::p_isinimg(p1, &m))	
				if (m.at<uchar>(p1) >=100)	{ pss[p_ + numb] = p1;	numb++;	m.at<uchar>(p1) = 0; }
			p1 = CvPoint(p.x - 1, p.y + 1);
			if (az::p_isinimg(p1, &m))	
				if (m.at<uchar>(p1) >=100)	{ pss[p_ + numb] = p1;	numb++;	m.at<uchar>(p1) = 0; }
			p1 = CvPoint(p.x - 1, p.y);
			if (az::p_isinimg(p1, &m))	
				if (m.at<uchar>(p1) >=100)	{ pss[p_ + numb] = p1;	numb++;	m.at<uchar>(p1) = 0; }
			p1 = CvPoint(p.x - 1, p.y - 1);
			if (az::p_isinimg(p1, &m))	
				if (m.at<uchar>(p1) >=100)	{ pss[p_ + numb] = p1;	numb++;	m.at<uchar>(p1) = 0; }
			p1 = CvPoint(p.x + 1, p.y + 1);
			if (az::p_isinimg(p1, &m))	
				if (m.at<uchar>(p1) >=100)	{ pss[p_ + numb] = p1;	numb++;	m.at<uchar>(p1) = 0; }
			p1 = CvPoint(p.x + 1, p.y - 1);
			if (az::p_isinimg(p1, &m))	
				if (m.at<uchar>(p1) >=100)	{ pss[p_ + numb] = p1;	numb++;	m.at<uchar>(p1) = 0; }
			p1 = CvPoint(p.x + 1, p.y);
			if (az::p_isinimg(p1, &m))	
				if (m.at<uchar>(p1) >=100)	{ pss[p_ + numb] = p1;	numb++;	m.at<uchar>(p1) = 0; }
		}

		p_ = p_ + numb;		//ָ��������
		if (numb == 0)	break;		//û���¼���ĵ�������
	}

	for (int i = 0; i < p_; i++)
	{
		ps.push_back(pss[i]);
	}
	delete[] pss;

	return ps;
}
//���ֵͼ�����ͨ���򣬽�����浽vs
void az::findconnectedregions(cv::Mat* im, std::vector<std::vector<CvPoint>> &vs)
{
	std::vector<CvPoint> ptem;
	cv::Mat m;
	im->copyTo(m);
	int h = m.size().height;
	int w = m.size().width;

	for (int i = 0; i < h; i++)
	{
		uchar* data = m.ptr<uchar>(i);
		for (int j = 0; j < w; j++)
		{
			if (*data++ >= 100)
			{
				ptem = az::connectedregion(m, CvPoint(j, i));
				vs.push_back(ptem);
				for (int k = 0; k < ptem.size(); k++)	//�Ѹ������û��ɰ�ɫ
				{
					m.at<uchar>(ptem[k]) = 0;
				}
			}
		}
	}
}
//�����ͨ�����ö�����ʾ��α���ĵ㣨������ĵ㣩���������ĵ㣨������ĵ㣩
void az::regions2points(std::vector<std::vector<cv::Point>> src_rs, std::vector<cv::Point> &dst_ps)
{
#define ZHENZHONGXINDIAN
#ifdef WEIZHONGXINDIAN
	int size = src_rs.size();
	int size_;
	for (int i = 0; i < size;i++)
	{
		size_ = src_rs[i].size();
		dst_ps.push_back(src_rs[i][(int)(size_ / 2)]);
	}
#endif
#ifdef ZHENZHONGXINDIAN
	int size = src_rs.size();
	int size_;
	int xx = 0, yy = 0;
	for (int i = 0; i < size; i++)
	{
		xx = 0, yy = 0;
		size_ = src_rs[i].size();
		for (int j = 0; j < size_; j++)
		{
			xx += src_rs[i][j].x;
			yy += src_rs[i][j].y;
		}
		xx /= size_;
		yy /= size_;
		dst_ps.push_back(cv::Point(xx, yy));
	}
#endif
}

//����һ�������ܱ�����ͨ���򣨰���ͨ����������ȱ���
void az::pot_erg(cv::Mat *pic, cv::Point p, std::vector<cv::Point> &dst_ps)
{
	cv::Point p_;
	int r = pic->rows, c = pic->cols;
	if (pic->at<uchar>(p)>200)
	{
		dst_ps.push_back(p);
		pic->at<uchar>(p) = 0;
		p_.x = p.x - 1, p_.y = p.y - 1;
		if (p_.x >= 0 && p_.x < c && p_.y >= 0 && p_.y < r)	pot_erg(pic, p_, dst_ps);
		p_.x = p.x - 1, p_.y = p.y;
		if (p_.x >= 0 && p_.x < c && p_.y >= 0 && p_.y < r)	pot_erg(pic, p_, dst_ps);
		p_.x = p.x - 1, p_.y = p.y + 1;
		if (p_.x >= 0 && p_.x < c && p_.y >= 0 && p_.y < r)	pot_erg(pic, p_, dst_ps);
		p_.x = p.x, p_.y = p.y - 1;
		if (p_.x >= 0 && p_.x < c && p_.y >= 0 && p_.y < r)	pot_erg(pic, p_, dst_ps);
		p_.x = p.x, p_.y = p.y + 1;
		if (p_.x >= 0 && p_.x < c && p_.y >= 0 && p_.y < r)	pot_erg(pic, p_, dst_ps);
		p_.x = p.x + 1, p_.y = p.y - 1;
		if (p_.x >= 0 && p_.x < c && p_.y >= 0 && p_.y < r)	pot_erg(pic, p_, dst_ps);
		p_.x = p.x + 1, p_.y = p.y;
		if (p_.x >= 0 && p_.x < c && p_.y >= 0 && p_.y < r)	pot_erg(pic, p_, dst_ps);
		p_.x = p.x + 1, p_.y = p.y + 1;
		if (p_.x >= 0 && p_.x < c && p_.y >= 0 && p_.y < r)	pot_erg(pic, p_, dst_ps);
	}
	return;
}
//����һ�Ŷ�ֵͼ������ͨ����
void az::findLTQY(cv::Mat* pic, std::vector<std::vector<cv::Point>> &pss)
{
	cv::Mat temp;
	pic->copyTo(temp);
	std::vector<cv::Point> tem_ps;
	int r = temp.rows, c = temp.cols;
	for (int i = 0; i < r; i++)
	{
		uchar* data = temp.ptr<uchar>(i);
		for (int j = 0; j < c; j++)
		{
			if (*data++ > 200)
			{
				tem_ps.clear();
				pot_erg(&temp, cv::Point(j, i), tem_ps);
				pss.push_back(tem_ps);
			}
		}
	}
	return;
}

//��ֵͼ��ȥ�롾��ͨ���򷨡��������ɫ�����ģ�ȥ����ɫǰ���к�ɫС��ͨ����
void az::quxiaodong(cv::Mat *pic, int max_th)
{
	std::vector<std::vector<CvPoint>> vs;
	az::findconnectedregions(pic, vs);
	for (int i = 0; i < vs.size(); i++)
	{
		if (vs[i].size() < max_th)
		{
			for (int j = 0; j < vs[i].size(); j++)
			{
				pic->at<uchar>(vs[i][j]) = 255;
			}
		}
	}

	return;
}

/**
* @brief split a string by delim
*
* @param str string to be splited
* @param c delimiter, const char*, just like " .,/", white space, dot, comma, splash
*
* @return a string vector saved all the splited world
*/
vector<string> split(string& str, const char* c)
{
	char *cstr, *p;
	vector<string> res;
	cstr = new char[str.size() + 1];
	strcpy(cstr, str.c_str());
	p = strtok(cstr, c);
	while (p != NULL)
	{
		res.push_back(p);
		p = strtok(NULL, c);
	}
	return res;
}