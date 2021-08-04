/*****************************************************************
*
* fileName:  DynamicCarrier.h
*
* function:  ��̬�ز�����
*
* author:    ztb��Steven��&lzs
*
* date:      2021.5.13
*
* notes:     ��̬�ز�����

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/

#pragma once
#ifndef DYNAMICCARRIER_H
#define DYNAMICCARRIER_H

#include <Windows.h>
#include "baseFunc.h"
using namespace std;

// �������
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
	* @brief DynamicProcess    ��̬�ز��㷨�ӿ�
	* @param phase             ���������ͼ
	* @param mask              �������ģͼ
	* @return
	*/
	void DynamicProcess(cv::Mat phase, cv::Mat mask);

	/**
	* @brief DC_Adaptive_Simulation ��̬�ز�����Ӧģ������
	* @param center                 ��׼ͼ���ĵ�����
	* @param current                ��׼ͼ��ǰ��׼����
	* @param target                 ��׼ͼĿ���׼����
	* @return
	*/
	static cv::Mat DC_Adaptive_Simulation(cv::Point &center,cv::Point &current, cv::Point &target);

private:

	// Ƶ��ʶ���Զ�ѡ���Ƶ��Ϣ��
	cv::Mat FD_Auto_Select(cv::Mat &fft,cv::Mat &fftlog,cv::Point &center,int &radius );
	// Ƶ��ʶ���ֶ�ѡ���Ƶ��Ϣ��
	cv::Mat FD_Manual_Select(cv::Mat &fft, cv::Mat &fftlog,int &x,int &y,int &r);
	// fft�任�����Ƶ�װ���
	void fftshift(cv::Mat &plane0, cv::Mat &plane1);
	// ͼ��߽紦����չ������ֵΪ0��
	cv::Mat image_make_border(cv::Mat &src);
	// ͼ��߽紦����չ������ֵΪnan��
	cv::Mat image_make_border_nan(cv::Mat src);
	// ����Ҷ�任
	cv::Mat FFT(cv::Mat &scr);
	// ������Ҷ�任
	cv::Mat IFFT(cv::Mat &scr);
	// ����Ҷ�任�Ľ��Log�������ڷ�����Ƶ����
	cv::Mat FFT_LOG(cv::Mat &fft);
	// ��Ĥ��������䣬ƽ��˼��
	cv::Mat FillMaskData_Avergae(cv::Mat &phase, cv::Mat fmask);

	/**
     * @brief DC_Simulated_Fringes ��̬�ز�����ģ������
     * @param mask                 �������ģͼ
     * @param number               ��������
     * @param angle                ���ƽǶ�
     * @return
     */
	static cv::Mat DC_Simulated_Fringes(cv::Mat mask, float number, float angle);

};
#endif    //DYNAMICCARRIER_H
