/*****************************************************************
*
* fileName:  unwrap.h
*
* function:  ��λ�����
*
* author:    sjt&ztb
*
* date:      2021.1.26
*
* notes:     ������㷨

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/

#pragma once
#ifndef UNWRAP_H
#define UNWRAP_H

#include <Windows.h>
#include "baseFunc.h"
using namespace std;

enum UNWRAP_METHOD {
	UNWRAP_METHOD_BRANCH_CUT = 0,          //֦�з�
	UNWRAP_METHOD_HISTOGRAM=1,             //����ͼ��δ��ɣ�
	UNWRAP_METHOD_SPAN_BRANCH_CUT=2,       //����֦�з�
	UNWRAP_METHOD_DCT=3,                   //DCTȫ�ַ��������ƣ�
	UNWRAP_METHOD_RESERVE3
};

class Unwrap {
public:
	ERROR_TYPE errorType;                  // ��������
	UNWRAP_METHOD unwrapMethod;            // ������ķ���
	cv::Mat unwrappedPhase;                // ����������λ
	cv::Mat unwrappedPhase_original;       // �洢ԭʼ����������λ
	int maxBoxRadius;                      // ֦�з�����������������İ뾶
	int residuesNumThresh;                 // �в�������ֵ
	int rowref;                            // �ο���ʼ���������
	int colref;                            // �ο���ʼ���������
	float adjust;                          // ����֦�з������ڵ�������

public:
	Unwrap(int inputBoxRadius = 20, int inputThresh = 150, float inputadjust=0.05,UNWRAP_METHOD Method = UNWRAP_METHOD_BRANCH_CUT)
		: maxBoxRadius(inputBoxRadius), residuesNumThresh(inputThresh),adjust(inputadjust), unwrapMethod(Method){}

	/**
	* @brief UnwrapProcess            ����������г���
	* @param phase                    ������λ
	* @param modulation               ���ƶ�
	* @Param mask                     ��Ĥ
	*/
	void UnwrapProcess(cv::Mat &phase,
		const cv::Mat &modulation,
		const cv::Mat &mask);

	// �������������� a / b
	inline float modf(float a, float b) {
		if (b == 0.0f) {
			return a;
		}
		else {
			return a - b * floorf(a / b);
		}
	}

	/**
	* @brief getUnwrappedPhase        ��ý��������
	* @param phase                    ������λ
	* @param modulation               ���ƶ�
	* @param branchcuts               ֦����
	* @param mask                     ��Ĥ
	* @return                         �������Ľ��
	*/
	inline cv::Mat getUnwrappedPhase(const cv::Mat& phase,
		const cv::Mat& modulation,
		const cv::Mat& branchcuts,
		const cv::Mat& mask) {

		// �ҵ���ʼ�ο������꣬ȡ���ǵ��ƶȴ���ĳ��ֵ�����е�����ĵ�
		GetRefPoint(modulation, branchcuts);

		// ͼ��ȫ�׻���ȫ��ʱ������ֱ�����Bug
		if (errorType == ERROR_TYPE_UNWRAP_ERROR) {
			return phase.clone();
		}
		// �������ˮ��䷨�����
		// cv::Mat unwrappedphase = ptrGoldsteinFloodFill(phase, modulation, branchcuts, mask);
		// cv::Mat unwrappedphase = ptrGoldsteinFloodFill2(phase, modulation, branchcuts, mask);
		cv::Mat unwrappedphase = ptrGoldsteinFloodFill3(phase, modulation, branchcuts, mask);
		return unwrappedphase;
	}

