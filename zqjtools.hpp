/////////////////////////////////
//  ┏┓　　　┏┓
// ┏┛┻━━━┛┻┓
// ┃　　　　　　　┃ 　
// ┃　　　━　　　┃
// ┃　┳┛　┗┳　┃
// ┃　　　　　　　┃
// ┃　　　┻　　　┃
// ┃　　　　　　　┃
// ┗━┓　　　┏━┛
//     ┃　　　┃   　　　　　　　　　
//     ┃　　　┃
//     ┃　　　┗━━━┓
//     ┃　　　　　　　┣┓
//     ┃　　　　　　　┏┛
//     ┗┓┓┏━┳┓┏┛
//       ┃┫┫　┃┫┫
//       ┗┻┛　┗┻┛
//
// Created by ZQJ, 2016.8.8 ‏‎18:04:17 PM.
// Copyright (c) 2016, 228812066@qq.com All Rights Reserved.

#pragma once

#define USE_OPENCV

#ifdef USE_OPENCV
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#endif
#include <math.h>
#include <string>
#include <fstream>
#include <iostream>
#include <time.h>
#include <windows.h>
#include <io.h>
#include <vector>
#include <psapi.h>    
#pragma comment(lib,"psapi.lib")  

using namespace std;

namespace az
{

	//显示进程当前使用内存和历史峰值使用内存，在控制台窗口打印；
	void showMemoryInfo(void);

	//对数组随机排序
	void random_sort(int* x, int size);

	//获取当前时间，返回毫秒
	long getTime();

#ifdef USE_OPENCV
	//图像转换大小
	cv::Mat imgto64(cv::Mat *img);
	cv::Mat imgto(cv::Mat *img, int w, int h);
#endif

	//整理结果类，5个结果里选出出现次数最多的
	//用法：一句话，ZLJG zl; zl.doit(str);
	class ZLJG
	{
	public:
		ZLJG();
		std::string doit(std::string str);	//使用

	private:
		void push(std::string ss);
		std::string get();
		std::string s[5];
		int size_now;
	};
	
	//获取文件夹下所有文件（包括子文件夹下的文件）的绝对路径,结果存到vector<string>& files中
	//速度很快，一万多个文件也是秒获，也很健壮，文件夹命名中含有点“.”也能通过。
	void getAllFilesPath(string path, vector<string>& files);
	
	//大津法求阈值,输入灰度图，默认灰阶256，开启mask后自动过滤黑色背景
	int get_th_useOtsu(cv::Mat *img, int gray_scale = 256, bool mask = false);

	//根据HSV空间，筛选某一通道在某范围的像素，默认h通道，返回一个二值图像
	cv::Mat sel_fromhsv(cv::Mat* img, int min, int max, char flg = 'h');

	//判断一个点是否在一个图像内
	bool p_isinimg(CvPoint p, cv::Mat* m);
	//很据一个点，求附近的连通区域，返回一个区域
	std::vector<CvPoint> connectedregion(cv::Mat& m, CvPoint p);
	//求二值图像的连通区域，结果保存到vs，返回一组连通区域【老版本】
	void findconnectedregions(cv::Mat* m, std::vector<std::vector<CvPoint>> &vs);
	//多个连通区域用多个点表示，伪中心点（序号中心点），或真中心点（面积中心点）
	void regions2points(std::vector<std::vector<cv::Point>> src_rs, std::vector<cv::Point> &dst_ps);

	//根据一点向四周遍历连通区域（八连通），会改变pic
	void pot_erg(cv::Mat *pic, cv::Point p, std::vector<cv::Point> &dst_ps);
	//输入一张二值图像，求连通区域【新版本，建议使用】
	void findLTQY(cv::Mat* pic, std::vector<std::vector<cv::Point>> &pss);

	//二值图像去噪，比如黑色背景的，去掉白色前景中黑色小连通区域
	void quxiaodong(cv::Mat *pic, int max_th);

	/**
	* @brief split a string by delim
	*
	* @param str string to be splited
	* @param c delimiter, const char*, just like " .,/", white space, dot, comma, splash
	*
	* @return a string vector saved all the splited world
	*/
	vector<string> split(string& str, const char* c);

}
