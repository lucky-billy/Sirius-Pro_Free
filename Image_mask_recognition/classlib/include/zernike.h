/*****************************************************************
*
* fileName:  zernike.h
*
* function:  计算zernike系数及拟合曲面
*
* author:    sjt&ztb（Steven）
*
* date:      2021.1.26
*
* notes:     计算zernike系数及拟合曲面

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/

#pragma once
#ifndef ZERNIKE_H
#define ZERNIKE_H

#include "baseFunc.h"

enum ZERNIKE_METHOD {
	ZERNIKE_METHOD_CIRCLE = 0,
	ZERNIKE_METHOD_ORTHO
};

//对角阵的对角线元素值
const std::vector<float> kOrtho = {
	SQRT_1, // 0

	SQRT_4, // 1
	SQRT_4, // 2
	SQRT_3, // 3

	SQRT_6, // 4
	SQRT_6, // 5
	SQRT_8, // 6
	SQRT_8, // 7
	SQRT_5, // 8

	SQRT_8, // 9
	SQRT_8, // 10
	SQRT_10, // 11
	SQRT_10, // 12
	SQRT_12, // 13
	SQRT_12, // 14
	SQRT_7, // 15

	SQRT_10, // 16
	SQRT_10, // 17
	SQRT_12, // 18
	SQRT_12, // 19
	SQRT_14, // 20
	SQRT_14, // 21
	SQRT_16, // 22
	SQRT_16, // 23
	SQRT_9, // 24

	SQRT_12, // 25
	SQRT_12, // 26
	SQRT_14, // 27
	SQRT_14, // 28
	SQRT_16, // 29
	SQRT_16, // 30
	SQRT_18, // 31
	SQRT_18, // 32
	SQRT_20, // 33
	SQRT_20, // 34
	SQRT_11, // 35

	SQRT_13 // 36
};

// 移除泽尼克相关项的标志，true为移除
struct REMOVE_ZERNIKE_FLAGS {
	bool isPolySelect = false;                   // 判断是否用36项多项式作清除项标志

	bool positionFlag = true;                    // 去掉偏移量的标志
	bool tiltFlag = true;                        // 去掉倾斜量的标志
	bool powerFlag = false;                      // 去掉power的标志
	bool astFlag = false;                        // 去掉ast的标志
	bool comaFlag = false;                       // 去掉coma的标志
	bool sphericalFlag = false;                  // s去掉pherical的标志

	vector<bool> ZernikePolyFlag = {
	false, //Zernike0
	false, //Zernike1
	false, //Zernike2
	false, //Zernike3
	false, //Zernike4
	false, //Zernike5
	false, //Zernike6
	false, //Zernike7
	false, //Zernike8
	false, //Zernike9
	false, //Zernike10
	false, //Zernike11
	false, //Zernike12
	false, //Zernike13
	false, //Zernike14
	false, //Zernike15
	false, //Zernike16
	false, //Zernike17
	false, //Zernike18
	false, //Zernike19
	false, //Zernike20
	false, //Zernike21
	false, //Zernike22
	false, //Zernike23
	false, //Zernike24
	false, //Zernike25
	false, //Zernike26
	false, //Zernike27
	false, //Zernike28
	false, //Zernike29
	false, //Zernike30
	false, //Zernike31
	false, //Zernike32
	false, //Zernike33
	false, //Zernike34
	false, //Zernike35
	false  //Zernike36
	};
};

class Zernike {
public:
	cv::Mat coef_display_4;                      // 最终显示的zernike 4项系数结果
	cv::Mat coef_display_9;                      // 最终显示的zernike 9项系数结果 （term >= 9）
	cv::Mat coef_display_16;                     // 最终显示的zernike 16项系数结果 （term >= 16）
	cv::Mat coef_display_25;                     // 最终显示的zernike 25项系数结果 （term >= 25）
	cv::Mat coef_display_36;                     // 最终显示的zernike 36项系数结果 （term >= 36）
	cv::Mat coef_display_37;                     // 最终显示的zernike 37项系数结果 （term >= 37）

	cv::Mat coefficient_4;                       // 4项zernike系数(zernike power, zernike tilt, concavity, Rms(Power), seidel tilt, seidel tilt clock, Iso Sag, Parallel Theta)
	cv::Mat coefficient_9;                       // 9项zernike系数 （term >= 9）
	cv::Mat coefficient_16;                      // 16项zernike系数 （term >= 16）
	cv::Mat coefficient_25;                      // 25项zernike系数 （term >= 25）
	cv::Mat coefficient_36;                      // 36项zernike系数 （term >= 36）
	cv::Mat coefficient_37;                      // 37项zernike系数 （term >= 37）

	cv::Mat unwrapphase;                         // 相位图
	cv::Mat fitting_all;                         // zernike系数拟合结果(ttv, fringes, zernike residual)
	cv::Mat fitting_rem_zer;                     // 去除用户设置的移除项之后拟合的结果，后期计算pv，rms等参数时，使用的拟合结果
	cv::Mat fitting_r0_r1_r2;                    // 移除前3项的拟合结果，计算ISO的Rmst时使用（前3项是指4项拟合系数的前3项结果）
	cv::Mat fitting_r0_r1_r2_r3;                 // 移除前4项的拟合结果，计算ISO的Irr，Rmsa, Rmsi时使用（前3项是指4项拟合系数的前4项结果）
	cv::Mat fitting_z8_z15_z24_z35_z36;          // zernike系数中表示球差的项数拟合结果，计算ISO的Rsi,Rmsa时使用
	cv::Mat zernike_residual;                    // 残差

	int term;                                    // zernike项数
	cv::Mat zerConMat;                           // 计算zernike系数时，用到的转换矩阵（37*37）
	cv::Mat zerConMatInv;                        // zernike系数进行曲面拟合时，用到的转换矩阵的逆矩阵（37*37）
	cv::Mat zernikeValue;                        // 37项zernike多项式的值（37*1）
	cv::Mat OrthoDiag;                           // 正交系数矩阵

	cv::Mat zernikeConvertMatrix;                // 正交多项式转换矩阵
	cv::Mat identityMatrix;                      // 单位阵
	cv::Mat zernikeConvertMatrixInv;             // 正交多项式转换矩阵的逆矩阵
	cv::Mat identityMatrixInv;                   // 单位阵的逆矩阵

	bool removeResidualFlag = false;             // 移除残差的标志
	bool calcFitR012Flag = false;                // 计算fitting_r0_r1_r2的标志
	bool calcFitR0123Flag = false;               // 计算fitting_r0_r1_r2_r3的标志
	bool calcFitZ8toZ36Flag = false;             // 计算fitting_z8_z15_z24_z35_z36的标志
	bool convertFinish = false;                  // 是否完成转换矩阵
	ERROR_TYPE errorType = ERROR_TYPE_NOT_ERROR;           // 错误类型
	ZERNIKE_METHOD zernikeMethod = ZERNIKE_METHOD_CIRCLE;// 计算zernike系数时，选择转换矩阵的方法
	REMOVE_ZERNIKE_FLAGS removeZernikeFlags;     // 移除zernike多项式的标志

public:
	Zernike(int inputTerm = 37);

	/**
	 * @brief ZernikeProcess                     zernike处理函数
	 * @param ang                                相位
	 * @param mag                                幅度
	 * @param mask                               mask
	 * @param phase                              解包裹相位
	 * @return
	 */
	void ZernikeProcess(const cv::Mat& ang,
		const cv::Mat& mag,
		const cv::Mat& mask,
		const cv::Mat& phase);

	void zernikeinit();

	/**
	 * @brief ZernikeConvert 32FC1                计算zernike转换矩阵
	 * @param _ang                                相位kAng129 or kAng259
	 * @param _mag                                幅值kMag129 or kMag259
	 * @param _mask square matrix                 n*n的mask
	 * @param terms                               zernike阶数
	 * @return                                    转换矩阵，计算圆的zernike拟合系数时，该矩阵为单位阵
	 */
	cv::Mat ZernikeConvert(cv::Size size, cv::Mat mask_roi, cv::Rect roi_in_square);

	/**
	* @brief ZernikeA 32FC1                      得到37项zernike多项式
	* @param ang                                 相位
	* @param mag                                 幅值
	* @param sampling                            抽样点坐标
	* @return                                    terms项zernike多项式
	*/
	cv::Mat ZernikeA(const cv::Mat& ang,
		const cv::Mat& mag,
		const std::vector<cv::Point>& sampling,
		int terms);

	// 得到37项zernike多项式(验证速度较快)
	cv::Mat ZernikeA2(const cv::Mat &ang,
		const cv::Mat &mag,
		const std::vector<cv::Point> &sampling,
		int terms);

	// 得到37项zernike多项式(验证速度最快)
	cv::Mat ZernikeA1(const cv::Mat &ang,
		const cv::Mat &mag,
		const std::vector<cv::Point> &sampling,
		int terms);

	/**
	 * @brief ZernikeFitting                  利用zernike多项式拟合曲面
	 * @param coefForFitting                  zernike系数
	 * @param mask                            mask
	 * @param maskPoints                      mask中不为0的点集
	 * @return                                拟合曲面
	 */
	cv::Mat ZernikeFitting(const cv::Mat &coefForFitting,
		const cv::Mat &mask,
		const std::vector<cv::Point> &maskPoints);

	/**
	 * @brief ItemMask                        设置各种去除项
	 * @param remove_piston                   piston
	 * @param remove_tilt                     tilt
	 * @param remove_power                    power
	 * @param remove_ast                      ast
	 * @param remove_coma                     coma
	 * @param remove_spherical                spherical
	 * @param terms                           阶数
	 * @return
	 */
	inline cv::Mat ItemMask(bool remove_piston = true,
		bool remove_tilt = true,
		bool remove_power = false,
		bool remove_ast = false,
		bool remove_coma = false,
		bool remove_spherical = false,
		int terms = 36) {
		cv::Mat m(terms, 1, CV_8UC1, cv::Scalar(255));
		if (remove_piston) { m.at<uchar>(0, 0) = 0; }
		if (remove_tilt) { m.at<uchar>(1, 0) = 0; m.at<uchar>(2, 0) = 0; }
		if (remove_power) { m.at<uchar>(3, 0) = 0; }
		if (remove_ast) { m.at<uchar>(4, 0) = 0; m.at<uchar>(5, 0) = 0; }
		if (remove_coma) { m.at<uchar>(6, 0) = 0; m.at<uchar>(7, 0) = 0; }
		if (remove_spherical) { m.at<uchar>(8, 0) = 0; }
		return m;
	}


	void calcConvert(cv::Size size, cv::Mat mask_roi, cv::Rect roi_in_square);

