/*****************************************************************
*
* fileName:  DynamicCarrier.h
*
* function:  动态载波技术
*
* author:    ztb（Steven）&lzs
*
* date:      2021.5.13
*
* notes:     动态载波技术

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/

#pragma once
#ifndef DYNAMICCARRIER_H
#define DYNAMICCARRIER_H

#include <Windows.h>
#include "baseFunc.h"
using namespace std;

// 输入参数
struct DC_PARAMS {
	bool Accurate_sign = true;
	bool isAuto = true;
	int DC_select_r = 0;
	int DC_select_x = 0;
	int DC_select_y = 0;
};
struct DC_SELECT_LOCATION {
	int select_r = 0;
	cv::Point select_p = cv::Point(0, 0);
};

class DynamicCarrier
{
public:
	DC_SELECT_LOCATION location;
	cv::Mat DCphase;
	cv::Mat DCFFTlog;
	ERROR_TYPE error = ERROR_TYPE_NOT_ERROR;
	DC_PARAMS dcParams;

public:
	DynamicCarrier(DC_PARAMS params):dcParams(params){};

	~DynamicCarrier() {};

	/**
	* @brief DynamicProcess    动态载波算法接口
	* @param phase             输入的条纹图
	* @param mask              输入的掩模图
	* @return
	*/
	void DynamicProcess(cv::Mat phase, cv::Mat mask);

	/**
	* @brief DC_Adaptive_Simulation 动态载波自适应模拟条纹
	* @param center                 对准图中心点坐标
	* @param current                对准图当前对准坐标
	* @param target                 对准图目标对准坐标
	* @return
	*/
	static cv::Mat DC_Adaptive_Simulation(cv::Point &center,cv::Point &current, cv::Point &target);

private:

	// 频域识别，自动选择高频信息带
	cv::Mat FD_Auto_Select(cv::Mat &fft,cv::Mat &fftlog,cv::Point &center,int &radius );
	// 频域识别，手动选择高频信息带
	cv::Mat FD_Manual_Select(cv::Mat &fft, cv::Mat &fftlog,int &x,int &y,int &r);
	// fft变换后进行频谱搬移
	void fftshift(cv::Mat &plane0, cv::Mat &plane1);
	// 图像边界处理（扩展区域数值为0）
	cv::Mat image_make_border(cv::Mat &src);
	// 图像边界处理（扩展区域数值为nan）
	cv::Mat image_make_border_nan(cv::Mat src);
	// 傅里叶变换
	cv::Mat FFT(cv::Mat &scr);
	// 反傅里叶变换
	cv::Mat IFFT(cv::Mat &scr);
	// 傅里叶变换的结果Log化，便于分析高频区域
	cv::Mat FFT_LOG(cv::Mat &fft);
	// 掩膜外数据填充，平均思想
	cv::Mat FillMaskData_Avergae(cv::Mat &phase, cv::Mat fmask);

	/**
     * @brief DC_Simulated_Fringes 动态载波生成模拟条纹
     * @param mask                 输入的掩模图
     * @param number               条纹数量
     * @param angle                条纹角度
     * @return
     */
	static cv::Mat DC_Simulated_Fringes(cv::Mat mask, float number, float angle);

};
#endif    //DYNAMICCARRIER_H
