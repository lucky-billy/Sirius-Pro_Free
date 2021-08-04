/*****************************************************************
*
* fileName:  zernike.h
*
* function:  ����zernikeϵ�����������
*
* author:    sjt&ztb��Steven��
*
* date:      2021.1.26
*
* notes:     ����zernikeϵ�����������

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

//�Խ���ĶԽ���Ԫ��ֵ
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

// �Ƴ�����������ı�־��trueΪ�Ƴ�
struct REMOVE_ZERNIKE_FLAGS {
	bool isPolySelect = false;                   // �ж��Ƿ���36�����ʽ��������־

	bool positionFlag = true;                    // ȥ��ƫ�����ı�־
	bool tiltFlag = true;                        // ȥ����б���ı�־
	bool powerFlag = false;                      // ȥ��power�ı�־
	bool astFlag = false;                        // ȥ��ast�ı�־
	bool comaFlag = false;                       // ȥ��coma�ı�־
	bool sphericalFlag = false;                  // sȥ��pherical�ı�־

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
	cv::Mat coef_display_4;                      // ������ʾ��zernike 4��ϵ�����
	cv::Mat coef_display_9;                      // ������ʾ��zernike 9��ϵ����� ��term >= 9��
	cv::Mat coef_display_16;                     // ������ʾ��zernike 16��ϵ����� ��term >= 16��
	cv::Mat coef_display_25;                     // ������ʾ��zernike 25��ϵ����� ��term >= 25��
	cv::Mat coef_display_36;                     // ������ʾ��zernike 36��ϵ����� ��term >= 36��
	cv::Mat coef_display_37;                     // ������ʾ��zernike 37��ϵ����� ��term >= 37��

	cv::Mat coefficient_4;                       // 4��zernikeϵ��(zernike power, zernike tilt, concavity, Rms(Power), seidel tilt, seidel tilt clock, Iso Sag, Parallel Theta)
	cv::Mat coefficient_9;                       // 9��zernikeϵ�� ��term >= 9��
	cv::Mat coefficient_16;                      // 16��zernikeϵ�� ��term >= 16��
	cv::Mat coefficient_25;                      // 25��zernikeϵ�� ��term >= 25��
	cv::Mat coefficient_36;                      // 36��zernikeϵ�� ��term >= 36��
	cv::Mat coefficient_37;                      // 37��zernikeϵ�� ��term >= 37��

	cv::Mat unwrapphase;                         // ��λͼ
	cv::Mat fitting_all;                         // zernikeϵ����Ͻ��(ttv, fringes, zernike residual)
	cv::Mat fitting_rem_zer;                     // ȥ���û����õ��Ƴ���֮����ϵĽ�������ڼ���pv��rms�Ȳ���ʱ��ʹ�õ���Ͻ��
	cv::Mat fitting_r0_r1_r2;                    // �Ƴ�ǰ3�����Ͻ��������ISO��Rmstʱʹ�ã�ǰ3����ָ4�����ϵ����ǰ3������
	cv::Mat fitting_r0_r1_r2_r3;                 // �Ƴ�ǰ4�����Ͻ��������ISO��Irr��Rmsa, Rmsiʱʹ�ã�ǰ3����ָ4�����ϵ����ǰ4������
	cv::Mat fitting_z8_z15_z24_z35_z36;          // zernikeϵ���б�ʾ����������Ͻ��������ISO��Rsi,Rmsaʱʹ��
	cv::Mat zernike_residual;                    // �в�

	int term;                                    // zernike����
	cv::Mat zerConMat;                           // ����zernikeϵ��ʱ���õ���ת������37*37��
	cv::Mat zerConMatInv;                        // zernikeϵ�������������ʱ���õ���ת������������37*37��
	cv::Mat zernikeValue;                        // 37��zernike����ʽ��ֵ��37*1��
	cv::Mat OrthoDiag;                           // ����ϵ������

	cv::Mat zernikeConvertMatrix;                // ��������ʽת������
	cv::Mat identityMatrix;                      // ��λ��
	cv::Mat zernikeConvertMatrixInv;             // ��������ʽת������������
	cv::Mat identityMatrixInv;                   // ��λ��������

	bool removeResidualFlag = false;             // �Ƴ��в�ı�־
	bool calcFitR012Flag = false;                // ����fitting_r0_r1_r2�ı�־
	bool calcFitR0123Flag = false;               // ����fitting_r0_r1_r2_r3�ı�־
	bool calcFitZ8toZ36Flag = false;             // ����fitting_z8_z15_z24_z35_z36�ı�־
	bool convertFinish = false;                  // �Ƿ����ת������
	ERROR_TYPE errorType = ERROR_TYPE_NOT_ERROR;           // ��������
	ZERNIKE_METHOD zernikeMethod = ZERNIKE_METHOD_CIRCLE;// ����zernikeϵ��ʱ��ѡ��ת������ķ���
	REMOVE_ZERNIKE_FLAGS removeZernikeFlags;     // �Ƴ�zernike����ʽ�ı�־

public:
	Zernike(int inputTerm = 37);

	/**
	 * @brief ZernikeProcess                     zernike������
	 * @param ang                                ��λ
	 * @param mag                                ����
	 * @param mask                               mask
	 * @param phase                              �������λ
	 * @return
	 */
	void ZernikeProcess(const cv::Mat& ang,
		const cv::Mat& mag,
		const cv::Mat& mask,
		const cv::Mat& phase);

	void zernikeinit();

	/**
	 * @brief ZernikeConvert 32FC1                ����zernikeת������
	 * @param _ang                                ��λkAng129 or kAng259
	 * @param _mag                                ��ֵkMag129 or kMag259
	 * @param _mask square matrix                 n*n��mask
	 * @param terms                               zernike����
	 * @return                                    ת�����󣬼���Բ��zernike���ϵ��ʱ���þ���Ϊ��λ��
	 */
	cv::Mat ZernikeConvert(cv::Size size, cv::Mat mask_roi, cv::Rect roi_in_square);

	/**
	* @brief ZernikeA 32FC1                      �õ�37��zernike����ʽ
	* @param ang                                 ��λ
	* @param mag                                 ��ֵ
	* @param sampling                            ����������
	* @return                                    terms��zernike����ʽ
	*/
	cv::Mat ZernikeA(const cv::Mat& ang,
		const cv::Mat& mag,
		const std::vector<cv::Point>& sampling,
		int terms);

	// �õ�37��zernike����ʽ(��֤�ٶȽϿ�)
	cv::Mat ZernikeA2(const cv::Mat &ang,
		const cv::Mat &mag,
		const std::vector<cv::Point> &sampling,
		int terms);

	// �õ�37��zernike����ʽ(��֤�ٶ����)
	cv::Mat ZernikeA1(const cv::Mat &ang,
		const cv::Mat &mag,
		const std::vector<cv::Point> &sampling,
		int terms);

	/**
	 * @brief ZernikeFitting                  ����zernike����ʽ�������
	 * @param coefForFitting                  zernikeϵ��
	 * @param mask                            mask
	 * @param maskPoints                      mask�в�Ϊ0�ĵ㼯
	 * @return                                �������
	 */
	cv::Mat ZernikeFitting(const cv::Mat &coefForFitting,
		const cv::Mat &mask,
		const std::vector<cv::Point> &maskPoints);

	/**
	 * @brief ItemMask                        ���ø���ȥ����
	 * @param remove_piston                   piston
	 * @param remove_tilt                     tilt
	 * @param remove_power                    power
	 * @param remove_ast                      ast
	 * @param remove_coma                     coma
	 * @param remove_spherical                spherical
	 * @param terms                           ����
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
	* @param A                                   ������ʵ�Գ���
	* @param S                                   �õ�����������
	* @note  Cholesky�ֽ�
	*/
	void Cholf(const cv::Mat& A, cv::Mat& S);

	/**
	* @brief ZernikeB                            �õ�sampling��������,���ݳ���
	* @param wave                                �����������ݣ�һ���ǽ��������
	* @param sampling                            �����㴦������
	* @return                                    �����������
	*/
	cv::Mat ZernikeB(const cv::Mat& wave,
		const std::vector<cv::Point>& sampling);

	/**
	* @brief ZernikeCoefficient              ������淽��A��A * dst = A'B�е�dst,�õ�zernikeϵ��
	* @param zernikeA                        zernike����ʽ
	* @param zernikeB                        �����������
	* @return                                zernikeϵ��
	*/
	cv::Mat ZernikeCoefficient(const cv::Mat& zernikeA,
		const cv::Mat& zernikeB);

	/**
	 * @brief calcZernikeCoefficient          ����zernikeϵ��
	 * @param ang                             ��λ
	 * @param mag                             ����
	 * @param mask                            mask
	 * @param phase                           �������λ
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


