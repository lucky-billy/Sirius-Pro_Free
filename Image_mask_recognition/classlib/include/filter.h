/*****************************************************************
*
* fileName:  filter.h
*
* function:  对图像进行不同算法的滤波
*
* author:    sjt&ztb（Steven）
*
* date:      2021.1.26
*
* notes:     滤波以及去毛刺

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/

#pragma once
#ifndef FILTER_H
#define FILTER_H

 #include <functional>
 #include <omp.h>
 #include "baseFunc.h"

// 滤波类型
enum FILTER_TYPE {
	FILTER_TYPE_NONE = 0,                        // 不滤波
	FILTER_TYPE_LOW_PASS,                        // 低通滤波
	FILTER_TYPE_HIGH_PASS,                       // 高通滤波
	FILTER_TYPE_BAND_PASS,                       // 带通滤波
	FILTER_TYPE_BAND_REJECT                      // 带阻滤波
};

// 滤波方法
enum FILTER_METHOD {
	FILTER_METHOD_AVERAGE = 0,                   // 均值滤波
	FILTER_METHOD_MEDIAN,                        // 中值滤波
	FILTER_METHOD_2_SIGMA,                       // 
	FILTER_METHOD_FFT_AUTO,                      // 
	FILTER_METHOD_FFT_FIXED,                     // 
	FILTER_METHOD_GAUSS_SPLINE,                  // 
	FILTER_METHOD_GAUSS_SPLINE_AUTO,             // 
	FILTER_METHOD_ROBUST_GAUSS_SPLINE,           // 
	FILTER_METHOD_ROBUST_GAUSS_SPLINE_AUTO       // 
};

// 移除spikes的参数
struct REVOME_SPIKES_PARAMS {
	bool rsFlag = false;                         // 是否移除spikes
	int removeSize = 2;                          // 移除框的半径
	int slopeSize = 3;                           // 计算梯度的尺寸Sobel
	float rsThreshCoef = 4.0f;                   // 移除spikes阈值的系数
};

struct FILTER_PARAMS {
	bool isQuadflip = true;                     // 是否使用四象翻转法，效果更好，但是速度更慢
	unsigned short filterWindowSize = 3;         // (3, 5, 7, ... , 49) 奇数
	unsigned short filterIterTimes = 1;          // 迭代次数
	unsigned short morphoTimes = 0;              // 形态学运算迭代次数
	unsigned short morphoSize = 3;               // 形态学运算尺寸
	float high_freq = 100000.0f;                 // 高截止频率
	float low_freq = 0.0f;                       // 低截止频率
	float px_1mm = 60.0f;                        // 每mm代表的像素个数
	FILTER_TYPE filterType = FILTER_TYPE_NONE;   // 滤波类型
	FILTER_METHOD filterMethod = FILTER_METHOD_MEDIAN; // 滤波方法
	REVOME_SPIKES_PARAMS removeSpikesParams;     // 移除spikes的参数
};

class Filter {
public:
	bool ASCFlag;
	bool HaveTilt = true;
	bool isFFT = false;
	cv::Mat mask;
	ERROR_TYPE errorType = ERROR_TYPE_NOT_ERROR;           // 错误类型
	FILTER_PARAMS filterParams;

public:
	Filter(FILTER_PARAMS filterP,bool isASC=false) : filterParams(filterP),ASCFlag(isASC) {}
	
	virtual ~Filter() {}
	
	void FilterProcess(cv::Mat &phase);

	//相位滤波
	void FilterSin(cv::Mat &phase, int iterTimes, int winSize);

	// 频率域滤波
	cv::Mat frequency_filter(cv::Mat &scr, cv::Mat &blur);

	// 理想低通滤波
	cv::Mat ideal_low_pass_filter(cv::Mat &src, float sigma);

	// 巴特沃斯低通滤波
	cv::Mat butterworth_low_pass_filter(cv::Mat &src, float d0, int n);

	// 巴特沃斯高通滤波
	cv::Mat butterworth_high_pass_filter(cv::Mat &src, float d0, int n);

	// 高斯低通滤波
	cv::Mat gaussian_low_pass_filter(cv::Mat &src, float d0);

	// 高斯高通滤波
	cv::Mat gaussian_high_pass_filter(cv::Mat &src, float d0);

private:
	/**
	 * @brief SplitTilt               得到倾斜量并在相位图中去除倾斜量
	 * @param src                     相位图
	 * @param tilt                    倾斜量
	 * @param mask                    mask
	 */
	void SplitTilt(cv::Mat &phase, cv::Mat& tilt);

	// 移除尖峰点
	void FilterSlopeSpike(cv::Mat &phase);

	// 对图像进行中值滤波
	void FilterMedian(cv::Mat &phase);

	// 均值滤波
	void FilterBlur(cv::Mat &phase);

	// 低通滤波-傅里叶
	void FilterFFT_Low(cv::Mat &phase,float high,float deltx);

	// 高通滤波-傅里叶
	void FilterFFT_High(cv::Mat &phase, float low,float deltx);

	// 带通滤波-傅里叶
	void FilterFFT_Band(cv::Mat &phase, float high, float low,float deltx);

	// 掩膜外数据填充：为了避免掩膜边界的值受0值影响导致异常
	void FillMaskData(cv::Mat &phase,cv::Mat fmask);

	// 掩膜外数据填充，平均法
	cv::Mat FillMaskData_Avergae(cv::Mat &phase, cv::Mat fmask);

	// 图像边界处理(扩展值为0)
	cv::Mat image_make_border(cv::Mat &src);
	
	// 图像边界处理(扩展值为nan)
	cv::Mat image_make_border_nan(cv::Mat &src);

	// 理想低通滤波核函数
	cv::Mat ideal_low_kernel(cv::Mat &scr, float sigma);
	
	// 巴特沃斯低通滤波核函数
	cv::Mat butterworth_low_kernel(cv::Mat &scr, float sigma, int n);

	// 高斯低通滤波核函数
	cv::Mat gaussian_low_pass_kernel(cv::Mat scr, float sigma);
	
	// 理想高通滤波核函数
	cv::Mat ideal_high_kernel(cv::Mat &scr, float sigma);
	
	// 理想高通滤波
	cv::Mat ideal_high_pass_filter(cv::Mat &src, float sigma);

	// 巴特沃斯高通滤波核函数
	cv::Mat butterworth_high_kernel(cv::Mat &scr, float sigma, int n);

	// 高斯高通滤波核函数
	cv::Mat gaussian_high_pass_kernel(cv::Mat scr, float sigma);
	
	// 理想带通滤波核函数
	cv::Mat ideal_band_kernel(cv::Mat &scr, float sigmah,float sigmal);

	// 实现频域滤波器的网格函数
	void getcart(int rows, int cols, cv::Mat &x, cv::Mat &y);

	// fft变换后进行频谱搬移
	void fftshift(cv::Mat &plane0, cv::Mat &plane1);

};

#endif // FILTER_H
