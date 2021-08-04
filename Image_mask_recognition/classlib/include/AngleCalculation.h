/*****************************************************************
*
* fileName:  AngleCalculation.h
*
* function:  �Ƕȼ��㼼��
*
* author:    ztb��Steven��
*
* date:      2021.7.13
*
* notes:     �Ƕȼ��㼼��

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/
#pragma once
#ifndef ANGLECALCULATION_H
#define ANGLECALCULATION_H

#include <Windows.h>
#include "baseFunc.h"
#include "edgedetector.h"
#include "zernike.h"

using namespace std;

// �Ƕȼ��㷽��
enum AC_METHOD {
	AC_METHOD_THROUGH = 0,                    // ͸������
	AC_METHOD_REFLECTION = 1,                 // �������
};

// �������
struct ANGLE_PARAMS
{
	float aperture1 = 0.0f;                   // ������Ŀھ�
	float aperture2 = 0.0f;                   // �ο���Ŀھ�
	float refractiveIndex = 1.5f;             // ������
	float testWaveLength = 632.8f;            // ����

};

// ��������
struct ANGLE_DATAS {
	cv::Mat zernikeCoef4_1;                   // �������zernike4�����ϵ��
	cv::Mat zernikeCoef4_2;                   // �ο����zernike4�����ϵ��
	cv::Mat unwrapphase_1;                    // ������Ľ����ͼ
	cv::Mat unwrapphase_2;                    // �ο���Ľ����ͼ
	cv::Mat fittingSurface1;                  // �����������ͼ
};

class AngleCalculation
{
public:
	ERROR_TYPE errorType = ERROR_TYPE_NOT_ERROR;           // ��������
	float DegreeX = 0.0f;                                  // X����Ƕ�
	float DegreeY = 0.0f;                                  // Y����Ƕ�
	float Degree = 0.0f;                                   // �Ƕ�
	float TestSurfacePV = 0.0f;                            // ������ԭ����PVֵ
	float includedAngle = 0.0f;                            // �����б�����б����������Ϊ����0�㣬����Ϊ����90��
	cv::Mat RefResult;                                     // �ο�����ͼ��У��Ϊ��׼ƽ�棩
	cv::Mat TestResult;                                    // ��������ͼ������ڲο������б�棩
	ANGLE_PARAMS angleParams;                              // �Ƕȼ������
	AC_METHOD ac_method;                                   // �Ƕȼ��㷽��

public:
	AngleCalculation() {};
	AngleCalculation(ANGLE_PARAMS params) :angleParams(params) {};
	~AngleCalculation() {};

	/**
	 * @brief AngleCalcProcess  �Ƕȼ����㷨�ӿ�
	 * @param datas             ���������
	 * @return
	 */
	void AngleCalcProcess(ANGLE_DATAS datas);


	/**
	 * @brief clearDat          �������
  	 * @return
	 */
	void clearData();

private:
	/**
	 * @brief AngleCalc_Exception_Analysis     �쳣����ģ��
	 * @param inputs                           ���������
	 */
	void AngleCalc_Exception_Analysis(const ANGLE_DATAS &inputs);

	/**
	 * @brief CalcParallelDegree                ����Ƕ�
	 * @param zernikeCoef4_1                    ������棬mask1��4��zernikeϵ����x:z2, y:z3��
	 * @param zernikeCoef4_2                    �ο����棬mask2��4��zernikeϵ����x:z2, y:z3��
	 * @param aperture1                         mask1�Ŀھ�(mm)
	 * @param aperture2                         mask2�Ŀھ�(mm)
	 * @param refractiveIndex                   ������
	 * @param testWaveLength                    ���Բ�����nm��
	 * @return                                  ƽ�жȣ���λ���룩
 	 */
	float CalcParallelDegree(cv::Mat zernikeCoef4_1, cv::Mat zernikeCoef4_2, float aperture1, float aperture2, float refractiveIndex, float testWaveLength);

	/**
	 * @brief CalcTheta                         ����x or y����ĽǶ�
	 * @param z1                                ������棬mask1��4��zernikeϵ������б�x:z2, y:z3��
	 * @param z2                                �ο����棬mask2��4��zernikeϵ������б�x:z2, y:z3��
	 * @param aperture1                         mask1�Ŀھ�(mm)
	 * @param aperture2                         mask2�Ŀھ�(mm)
	 * @param refractiveIndex                   ������
	 * @param testWaveLength                    ���Բ�����nm��
	 * @return                                  x or y����ĽǶ�(��)
	 */
	float CalcTheta(float z1, float z2, float aperture1,float aperture2, float refractiveIndex, float testWaveLength);

	/**
	 * @brief getMagAndAng                      ���zernike������ϵ�°뾶ͼ�ͽǶ�ͼ
	 * @param phase,                            ����ͼ��
	 * @param mask                              ������Ĥ
	 * @param mag                               �����İ뾶ͼ
	 * @param ang                               �����ĽǶ�ͼ
	 * @return                                  
	 */
	void getMagAndAng(cv::Mat phase, cv::Mat mask, cv::Mat &mag, cv::Mat &ang);

	/**
	 * @brief removeTilt                        ������б
	 * @param phase,                            ����ͼ��
	 * @param mask                              ������Ĥ
	 * @param Zernike4                          ����ͼ���zernike4�����ϵ��
	 * @param mag                               �����İ뾶ͼ
	 * @param ang                               �����ĽǶ�ͼ
	 * @return                                  ȥ��б��Ľ��
	 */
	cv::Mat removeTilt(cv::Mat phase, cv::Mat mask, cv::Mat Zernike4,cv::Mat mag, cv::Mat ang);

	/**
	 * @brief calcRelativeSurface               ���������б��
	 * @param phase,                            ����ͼ��
	 * @param mask                              ������Ĥ
	 * @param RefZernike4                       �ο����zernike4�����ϵ��
	 * @param mag                               �����İ뾶ͼ
	 * @param ang                               �����ĽǶ�ͼ
	 * @return                                  �����б��
	 */
	cv::Mat calcRelativeSurface(cv::Mat phase, cv::Mat mask, cv::Mat RefZernike4, cv::Mat mag, cv::Mat ang);

	/**
	 * @brief FitSlope                          ���б��
	 * @param phase,                            ����ͼ��
	 * @param mask                              ������Ĥ
	 * @param a                                 ���ƽ��ϵ��a,ax+by+c=0
	 * @param b                                 ���ƽ��ϵ��b
	 * @param c                                 ���ƽ��ϵ��c
	 * @return                                  ��ȥ���б�������
	 */
	cv::Mat FitSlope(cv::Mat &phase, cv::Mat &mask, float &a, float &b, float &c);

};
#endif
