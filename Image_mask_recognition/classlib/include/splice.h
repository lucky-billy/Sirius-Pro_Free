/*****************************************************************
*
* fileName:  splice.h
*
* function:  �ӿ׾�ƴ��
*
* author:    sjt&ztb
*
* date:      2021.1.26
*
* notes:     �ӿ׾�ƴ��

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/

#pragma once
#ifndef SPLICE_H
#define SPLICE_H

#include "baseFunc.h"

class Splice {
public:
	float px_1mm;
	std::vector<cv::Point> delta;
	std::vector<cv::Mat> phase;
	cv::Mat splicedPhase;

public:
	Splice(float input_px_1mm): px_1mm(input_px_1mm){}

	/**
	 * @brief SpliceProcess                      �ӿ׾�ƴ�ӹ��ܽӿ�
	 * @param files                              asc�ļ�
	 */
	void SpliceProcess(const std::vector<std::string> &files);
	
private:
	/**
	 * @brief toDelta                            ����ƫ��������0��ʼ
	 * @param delta                              ƫ����
	 */
	void toDelta();
	
	/**
	 * @brief getTopLeft                         �õ�ͳһ����ϵ�е����Ͻǵĵ������
	 * @param delta                              ƫ����
	 * @param topleft                            ���Ͻǵĵ������
	 */
	void getTopLeft(std::vector<cv::Point>&topleft);
	
	/**
	 * @brief getSize                            ��ȡƴ�Ӻ��ͼ���С
	 * @param size0                              ��������ݵĴ�С
	 * @param size                               ���������ƴ�Ӻ��ͼ���С
	 */
	void getSize(const std::vector<cv::Point>& topleft,
		cv::Size size0, cv::Size& size);
	
	/**
	 * @brief extendSize                         �����������չ��
	 * @param phase                              ���������
	 * @param topleft                            ���������������չ��ͼ���е����Ͻǵ�����
	 * @param size                               չ���Ĵ�С
	 * @param extendphase                        չ��������
	 */
	void extendSize(const std::vector<cv::Point>& topleft,
		cv::Size size,
		std::vector<cv::Mat>& extendphase);
	
	/**
	 * @brief splice22                           ͼ��ƴ��
	 * @param extendphase1                       չ��ͼ1��Ҳ�������ƴ�ӽ��
	 * @param extendphase2                       չ��ͼ2
	 */
	void splice22(cv::Mat& extendphase1,
		const cv::Mat& extendphase2);
	
	/**
	 * @brief splice22_2                         ͼ��ƴ��,�ٶȸ���
	 * @param extendphase1                       չ��ͼ1��Ҳ�������ƴ�ӽ��
	 * @param extendphase2                       չ��ͼ2
	 */
	void splice22_2(cv::Mat& extendphase1,
		const cv::Mat& extendphase2);

	/*
	 * @brief getNonZero                         ���Ҳ�Ϊ0�ĵ������
	 * @param mask                               ���ҵ�mask
	 * @param Coordinate                         �洢�������
	 */
	int getNonZero(cv::Mat mask,
		int **Coordinate);
	
	/**
	 * @brief spliceSerial                       ƴ�Ӷ��ͼ��
	 * @param phase                              ���������
	 * @param delta                              ƫ����
	 * @param splicedphase                       ƴ��ͼ��
	 */
	void spliceSerial();
	
	/**
	 * @brief readSerialFiles                    ��ȡ���asc�ļ�
	 * @param files                              ����ȡ���ļ�
	 * @param px_1mm                             ÿmm�����ظ������ֱ���
	 * @param phase                              ���������
	 * @param delta                              ƫ����
	 */
	void readSerialFiles(const std::vector<std::string>& files);

};

#endif