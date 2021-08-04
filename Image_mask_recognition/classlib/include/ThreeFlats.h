/*****************************************************************
*
* fileName:  ThreeFkats.h
*
* function:  ��ƽ�滥��
*
* author:    ztb��Steven��
*
* date:      2021.3.2
*
* notes:     ��ƽ�滥��

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/
#pragma once
#ifndef THREEFLATS_H
#define THREEFLATS_H

#include "windows.h"
#include "algorithmMain.h"

//��ƽ�滥�췽��
enum THREEFLATS_METHOD {
	THREEFLATS_METHOD_ZERNIKE = 0,
	THREEFLATS_METHOD_N_AVERAGE = 1,
	THREEFLATS_METHOD_N_AVERAGE_PRO = 2,                 // ��ʵ�ִ���Ƶ��Ϣ���в�㣩�����λ�ԭ
};

// ��ż��
enum OddOrEven {
	Odd=0,
	Even=1,
};

// �������
struct TF_CONFIG_PARAMS {
	bool isNoTilt = true;
	int rotate_number = 0;
	float angle = 0.0f;
	THREEFLATS_METHOD threeflats_method = THREEFLATS_METHOD_ZERNIKE;
};

// ��������
struct TF_INPUT_DATAS {
	// Zernike��������˳��Ϊ����ǰΪ�������Ϊ�ο�����KL��D����ML��E����MK��F����MrotL��G��
	// Nλƽ����������˳��Ϊ����ǰΪ�������Ϊ�ο�����KL ��KrotL(n-1����ת��������ת8�Σ���KL+KrotL=8����ת�Ƕ�Ϊ45��)��KM��LM
	// �Ľ�Nλƽ����������˳��Ϊ����ǰΪ�������Ϊ�ο�����KL ��KrotL(n-1����ת��������ת8�Σ���KL+KrotL=8����ת�Ƕ�Ϊ45��)��KM��ML��K--M��K--��Ϊ��K��˳ʱ����ת180�㣩
	std::vector<cv::Mat> VzernikeCoef_37;                   // �����漯�ϵĶ�ӦZernikeϵ��
	std::vector<cv::Mat> VfittingSurface;                   // �����漯�ϵ���������
	// �㷨ʵ���е�roiMask
	cv::Mat mask;                                           // �û����õ���Ĥ
};

// ���
struct TF_RESULTS {
	// ��������Ͻ��
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
	ERROR_TYPE errorType = ERROR_TYPE_NOT_ERROR;           // ��������
	// ������zernike���ϵ��
	cv::Mat K_coef_37;
	cv::Mat L_coef_37;
	cv::Mat M_coef_37;

public:
	ThreeFlats(TF_CONFIG_PARAMS config, CONFIG_PARAMS userSets):tf_config(config),tf_userSets(userSets) {};
	~ThreeFlats() {};

	/**
     * @brief ThreeFlatsProcess                  ��ƽ�滥�칦�ܽӿ�
     * @param inputs                             ���������                   
     */
	void ThreeFlatsProcess(TF_INPUT_DATAS inputs);

	/**
     * @brief detecThreeFlat                    ��ƽ���⣨a, b, c��������ͳ��(ֻ�ܵõ�����ƽ��XY����Ƭ)
     * @param xSlice                            �����������귽�����Ƭ(a+b, a+c, b+c, br/cr)@������+�ο��棬br����b��ת180�ȣ�cr=c���ο��治�䣩
     * @param ySlice                            ������������귽�����Ƭ(a+b, a+c, b+c, br/cr)@������+�ο��棬br����b��ת180�ȣ�cr=c���ο��治�䣩
     * @param pv                                ����ƽ���x,y������PV��˳��Ϊ��a,b,c����һ��x,�ڶ���y��
     * @param rms                               ����ƽ���x,y������rms��˳��Ϊ��a,b,c����һ��x,�ڶ���y��
     * @return                                  ����ƽ��ļ�����(��x(abc)����y(abc))
     */
	static std::vector<cv::Mat> detecThreeFlat(std::vector<cv::Mat>xSlice,std::vector<cv::Mat>ySlice, float pv[2][3], float rms[2][3]); 

private:
	Zernike *tf_zernike;
	// ������Ч����ļ�����
	cv::Mat mag_square;
	cv::Mat ang_square;
	// zernike����ʱ�ĵ㼯��
	std::vector<cv::Point> maskPoints;

