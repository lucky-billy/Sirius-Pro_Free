/*****************************************************************
*
* fileName:  pztcalibrate.h
*
* function:  �������궨�㷨
*
* author:    sjt&ztb
*
* date:      2021.1.26
*
* notes:     ������λֱ��ͼ��������

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/

#pragma once
#ifndef PZTCALIBRATE_H
#define PZTCALIBRATE_H

#include "baseFunc.h"

namespace calc {
	/**
	 * @brief PztHistCalibrate          ����У׼        
	 * @param I0 32FC1                  ��λͼ0
	 * @param I1                        ��λͼ1
	 * @param I3                        ��λͼ3
	 * @param I4                        ��λͼ4
	 * @param roi                       ��λͼ����ѡ����
	 * @param hist CV_32SC1             ��λֱ��ͼ
	 * @param maxloc good if = 90       ���ƽǶ�
	 * @note phase difference = 90 degree
	 */
	void PztHistCalibrate(const cv::Mat& I0,
		const cv::Mat& I1,
		const cv::Mat& I3,
		const cv::Mat& I4,
		const cv::Rect& roi,
		cv::Mat& hist,
		int* maxloc);

	/**
	 * @brief PztHistCalibrate          ����У׼
	 * @param I0 32FC1                  ��λͼ0
	 * @param I1                        ��λͼ1
	 * @param I3                        ��λͼ3
	 * @param I4                        ��λͼ4
	 * @param hist CV_32SC1             ��λֱ��ͼ
	 * @param maxloc good if = 90       ���ƽǶ�
	 * @note phase difference = 90 degree
	 */
	void PztHistCalibrate(const cv::Mat& I0,
		const cv::Mat& I1,
		const cv::Mat& I3,
		const cv::Mat& I4,
		cv::Mat& hist,
		int* maxloc);

	/**
	 * @brief PztMser                   �������ƽǶ�ֱ��ͼ�ı�׼���׼��Խ��˵������Խ���У�
     * @param hist                      �Ƕ�ֱ��ͼ
	 * @param I0                        ͼ��0������maskʹ��
	 * @param roi                       �û�ѡ������򣬼���maskʹ��
	 * @return                          ��׼��
	 * @note                            ������ֻ��90�㲻Ϊ0�������Ϊ0
	 */
	float PztHistStd(cv::Mat& hist,
		const cv::Mat &I0,
		const cv::Rect &roi);

	/**
    * @brief PztMser                   �������ƽǶ�ֱ��ͼ�ı�׼���׼��Խ��˵������Խ���У�
    * @param hist                      �Ƕ�ֱ��ͼ
    * @param rows                      ͼ���rows
    * @param cols                      ͼ��cols
    * @return                          ��׼��
    * @note  ������ֻ��90�㲻Ϊ0�������Ϊ0
    */
	float PztHistStd(cv::Mat& hist,
		int rows,
		int cols);

	/**
	 * @brief PztMser                   ������λͼ4����λͼ0���֮��ľ�����
	 * @param I0 32FC1                  ��λͼ0
	 * @param I4                        ��λͼ4
	 * @param roi                       ��λͼ����ѡ����
	 * @return                          ������
	 * @note  ����������ͼ��ȫһ����    delta = 0
	 */
	float PztMser(const cv::Mat& I0,
		const cv::Mat& I4,
		const cv::Rect& roi);
} // namespace calc

#endif // PZTCALIBRATE_H