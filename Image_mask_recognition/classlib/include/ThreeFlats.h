/*****************************************************************
*
* fileName:  ThreeFkats.h
*
* function:  三平面互检
*
* author:    ztb（Steven）
*
* date:      2021.3.2
*
* notes:     三平面互检

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/
#pragma once
#ifndef THREEFLATS_H
#define THREEFLATS_H

#include "windows.h"
#include "algorithmMain.h"

//三平面互检方法
enum THREEFLATS_METHOD {
	THREEFLATS_METHOD_ZERNIKE = 0,
	THREEFLATS_METHOD_N_AVERAGE = 1,
	THREEFLATS_METHOD_N_AVERAGE_PRO = 2,                 // 可实现带高频信息（残差点）的面形还原
};

// 奇偶性
enum OddOrEven {
	Odd=0,
	Even=1,
};

// 输入参数
struct TF_CONFIG_PARAMS {
	bool isNoTilt = true;
	int rotate_number = 0;
	float angle = 0.0f;
	THREEFLATS_METHOD threeflats_method = THREEFLATS_METHOD_ZERNIKE;
};

// 输入数据
struct TF_INPUT_DATAS {
	// Zernike法中容器顺序为：（前为待测物，后为参考镜）KL（D）、ML（E）、MK（F）、MrotL（G）
	// N位平均法中容器顺序为：（前为待测物，后为参考镜）KL 、KrotL(n-1次旋转，假设旋转8次，则KL+KrotL=8，旋转角度为45°)、KM、LM
	// 改进N位平均法中容器顺序为：（前为待测物，后为参考镜）KL 、KrotL(n-1次旋转，假设旋转8次，则KL+KrotL=8，旋转角度为45°)、KM、ML、K--M（K--意为将K面顺时针旋转180°）
	std::vector<cv::Mat> VzernikeCoef_37;                   // 待测面集合的对应Zernike系数
	std::vector<cv::Mat> VfittingSurface;                   // 待测面集合的面形数据
	// 算法实例中的roiMask
	cv::Mat mask;                                           // 用户设置的掩膜
};

// 结果
struct TF_RESULTS {
	// 待测物拟合结果
	cv::Mat K;
	cv::Mat L;
	cv::Mat M;                                              
};

class ThreeFlats
{
public:
	TF_CONFIG_PARAMS tf_config;
	TF_RESULTS tf_result;
	CONFIG_PARAMS tf_userSets;
	ERROR_TYPE errorType = ERROR_TYPE_NOT_ERROR;           // 错误类型
	// 待测物zernike拟合系数
	cv::Mat K_coef_37;
	cv::Mat L_coef_37;
	cv::Mat M_coef_37;

public:
	ThreeFlats(TF_CONFIG_PARAMS config, CONFIG_PARAMS userSets):tf_config(config),tf_userSets(userSets) {};
	~ThreeFlats() {};

	/**
     * @brief ThreeFlatsProcess                  三平面互检功能接口
     * @param inputs                             输入的数据                   
     */
	void ThreeFlatsProcess(TF_INPUT_DATAS inputs);

	/**
     * @brief detecThreeFlat                    三平面检测（a, b, c）——传统法(只能得到三个平面XY的切片)
     * @param xSlice                            拟合曲面横坐标方向的切片(a+b, a+c, b+c, br/cr)@待测面+参考面，br即令b旋转180度，cr=c（参考面不变）
     * @param ySlice                            拟合曲面纵坐标方向的切片(a+b, a+c, b+c, br/cr)@待测面+参考面，br即令b旋转180度，cr=c（参考面不变）
     * @param pv                                三个平面的x,y方向上PV（顺序为：a,b,c，第一行x,第二行y）
     * @param rms                               三个平面的x,y方向上rms（顺序为：a,b,c，第一行x,第二行y）
     * @return                                  三个平面的计算结果(先x(abc)，后y(abc))
     */
	static std::vector<cv::Mat> detecThreeFlat(std::vector<cv::Mat>xSlice,std::vector<cv::Mat>ySlice, float pv[2][3], float rms[2][3]); 

private:
	Zernike *tf_zernike;
	// 建立有效区域的极坐标
	cv::Mat mag_square;
	cv::Mat ang_square;
	// zernike计算时的点集合
	std::vector<cv::Point> maskPoints;