private:
	/*--------------------- Basic Function -------------------------*/
	/**
	 * @brief ThreeFlats_Init                   �쳣����ģ��
	 * @param inputs                            ���������   
	 */
	void ThreeFlats_Exception_Analysis(const TF_INPUT_DATAS &inputs);

	/**
	 * @brief ThreeFlats_Init                   ��ʼ��ģ��
	 * @param fittingsurface                    ���������ͼ
	 * @param mask                              �������Ĥ
	 */
	void ThreeFlats_Init(const cv::Mat &fittingsurface,const cv::Mat &mask);

	/**
     * @brief ThreeFlats_GetResult              ��ȡ���
     * @param inputs                            ���������
     */
	void ThreeFlats_GetResult(const TF_INPUT_DATAS &inputs);

	/*------------------- ��ƽ�滥��-Zernike�� -----------------------*/
	/**
	 * @brief ThreeFlats_Zernike                ��ƽ�滥�취����Zernike��
	 * @param inputs                            ���������
	 */
	void ThreeFlats_Zernike(const TF_INPUT_DATAS &inputs);

	/**
	 * @brief ThreeFlats_Zernike_FirstSort      Zernike����һ��ϵ�����㣬��ת�Գ���
	 * @param inputs                            ���������
	 */
	void ThreeFlats_Zernike_FirstSort(const TF_INPUT_DATAS &inputs);

	/**
	 * @brief ThreeFlats_Zernike_SecondSort     Zernike���ڶ���ϵ�����㣬XΪż�����ҷ���ת�Գ���
	 * @param inputs                            ���������
	 */
	void ThreeFlats_Zernike_SecondSort(const TF_INPUT_DATAS &inputs);

	/**
	 * @brief ThreeFlats_Zernike_ThirdSort      Zernike��������ϵ�����㣬XΪ�溯����YΪż����
	 * @param inputs                            ���������
	 */
	void ThreeFlats_Zernike_ThirtSort(const TF_INPUT_DATAS &inputs);

	/**
	 * @brief ThreeFlats_Zernike_FirstSort      Zernike����һ��ϵ�����㣬X��Y��Ϊ�溯��
	 * @param inputs                            ���������
	 */
	void ThreeFlats_Zernike_FourthSort(const TF_INPUT_DATAS &inputs);

	/*------------------- ��ƽ�滥��-Nλƽ���� -----------------------*/
	/**
	 * @brief ThreeFlats_N_Average              ��ƽ�滥�취����Nλƽ����
	 * @param inputs                            ���������
	 */
	void ThreeFlats_N_Average(const TF_INPUT_DATAS &inputs);

	/**
	 * @brief ThreeFlats_N_Average_Pretreatment Nλƽ��Ԥ����
	 * @param inputs                            ���������
	 * @return                                  Nλƽ�����
	 */
	vector<cv::Mat> ThreeFlats_N_Average_Pretreatment(const TF_INPUT_DATAS &inputs);

	/**
     * @brief ZernikeOdd                        ��ȡZernike������ϵ��
     * @param zernikecoef                       Zernikeϵ������
     * @return                                  Zernike����ϵ������
     */
	cv::Mat ZernikeOdd(const cv::Mat& zernikecoef);

	/**
	 * @brief ZernikeEven                       ��ȡZernikeż����ϵ��
	 * @param zernikecoef                       Zernikeϵ������
	 * @return                                  Zernikeż��ϵ������
	 */
	cv::Mat ZernikeEven(const cv::Mat& zernikecoef);

	/*------------------- ��ƽ�滥��-�Ľ���Nλƽ���� -----------------------*/
	/**
	 * @brief ThreeFlats_N_Average_Pro          ��ƽ�滥�취�����Ľ���Nλƽ����
	 * @param inputs                            ���������
	 */
	void ThreeFlats_N_Average_Pro(const TF_INPUT_DATAS &inputs);

	/**
	 * @brief Image_Flip                        ͼ��ת�����(x,y)��(-x,y)��(x,-y)��(-x,-y)
	 * @param phase                             ��ת��ͼ��
	 * @return                                  ��ת��Ľ������4������
	 */
	vector<cv::Mat> Image_Flip(const cv::Mat &phase);

	/**
	 * @brief ThreeFlats_N_Average_Pro_Calculation  �Ľ�Nλƽ��������ģ��
	 * @param W_Set                                 ��Ų��Բ��漰�䷭תͼ�����ݵļ��ϵ�����
	 * @return                                      ��ƽ�沨�����ݼ��ϣ���3��Mat��
	 */
	vector<cv::Mat> ThreeFlats_N_Average_Pro_Calculation(const vector<vector<cv::Mat>> W_Set);

};
#endif // THREEFLATS_H
