//#include "stdafx.h"
#include "zqjtools.hpp"

//显示进程当前使用内存和历史峰值使用内存；
void az::showMemoryInfo(void)
{
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	
	cout << "********************************\n\n"
		<< "内存使用:\n\n"
		<< "当前内存："<<pmc.WorkingSetSize / 1000000 << "MB\n\n" 
		<< "峰值内存："<< pmc.PeakWorkingSetSize / 1000000 << "MB\n\n" 
		<< "********************************"<< endl;
}

//对数组随机排序
void az::random_sort(int* x, int size)	//对数组随机排序
{
	for (int i = size; i >= 2; i--)
	{
		int index = rand() % i;
		int temp = x[index];
		x[index] = x[i - 1];
		x[i - 1] = temp;
	}
}

//获取当前时间,返回毫秒
long az::getTime()
{
	return clock();
}

#ifdef USE_OPENCV
//图像大小转换
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
		return "处理中...";
	}
	std::vector<std::string> strtemp;
	for (int i = 0; i < 5; i++)
	{
		strtemp.push_back(s[i]);
	}
	//获取每个元素的出现次数
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
	//找出出现次数最大的元素的ID
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

//获取文件夹下所有文件（包括子文件夹下的文件）的绝对路径,结果存到vector<string>& files中
//速度很快，一万多个文件也是秒获；也很健壮，文件夹命名中含有点“.”也能通过。
void az::getAllFilesPath(string path, vector<string>& files)
{
	//文件句柄
	long   hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
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


//大津法求阈值,输入灰度图，默认灰阶256
int az::get_th_useOtsu(cv::Mat *img, int gray_scale, bool mask)
{
#define GrayScale gray_scale   //frame灰度级  
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

	//统计每个灰度级中像素的个数
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

	//计算每个灰度级的像素数目占整幅图像的比例  
	for (i = 0; i < GrayScale; i++)
	{
		pixelPro[i] = (float)pixelCount[i] / pixelSum;
	}

	//遍历灰度级,寻找合适的threshold  
	float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp, deltaMax = 0;
	for (i = 0; i < GrayScale; i++)
	{
		w0 = w1 = u0tmp = u1tmp = u0 = u1 = deltaTmp = 0;
		for (j = 0; j < GrayScale; j++)
		{
			if (j <= i)   //背景部分  
			{
				w0 += pixelPro[j];
				u0tmp += j * pixelPro[j];
			}
			else   //前景部分  
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


//根据HSV空间，筛选某一通道在某范围的像素，默认h通道，返回一个二值图像
cv::Mat az::sel_fromhsv(cv::Mat* img, int min, int max, char flg)
{
	if (img->channels() != 3)
		return *img;
	cv::Mat pic;
	cv::Mat res(img->size(), CV_8UC1);
	img->copyTo(pic);	//用来转换空间
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

	//不返回二值图像，返回彩色图像
	//把res当做mask，重新遍历出一副图像
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

//判断一个点是否在一个图像内
bool az::p_isinimg(CvPoint p, cv::Mat* m)
{
	if (p.x >= 0 && p.x < m->size().width &&
		p.y >= 0 && p.y < m->size().height)
		return true;
	else
		return false;
}
//很据一个点，求附近的连通区域，返回一个区域
std::vector<CvPoint> az::connectedregion(cv::Mat& im, CvPoint p)
{
	cv::Mat m;
	im.copyTo(m);
	std::vector<CvPoint> ps;
	CvPoint *pss = new CvPoint[310000];	//存连通区域像素
	int p_ = 1;		//记录指针位置
	int numb = 1;		//新加入的像素的个数

	pss[0] = p;
	while (1)
	{
		int k = numb;	//轮廓长度
		numb = 0;		//归零
		for (int i = p_ - 1; i >= p_ - k; i--)	//求新加入的点
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

		p_ = p_ + numb;		//指针往后移
		if (numb == 0)	break;		//没有新加入的点了跳出
	}

	for (int i = 0; i < p_; i++)
	{
		ps.push_back(pss[i]);
	}
	delete[] pss;

	return ps;
}
//求二值图像的连通区域，结果保存到vs
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
				for (int k = 0; k < ptem.size(); k++)	//把该区域置换成白色
				{
					m.at<uchar>(ptem[k]) = 0;
				}
			}
		}
	}
}
//多个连通区域用多个点表示，伪中心点（序号中心点），或真中心点（面积中心点）
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

//根据一点向四周遍历连通区域（八连通），属于深度遍历
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
//输入一张二值图像，求连通区域
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

//二值图像去噪【连通区域法】，比如黑色背景的，去掉白色前景中黑色小连通区域
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