private:
	/*--------------------- Basic Function -------------------------*/
	/**
	 * @brief ThreeFlats_Init                   异常分析模块
	 * @param inputs                            输入的数据   
	 */
	void ThreeFlats_Exception_Analysis(const TF_INPUT_DATAS &inputs);

	/**
	 * @brief ThreeFlats_Init                   初始化模块
	 * @param fittingsurface                    输入的面形图
	 * @param mask                              输入的掩膜
	 */
	void ThreeFlats_Init(const cv::Mat &fittingsurface,const cv::Mat &mask);

	/**
     * @brief ThreeFlats_GetResult              获取结果
     * @param inputs                            输入的数据
     */
	void ThreeFlats_GetResult(const TF_INPUT_DATAS &inputs);

	/*------------------- 三平面互检-Zernike法 -----------------------*/
	/**
	 * @brief ThreeFlats_Zernike                三平面互检法——Zernike法
	 * @param inputs                            输入的数据
	 */
	void ThreeFlats_Zernike(const TF_INPUT_DATAS &inputs);

	/**
	 * @brief ThreeFlats_Zernike_FirstSort      Zernike法第一类系数计算，旋转对称项
	 * @param inputs                            输入的数据
	 */
	void ThreeFlats_Zernike_FirstSort(const TF_INPUT_DATAS &inputs);

	/**
	 * @brief ThreeFlats_Zernike_SecondSort     Zernike法第二类系数计算，X为偶函数且非旋转对称项
	 * @param inputs                            输入的数据
	 */
	void ThreeFlats_Zernike_SecondSort(const TF_INPUT_DATAS &inputs);

	/**
	 * @brief ThreeFlats_Zernike_ThirdSort      Zernike法第三类系数计算，X为奇函数且Y为偶函数
	 * @param inputs                            输入的数据
	 */
	void ThreeFlats_Zernike_ThirtSort(const TF_INPUT_DATAS &inputs);

	/**
	 * @brief ThreeFlats_Zernike_FirstSort      Zernike法第一类系数计算，X和Y均为奇函数
	 * @param inputs                            输入的数据
	 */
	void ThreeFlats_Zernike_FourthSort(const TF_INPUT_DATAS &inputs);

	/*------------------- 三平面互检-N位平均法 -----------------------*/
	/**
	 * @brief ThreeFlats_N_Average              三平面互检法——N位平均法
	 * @param inputs                            输入的数据
	 */
	void ThreeFlats_N_Average(const TF_INPUT_DATAS &inputs);

	/**
	 * @brief ThreeFlats_N_Average_Pretreatment N位平均预处理
	 * @param inputs                            输入的数据
	 * @return                                  N位平均结果
	 */
	vector<cv::Mat> ThreeFlats_N_Average_Pretreatment(const TF_INPUT_DATAS &inputs);

	/**
     * @brief ZernikeOdd                        获取Zernike奇数项系数
     * @param zernikecoef                       Zernike系数矩阵
     * @return                                  Zernike奇数系数矩阵
     */
	cv::Mat ZernikeOdd(const cv::Mat& zernikecoef);

	/**
	 * @brief ZernikeEven                       获取Zernike偶数项系数
	 * @param zernikecoef                       Zernike系数矩阵
	 * @return                                  Zernike偶数系数矩阵
	 */
	cv::Mat ZernikeEven(const cv::Mat& zernikecoef);

	/*------------------- 三平面互检-改进的N位平均法 -----------------------*/
	/**
	 * @brief ThreeFlats_N_Average_Pro          三平面互检法——改进的N位平均法
	 * @param inputs                            输入的数据
	 */
	void ThreeFlats_N_Average_Pro(const TF_INPUT_DATAS &inputs);

	/**
	 * @brief Image_Flip                        图像翻转，获得(x,y)、(-x,y)、(x,-y)、(-x,-y)
	 * @param phase                             翻转的图像
	 * @return                                  翻转后的结果，共4组数据
	 */
	vector<cv::Mat> Image_Flip(const cv::Mat &phase);

	/**
	 * @brief ThreeFlats_N_Average_Pro_Calculation  改进N位平均法计算模块
	 * @param W_Set                                 存放测试波面及其翻转图像数据的集合的容器
	 * @return                                      三平面波面数据集合（共3个Mat）
	 */
	vector<cv::Mat> ThreeFlats_N_Average_Pro_Calculation(const vector<vector<cv::Mat>> W_Set);

};
#endif // THREEFLATS_H
