/*****************************************************************
*
* fileName:  filter.h
*
* function:  ��ͼ����в�ͬ�㷨���˲�
*
* author:    sjt&ztb
*
* date:      2021.1.26
*
* notes:     �˲��Լ�ȥë��

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/

#pragma once
#ifndef FILTER_H
#define FILTER_H

 #include <functional>
 #include <omp.h>
 #include "baseFunc.h"

// �˲�����
enum FILTER_TYPE {
	FILTER_TYPE_NONE = 0,                        // ���˲�
	FILTER_TYPE_LOW_PASS,                        // ��ͨ�˲�
	FILTER_TYPE_HIGH_PASS,                       // ��ͨ�˲�
	FILTER_TYPE_BAND_PASS,                       // ��ͨ�˲�
	FILTER_TYPE_BAND_REJECT                      // �����˲�
};

// �˲�����
enum FILTER_METHOD {
	FILTER_METHOD_AVERAGE = 0,                   // ��ֵ�˲�
	FILTER_METHOD_MEDIAN,                        // ��ֵ�˲�
	FILTER_METHOD_2_SIGMA,                       // 
	FILTER_METHOD_FFT_AUTO,                      // 
	FILTER_METHOD_FFT_FIXED,                     // 
	FILTER_METHOD_GAUSS_SPLINE,                  // 
	FILTER_METHOD_GAUSS_SPLINE_AUTO,             // 
	FILTER_METHOD_ROBUST_GAUSS_SPLINE,           // 
	FILTER_METHOD_ROBUST_GAUSS_SPLINE_AUTO       // 
};

// �Ƴ�spikes�Ĳ���
struct REVOME_SPIKES_PARAMS {
	bool rsFlag = false;                         // �Ƿ��Ƴ�spikes
	int removeSize = 2;                          // �Ƴ���İ뾶
	int slopeSize = 3;                           // �����ݶȵĳߴ�Sobel
	float rsThreshCoef = 4.0f;                   // �Ƴ�spikes��ֵ��ϵ��
};

struct FILTER_PARAMS {
	unsigned short filterWindowSize = 3;         // (3, 5, 7, ... , 49) ����
	unsigned short filterIterTimes = 1;          // ��������
	unsigned short morphoTimes = 0;              // ��̬ѧ�����������
	unsigned short morphoSize = 3;               // ��̬ѧ����ߴ�
	FILTER_TYPE filterType = FILTER_TYPE_NONE;   // �˲�����
	FILTER_METHOD filterMethod = FILTER_METHOD_MEDIAN; // �˲�����
	REVOME_SPIKES_PARAMS removeSpikesParams;     // �Ƴ�spikes�Ĳ���
};

class Filter {
public:
	FILTER_PARAMS filterParams;
	cv::Mat mask;

public:
	Filter(FILTER_PARAMS filterP) : filterParams(filterP) {}
	
	virtual ~Filter() {}
	
	void FilterProcess(cv::Mat &phase);

	//��λ�˲�
	void FilterSin(cv::Mat &phase, int iterTimes, int winSize);

	// Ƶ�����˲�
	cv::Mat frequency_filter(cv::Mat &scr, cv::Mat &blur);

	// �����ͨ�˲�
	cv::Mat ideal_low_pass_filter(cv::Mat &src, float sigma);

	// ������˹��ͨ�˲�
	cv::Mat butterworth_low_paass_filter(cv::Mat &src, float d0, int n);

	// ������˹��ͨ�˲�
	cv::Mat butterworth_high_pass_filter(cv::Mat &src, float d0, int n);

	// ��˹��ͨ�˲�
	cv::Mat gaussian_low_pass_filter(cv::Mat &src, float d0);

	// ��˹��ͨ�˲�
	cv::Mat gaussian_high_pass_filter(cv::Mat &src, float d0);

private:
	/**
	 * @brief SplitTilt               �õ���б��������λͼ��ȥ����б��
	 * @param src                     ��λͼ
	 * @param tilt                    ��б��
	 * @param mask                    mask
	 */
	void SplitTilt(cv::Mat &phase, cv::Mat& tilt);

	//�Ƴ�����
	void FilterSlopeSpike(cv::Mat &phase);

	//��ͼ�������ֵ�˲�
	void FilterMedian(cv::Mat &phase);

	//��ֵ�˲�
	void FilterBlur(cv::Mat &phase);

	//��Ĥ��������䣺Ϊ�˱�����Ĥ�߽��ֵ��0ֵӰ�쵼���쳣
	void FillMaskData(cv::Mat &phase);

	// ͼ��߽紦��
	cv::Mat image_make_border(cv::Mat &src);
	
	// �����ͨ�˲��˺���
	cv::Mat ideal_low_kernel(cv::Mat &scr, float sigma);
	
	// ������˹��ͨ�˲��˺���
	cv::Mat butterworth_low_kernel(cv::Mat &scr, float sigma, int n);

	// ��˹��ͨ�˲��˺���
	cv::Mat gaussian_low_pass_kernel(cv::Mat scr, float sigma);
	
	// �����ͨ�˲��˺���
	cv::Mat ideal_high_kernel(cv::Mat &scr, float sigma);
	
	// �����ͨ�˲�
	cv::Mat ideal_high_pass_filter(cv::Mat &src, float sigma);

	// ������˹��ͨ�˲��˺���
	cv::Mat butterworth_high_kernel(cv::Mat &scr, float sigma, int n);

	// ��˹��ͨ�˲��˺���
	cv::Mat gaussian_high_pass_kernel(cv::Mat scr, float sigma);
	
	// ʵ��Ƶ���˲�����������
	void getcart(int rows, int cols, cv::Mat &x, cv::Mat &y);

	// fft�任�����Ƶ�װ���
	void fftshift(cv::Mat &plane0, cv::Mat &plane1);

};

#endif // FILTER_H