private:
	/**
	* @brief Calculates lower triangular matrix S, where A is a symmetrical matrix A=S*S'
	* @param A                                   正定的实对称阵
	* @param S                                   得到的下三角阵
	* @note  Cholesky分解
	*/
	void Cholf(const cv::Mat& A, cv::Mat& S);

	/**
	* @brief ZernikeB                            得到sampling处的数据,数据抽样
	* @param wave                                待抽样的数据，一般是解包裹数据
	* @param sampling                            抽样点处的坐标
	* @return                                    抽样后的数据
	*/
	cv::Mat ZernikeB(const cv::Mat& wave,
		const std::vector<cv::Point>& sampling);

	/**
	* @brief ZernikeCoefficient              求解正规方程A’A * dst = A'B中的dst,得到zernike系数
	* @param zernikeA                        zernike多项式
	* @param zernikeB                        抽样后的数据
	* @return                                zernike系数
	*/
	cv::Mat ZernikeCoefficient(const cv::Mat& zernikeA,
		const cv::Mat& zernikeB);

	/**
	 * @brief calcZernikeCoefficient          计算zernike系数
	 * @param ang                             相位
	 * @param mag                             幅度
	 * @param mask                            mask
	 * @param phase                           解包裹相位
	 * @return
	 */
	void calcZernikeCoefficient(const cv::Mat& ang,
		const cv::Mat& mag,
		const cv::Mat& mask,
		const cv::Mat& phase);

	void GetConMat();

	cv::Mat GetMaskSquare(cv::Size size, cv::Mat mask_roi, cv::Rect roi_in_square);
};

#endif // ZERNIKE_H


