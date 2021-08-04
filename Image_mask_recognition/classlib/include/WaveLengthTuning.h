/*****************************************************************
*
* fileName:  WaveLengthTuning.h
*
* function:  波长调谐技术
*
* author:    ztb（Steven）
*
* date:      2021.3.29
*
* notes:     波长调谐技术

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/
#pragma once
#ifndef WAVELENGTHTUNING_H
#define WAVELENGTHTUNING_H

#include "baseFunc.h"
#include <utility>

// 错误类型
enum WT_ERROR_TYPE {
	WT_ERROR_TYPE_NOT_ERROR = 0,
	WT_ERROR_TYPE_NO_ENOUGH_DATA
};

// 输入参数
struct WT_CONFIG_PARAMS {
	float Diff_Caity_Ln = 1.202f;
};

class WaveLengthTuning
{
public:
	WT_ERROR_TYPE errorType = WT_ERROR_TYPE_NOT_ERROR;           // 错误类型
	WT_CONFIG_PARAMS wt_config;
public:
	WaveLengthTuning(WT_CONFIG_PARAMS config):wt_config(config){};
	~WaveLengthTuning(){};

	/*--------------------- Basic Function -------------------------*/
	/**
	 * @brief Curve_Fitting                   拟合关系曲线：V=aH^(-b),a、b均为正
	 * @param hv                              输入的数据
	 */
	void Curve_Fitting(vector<pair<float, float>> hv);

	float GetVolt(float Caity_Ln);

	float GetA();

	float GetB();

private:
	float parm_a{ 0 };
	float parm_b{ 0 };

};
#endif    //WAVELENGTHTUNING_H