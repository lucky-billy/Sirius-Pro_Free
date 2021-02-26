/*****************************************************************
*
* fileName:  psi.h
*
* function:  (Phase shift interferometry)�������ͼ�и������λ
*
* author:    sjt&ztb
*
* date:      2021.1.26
*
* notes:     ������λ

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/

#pragma once
#ifndef PSI_H
#define PSI_H

#include <omp.h>
#include "baseFunc.h"

#define PHASE_SHIFT_THR      10                     // ����10�㣬��Ϊ�Ѿ��Բ�������нϴ�Ӱ����
#define IMG_0_SUB_4_STD_THR  30                     // 30�Ѿ�����Խϴ�Ľ���ˣ�����׼ȷ�޸���ʱ��һ�㶼��10����
#define STD_RATIO_THR        0.2                    // ��׼�����Һ���������90�㣬ƫ��10��ʱ����׼���ֵΪ[0.9091, 1.1]

#define DISTINGUISHING_THR   160000                 // shift_std����mask��أ�maskԽ��std��Խ�С
#define PHASE_SHIFT_STD_THR1 0.05                   // ����׼ȷ���޸���ʱ��һ�㶼��0.07���ң��и���ʱ��һ����0.03����
#define PHASE_SHIFT_STD_THR2 0.04                   // ����׼ȷ���޸���ʱ��һ�㶼��0.06���ң��и���ʱ��һ����0.02����

#define AIA_THR              0.001

// ���Ʒ���
enum PSI_METHOD{
	PSI_METHOD_BUCKET4A_P = 0,
	PSI_METHOD_BUCKETB5A_H_P,
	PSI_METHOD_BUCKET5B_CS_P,
	PSI_METHOD_BUCKET9A_CS_P,
	PSI_METHOD_BUCKET9B_CS_P,
	PSI_METHOD_OPT_SEQUENCE
};

class Psi {
public:
	bool checkFalg;                                 // �Ƿ�������Ƽ��
	bool isSingleHole;                              // ���ױ�־��true�����ף�false:���
	cv::Mat phaseModulation;                        // �źŵ���
	cv::Mat phase;                                  // ����
	ERROR_TYPE errorType;                           // ��������
	PSI_METHOD psiMethod;                           // psi����
	CHECK_THR checkThr;                             // check���ܵ���ֵ

public:
	Psi(bool isCheck, PSI_METHOD method, bool singleHoleFlag, CHECK_THR thr)
		: checkFalg(isCheck), psiMethod(method), isSingleHole(singleHoleFlag), checkThr(thr) {}
	
	/**
	* @brief PsiProcess        psi�㷨�ӿڣ����������λ����λ���ƶ� gama = I'' / I'
	* @param srcs              �����ͼ��
	* @return
	*/
	void PsiProcess(std::vector<cv::Mat> srcs, cv::Mat mask);

	/**
	* @brief psiPretreatment   ����Ԥ����(��ѡͼ�񣬼�������Ƿ�׼ȷ)
	* @param srcs              �����ͼ��
	* @param mask              �����mask
	* @return                  ��ѡ���ͼ��
	*/
	std::vector<cv::Mat> psiPretreatment(const std::vector<cv::Mat> srcs, cv::Mat mask);

	/**
	 * @brief ImgNum �����㷨�����ͼƬ��
	 * @return
	 */
	inline unsigned int imgNum() {
		switch (psiMethod) {
		case PSI_METHOD_BUCKET4A_P:  return 4;
		case PSI_METHOD_BUCKETB5A_H_P:
		case PSI_METHOD_BUCKET5B_CS_P: return 5;
		case PSI_METHOD_BUCKET9A_CS_P:
		case PSI_METHOD_BUCKET9B_CS_P: return 9;
		case PSI_METHOD_OPT_SEQUENCE: return 5;
		default:
			return 0;
		}
	}

