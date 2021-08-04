/*****************************************************************
*
* fileName:  AngleCalculation.h
*
* function:  角度计算技术
*
* author:    ztb（Steven）
*
* date:      2021.7.13
*
* notes:     角度计算技术

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

// 角度计算方法
enum AC_METHOD {
	AC_METHOD_THROUGH = 0,                    // 透过计算
	AC_METHOD_REFLECTION = 1,                 // 反射计算
};

// 输入参数
struct ANGLE_PARAMS
{
	float aperture1 = 0.0f;                   // 待测面的口径
	float aperture2 = 0.0f;                   // 参考面的口径
	float refractiveIndex = 1.5f;             // 折射率
	float testWaveLength = 632.8f;            // 波长

};

// 输入数据
struct ANGLE_DATAS {
	cv::Mat zernikeCoef4_1;                   // 待测面的zernike4项拟合系数
	cv::Mat zernikeCoef4_2;                   // 参考面的zernike4项拟合系数
	cv::Mat unwrapphase_1;                    // 待测面的解包裹图
	cv::Mat unwrapphase_2;                    // 参考面的解包裹图
	cv::Mat fittingSurface1;                  // 待测面的面形图
};

class AngleCalculation
{
public:
	ERROR_TYPE errorType = ERROR_TYPE_NOT_ERROR;           // 错误类型
	float DegreeX = 0.0f;                                  // X方向角度
	float DegreeY = 0.0f;                                  // Y方向角度
	float Degree = 0.0f;                                   // 角度
	float TestSurfacePV = 0.0f;                            // 测试面原本的PV值
	float includedAngle = 0.0f;                            // 相对倾斜面的倾斜方向，以向下为正向0°，向右为正向90°
	cv::Mat RefResult;                                     // 参考面结果图（校正为基准平面）
	cv::Mat TestResult;                                    // 测试面结果图（相对于参考面的倾斜面）
	ANGLE_PARAMS angleParams;                              // 角度计算参数
	AC_METHOD ac_method;                                   // 角度计算方法

public:
	AngleCalculation() {};
	AngleCalculation(ANGLE_PARAMS params) :angleParams(params) {};
	~AngleCalculation() {};

	/**
	 * @brief AngleCalcProcess  角度计算算法接口
	 * @param datas             输入的数据
	 * @return
	 */
	void AngleCalcProcess(ANGLE_DATAS datas);


	/**
	 * @brief clearDat          数据清空
  	 * @return
	 */
	void clearData();

private:
	/**
	 * @brief AngleCalc_Exception_Analysis     异常分析模块
	 * @param inputs                           输入的数据
	 */
	void AngleCalc_Exception_Analysis(const ANGLE_DATAS &inputs);

	/**
	 * @brief CalcParallelDegree                计算角度
	 * @param zernikeCoef4_1                    待测界面，mask1的4阶zernike系数（x:z2, y:z3）
	 * @param zernikeCoef4_2                    参考界面，mask2的4阶zernike系数（x:z2, y:z3）
	 * @param aperture1                         mask1的口径(mm)
	 * @param aperture2                         mask2的口径(mm)
	 * @param refractiveIndex                   折射率
	 * @param testWaveLength                    测试波长（nm）
	 * @return                                  平行度（单位：秒）
 	 */
	float CalcParallelDegree(cv::Mat zernikeCoef4_1, cv::Mat zernikeCoef4_2, float aperture1, float aperture2, float refractiveIndex, float testWaveLength);

	/**
	 * @brief CalcTheta                         计算x or y方向的角度
	 * @param z1                                待测界面，mask1的4阶zernike系数的倾斜项（x:z2, y:z3）
	 * @param z2                                参考界面，mask2的4阶zernike系数的倾斜项（x:z2, y:z3）
	 * @param aperture1                         mask1的口径(mm)
	 * @param aperture2                         mask2的口径(mm)
	 * @param refractiveIndex                   折射率
	 * @param testWaveLength                    测试波长（nm）
	 * @return                                  x or y方向的角度(秒)
	 */
	float CalcTheta(float z1, float z2, float aperture1,float aperture2, float refractiveIndex, float testWaveLength);

	/**
	 * @brief getMagAndAng                      获得zernike极坐标系下半径图和角度图
	 * @param phase,                            输入图像
	 * @param mask                              输入掩膜
	 * @param mag                               计算后的半径图
	 * @param ang                               计算后的角度图
	 * @return                                  
	 */
	void getMagAndAng(cv::Mat phase, cv::Mat mask, cv::Mat &mag, cv::Mat &ang);

	/**
	 * @brief removeTilt                        消除倾斜
	 * @param phase,                            输入图像
	 * @param mask                              输入掩膜
	 * @param Zernike4                          输入图像的zernike4项拟合系数
	 * @param mag                               计算后的半径图
	 * @param ang                               计算后的角度图
	 * @return                                  去倾斜后的结果
	 */
	cv::Mat removeTilt(cv::Mat phase, cv::Mat mask, cv::Mat Zernike4,cv::Mat mag, cv::Mat ang);

	/**
	 * @brief calcRelativeSurface               计算相对倾斜面
	 * @param phase,                            输入图像
	 * @param mask                              输入掩膜
	 * @param RefZernike4                       参考面的zernike4项拟合系数
	 * @param mag                               计算后的半径图
	 * @param ang                               计算后的角度图
	 * @return                                  相对倾斜面
	 */
	cv::Mat calcRelativeSurface(cv::Mat phase, cv::Mat mask, cv::Mat RefZernike4, cv::Mat mag, cv::Mat ang);

	/**
	 * @brief FitSlope                          拟合斜面
	 * @param phase,                            输入图像
	 * @param mask                              输入掩膜
	 * @param a                                 拟合平面系数a,ax+by+c=0
	 * @param b                                 拟合平面系数b
	 * @param c                                 拟合平面系数c
	 * @return                                  减去拟合斜面的面形
	 */
	cv::Mat FitSlope(cv::Mat &phase, cv::Mat &mask, float &a, float &b, float &c);

};
#endif