private:
	// ������������������ a ./ b
	cv::Mat modf(const cv::Mat& a, const cv::Mat& b);
	
	// ���������������� a / b 
	cv::Mat modf(const cv::Mat& a, float b);

	/*--------------------- BranchCuts -------------------------*/
	/**
	* @brief UnwrapBranchCuts         ֦�з���λ�����
	* @param phase                    ������λ
	* @param modulation               ���ƶ�
	* @Param mask                     ��Ĥ
	* @return                         
	*/
	int UnwrapBranchCuts(cv::Mat &phase,
		const cv::Mat &modulation,
		const cv::Mat &mask);

	/**
	* @brief PhaseResidues            ���������λ�в�
	* @param phase CV_32FC1           ������λ
	* @return CV_8SC1 -127,0,127      �в�
	*/
	cv::Mat PhaseResidues(const cv::Mat& phase);

	/**
	* @brief ResiduesCount            ͳ�Ʋв��ĸ���
	* @param residues                 PhaseResidues����õ��Ĳв�
	* @Param mask                     ��Ĥ
	* @return                         �в�����
	*/
	inline int ResiduesCount(const cv::Mat& residues,
		const cv::Mat& mask);

	/**
	* @brief ResiduesCount            ���ݲв�õ�֦����
	* @param residues                 PhaseResidues����õ��Ĳв�
	* @Param mask                     ��Ĥ
	* @Param maxboxradius             ���������İ뾶
	* @return                         ֦����
	*/
	cv::Mat BranchCuts(const cv::Mat& residues,
		const cv::Mat& mask,
		int maxboxradius = 20);

	/**
	* @brief PlaceBranchCutInternal   ������֮�����֦����
	* @param branchcuts               ֦����
	* @param r0                       0��������
	* @param c0                       0��������
	* @param r1                       1��������
	* @param c1                       1��������
	* @return BranchCuts              ֦����
	*/
	void PlaceBranchCutInternal(cv::Mat& branchcuts,
		int r0,
		int c0,
		int r1,
		int c1);

	/**
	* @brief getCoordinates           ���Ҳ�Ϊ0�ĵ������
	* @param adjoin                   ���ڵ�
	* @param rows                     mask��rows
	* @param cols                     mask��cols
	* @param Coordinate               ���ڵ������
	* @param count                    ƫ��������ʼ��ĸ���
	* @retur                          ��Ϊ0�ĵ�ĸ���
	*/
	int getCoordinates(uchar **adjoin,
		int rows,
		int cols,
		int **Coordinate,
		int count);

	/**
	* @brief getCoordinates           ���Ҳ�Ϊ0�ĵ������
	* @param adjoin                   ���ڵ�
	* @param rows                     mask��rows
	* @param cols                     mask��cols
	* @param Coordinate               ���ڵ������
	* @param count                    ƫ��������ʼ��ĸ���
	* @retur                          ��Ϊ0�ĵ�ĸ���
	*/
	int getCoordinates2(uchar *adjoin,
		int rows,
		int cols,
		int **Coordinate,
		int count);

	/**
	* @brief GoldsteinFloodFill       �������ˮ��䷨�����
	* @param phase                    ������λ
	* @param modulation               ���ƶ�
	* @param branchcuts               ֦����
	* @param mask                     ��Ĥ
	* @return                         �������Ľ��
	*/
	cv::Mat GoldsteinFloodFill(const cv::Mat& phase,
		const cv::Mat& modulation,
		const cv::Mat& branchcuts,
		const cv::Mat& mask);

	/**
	* @brief ptrGoldsteinFloodFill    �������ˮ��䷨�����,��ptr�滻at
	* @param phase                    ������λ
	* @param modulation               ���ƶ�
	* @param branchcuts               ֦����
	* @param mask                     ��Ĥ
	* @return                         �������Ľ��
	*/
	cv::Mat ptrGoldsteinFloodFill(const cv::Mat& phase,
		const cv::Mat& modulation,
		const cv::Mat& branchcuts,
		const cv::Mat& mask);

	/**
	* @brief ptrGoldsteinFloodFill2   �������ˮ��䷨���������ptr�滻at,��getCoordinates�滻findNonZero��
	* @param phase                    ������λ
	* @param modulation               ���ƶ�
	* @param branchcuts               ֦����
	* @param mask                     ��Ĥ
	* @return                         �������Ľ��
	*/
	cv::Mat ptrGoldsteinFloodFill2(const cv::Mat &phase,
		const cv::Mat &modulation, 
		const cv::Mat &branchcuts,
		const cv::Mat &mask);

	/**
	* @brief ptrGoldsteinFloodFill2   �������ˮ��䷨���������ptr�滻at,��getCoordinates�滻findNonZero,��һά�����滻��ά����
	* @param phase                    ������λ
	* @param modulation               ���ƶ�
	* @param branchcuts               ֦����
	* @param mask                     ��Ĥ
	* @return                         �������Ľ��
	*/
	cv::Mat ptrGoldsteinFloodFill3(const cv::Mat &phase,
		const cv::Mat &modulation,
		const cv::Mat &branchcuts,
		const cv::Mat &mask);

	/**
	* @brief GetRefPoint              �����ʼ�ο��������
	* @param modulation               ���ƶ�
	* @param branchcuts               ֦����
	*/
	void GetRefPoint(const cv::Mat& modulation,
		const cv::Mat& branchcuts);

	/*--------------------- SpanBranchCuts -------------------------*/
	/**
	* @brief UnwrapSpanBranchCuts     ����֦�з������
	* @param phase                    ������λ
	* @param modulation               ���ƶ�
	* @Param mask                     ��Ĥ
	*/
	int UnwrapSpanBranchCuts(cv::Mat &phase,
		const cv::Mat &modulation,
		const cv::Mat &mask,
		const float adjust);

	/**
    * @brief UnwrapSpanCalcTile       ���������������ο���ͨ���������(ƽ��)
    * @param unwrap_phase             �ο����Ľ��������λ
    * @Param mask                     �ο�������Ĥ
    */
	cv::Mat UnwrapSpanCalcTile(const cv::Mat &unwrap_phase,
		const cv::Mat &mask);

	/*--------------------- DCT�������ƣ� -------------------------*/
	//void DctProcess(cv::Mat &phase,
	//	const cv::Mat &modulation,
	//	const cv::Mat &mask);
};

#endif // UNWRAP_H