private:
	/**
	* @brief AIA               ���Ƶ����㷨
	* @param srcs              �����ͼ��
	* @param mask              �����mask
	* @param iterNum           ��������
	* @return                  
	*/
	void AIA(std::vector<cv::Mat> srcs, cv::Mat mask, int iterNum);
	
	/**
	* @brief Todelta           ����λ�ֲ����������
	* @param delta             ����Ƕ�
	* @param srcs              �����ͼ��
	* @param mask              �����mask
	* @param a                 ��������
	* @param b                 ��������
	* @return
	*/
	void Todelta(float *delta, std::vector<cv::Mat> srcs, cv::Mat mask, float *a, float *b);
	
	// ����λ�ֲ���������� (�ٶȽϿ�)
	void Todelta2(float *delta, std::vector<cv::Mat> srcs, cv::Mat mask, float *a, float *b);

	/**
	* @brief Todelta           �������������λ�ֲ�
	* @param delta             ����Ƕ�
	* @param srcs              �����ͼ��
	* @param mask              �����mask
	* @param a                 ��������
	* @param b                 ��������
	* @return
	*/
	void Tophase(float *delta, std::vector<cv::Mat> srcs, cv::Mat mask, float *a, float *b);
	
	// �������������λ�ֲ� (�ٶȽϿ�)
	void Tophase2(float *delta, std::vector<cv::Mat> srcs, cv::Mat mask, float *a, float *b);

	// �������������λ�ֲ� (�ٶ����)
	void Tophase3(float *delta, std::vector<cv::Mat> srcs, cv::Mat mask, float *a, float *b);

	/**
	* @brief findGoodPics      �ҵ����ƽϺõ�ͼ��
	* @param srcs              �����ͼ��
	* @param mask              �����mask
	* @return                  ���ƽϺõĸ���ͼ
	*/
	std::vector<cv::Mat> findGoodPics(std::vector<cv::Mat> srcs, cv::Mat mask);

	/**
	* @brief calcShiftAndStd   �����㷨������ƫ�ƽǶ���Ƕ�ֱ��ͼ�ı�׼��
	* @param srcs0             �����ͼ��0
	* @param srcs1             �����ͼ��1
	* @param srcs3             �����ͼ��3
	* @param srcs4             �����ͼ��4
	* @param mask              �����mask
	* @param data              ����ĸ�����ʼ��λ�µ����Ƽ���׼��
	* @param idx               ���ŵ�һ�����ͼ����ʼ����
	* @return                  
	*/
	void calcShiftAndStd(cv::Mat srcs0,
		cv::Mat srcs1,
		cv::Mat srcs3,
		cv::Mat srcs4,
		cv::Mat mask,
		float data[2][5],
		int idx);

	/**
	* @brief checkPsi          ��������Ƿ�׼ȷ
	* @param srcs              �����ͼ��
	* @param mask              �����mask
	* @return                  
	*/
	void checkPsi(const std::vector<cv::Mat> srcs, cv::Mat mask);

	/**
	 * @brief Bucket4A_P
	 * @note Phase Shift: 90; Purpose: General purposes.
	 * @param I CV_32FC1
	 * @return
	 */
	cv::Mat Bucket4A_P(const std::vector<cv::Mat>& I);

	/**
	 * @brief Bucket4A_M    �õ���������ͼ�����ݵ���ϵ��gama = I'' / I'
	 * @param I
	 * @return
	 */
	cv::Mat Bucket4A_M(const std::vector<cv::Mat>& I);

	/**
	 * @brief Bucket5A_H_P
	 * @note Phase Shift: 90; Purpose: Phase shift error correction.
	 * @param I
	 * @return
	 */
	cv::Mat Bucket5A_H_P(const std::vector<cv::Mat>& I);

	/**
	 * @brief Bucket5A_H_M   �õ���������ͼ�����ݵ���ϵ��gama = I'' / I'
	 * @param I
	 * @return
	 */
	cv::Mat Bucket5A_H_M(const std::vector<cv::Mat>& I);

	/**
	 * @brief Bucket5B_CS_P
	 * @note Phase Shift: 90; Purpose: Enhanced phase shift error correction.
	 * @param I
	 * @return
	 */
	cv::Mat Bucket5B_CS_P(const std::vector<cv::Mat>& I);

	/**
	 * @brief Bucket5B_CS_M
	 * @param I
	 * @return           �õ���������ͼ�����ݵ���ϵ��gama = I'' / I'
	 */
	cv::Mat Bucket5B_CS_M(const std::vector<cv::Mat>& I);

	/**
	 * @brief Bucket9A_CS_P
	 * @note Phase Shift: 90; Purpose: Enhanced phase shift error correction.
	 * @param I
	 * @return
	 */
	cv::Mat Bucket9A_CS_P(const std::vector<cv::Mat>& I);

	/**
	 * @brief Bucket9A_CS_M
	 * @param I
	 * @return           �õ���������ͼ�����ݵ���ϵ��gama = I'' / I'
	 */
	cv::Mat Bucket9A_CS_M(const std::vector<cv::Mat>& I);

	/**
	 * @brief Bucket9B_CS_P
	 * @note Phase Shift: 90; Purpose: Enhanced phase shift error correction.
	 * @param I
	 * @return
	 */
	cv::Mat Bucket9B_CS_P(const std::vector<cv::Mat>& I);

	/**
	 * @brief Bucket9B_CS_M
	 * @param I
	 * @return           �õ���������ͼ�����ݵ���ϵ��gama = I'' / I'
	 */
	cv::Mat Bucket9B_CS_M(const std::vector<cv::Mat>& I);
};

#endif
