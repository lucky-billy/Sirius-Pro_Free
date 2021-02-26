/*****************************************************************
*
* fileName:  algorithmMain.h
*
* function:  �㷨�������ӿ�
*
* author:    sjt&ztb
*
* date:      2021.1.26
*
* notes:     ��Ӧ�ò�ֻ��һ���ӿڣ��������

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/

#pragma once
#ifndef ALGORITHMMAIN_H
#define ALGORITHMMAIN_H

#include "psi.h"
#include "psf.h"
#include "unwrap.h"
#include "filter.h"
#include "edgedetector.h"
#include "result.h"

// �׵�����
enum HOLE_TYPE {
	HOLE_TYPE_CG = 0,                                      // �����
	HOLE_TYPE_IR,                                          // ��ī��
	HOLE_TYPE_REVERSE1,                                    // Ԥ��1
	HOLE_TYPE_REVERSE2                                     // Ԥ��2
};

// ���ò���
struct CONFIG_PARAMS {
	bool isPhaseAverage = false;                           // �ж��Ƿ����λ��ƽ����״̬
	bool isEdgeEroding = false;                            // ��Ե��ʴ��������1��pixel
	bool isautomask = false;                               // �Զ���Ĥ�������������Ҫ��
	bool isASCFile = false;                                // asc�ļ��ı�־��asc�ļ��е����ݣ�����Ҫ�������Ƽ����Լ���������㣩
	bool checkFlag = false;                                // ���λ��ͼ�����ı�־
	bool removeErrorFlag = false;                          // �Ƴ�����ı�־����ʱδ��
	bool removeResidualFlag = false;                       // �Ƴ��в�ı�־
	bool isSingleHole = true;                              // ���ױ�־��true�����ף�false:��ף�Pro�汾��־��true:pro,false:ind��
	bool isFillSpikes = false;                             // ���ë�̵�ı�־��true����䣬false�������
	unsigned int residuesNumThresh = 500;				   // �в�������ֵ
	unsigned int span_automask_thresh = 40;                // �����ɸѡ����ֵ
	int zernikeTerm = 37;                                  // zernike����ʽ������
	int unwrap_max_radius = 5;                             // ֦�з�֦����������Ӱ뾶
	int edge_erode_r = 1;                                  // ��ʴ�ߴ�
	int debugGrade = 0;                                    // ��ӡ��Ϣ�ı�־0������ӡ�κ���Ϣ��1��ֻ��ӡ��������2����ӡ�����Ŀ�ʼ���������������3����ӡ���е���Ϣ
	float unwrap_span_adjust = -0.1f;                      // �������������������
	float scaleFactor = 0.5f;                              // ��������
	float highPassFilterCoef = 0.01f;                      // �����ͨ�˲��뾶��ϵ��
	HOLE_TYPE holeType = HOLE_TYPE_CG;                     // �׵�����
	CHECK_THR checkThr;                                    // �������ò���
	PSI_METHOD psiMethod = PSI_METHOD_BUCKET9A_CS_P;       // ���Ƽ��㷽��
	UNWRAP_METHOD unwrapMethod = UNWRAP_METHOD_BRANCH_CUT; // ���������
	ZERNIKE_METHOD zernikeMethod = ZERNIKE_METHOD_CIRCLE;  // zernike���㷽��
	FILTER_PARAMS filterParams;                            // �˲�ȥë�̵Ĳ���
	EDGE_DETEC_PARAMS edgeDetecParams;                     // ��Ե�������������� 
	REMOVE_ZERNIKE_FLAGS removeZernikeFlags;               // zernike�����Ƴ���ı�־
	CALC_TOTAL_RST_FLAGS calcTotalResultFlags;             // Ҫ�����result����Щ��ı�־
	CALC_RST_INPUT_PARAMS calcResultInputParams;           // ������ؽ��ʱ���û����ò���
};

// ��������
struct INPUT_DATAS {
	std::vector<cv::Mat> oriPhase;                         // ԭʼͼ������
	cv::Mat mask;                                          // �û����õ���Ĥ
};

// �㷨��
class AlgorithmMain {
public:
	cv::Mat roiMask;                                       // ��С��ΧԲ����Ӿ���
	cv::Mat retMask;                                       // ������ͼ���С��ͬ��mask,
	cv::Mat maskEdge;                                      // mask�ı�Ե����С��������ͼ��Ĵ�С

	cv::Mat unwrappedPhase;                                // ��λ�������Ľ���� ��СΪ����ͼ��Ĵ�С
	cv::Mat unwrappedPhase_original;                       // δ���κδ���Ľ�������
	cv::Mat xSlice;                                        // ĳһ�е�ͼ����Ƭ����λ��������
	cv::Mat ySlice;                                        // ĳһ�е�ͼ����Ƭ����λ��������
	cv::Mat PSD_X;                                         // X��Ƭ��Ӧ��PSDͼ����������������λ������^2*mm��
	cv::Mat PSD_Y;                                         // Y��Ƭ��Ӧ��PSDͼ����������������λ������^2*mm��

	cv::Mat zernikeCoef_4;                                 // zernikeϵ��
	cv::Mat zernikeCoef_9;                                 // zernikeϵ��
	cv::Mat zernikeCoef_16;                                // zernikeϵ��
	cv::Mat zernikeCoef_25;                                // zernikeϵ��
	cv::Mat zernikeCoef_36;                                // zernikeϵ��
	cv::Mat zernikeCoef_37;                                // zernikeϵ��
	cv::Mat fittingSurface_all;                            // zernike��Ͻ�����������
	cv::Mat fittingSurface;                                // zernike��ϵĽ���� ��СΪ��С��ΧԲ����Ӿ��εĴ�С����λ��������
	cv::Mat zernikeResidual;                               // �в��СΪ��С��ΧԲ����Ӿ��εĴ�С����λ��������
	cv::Mat fittingSurface_fillSpikes;                     // ��ë�̵㴦��ֵ����ϵĽ����䣨��λ��������
	 
	cv::Mat psf;                                           // PSF
	cv::Mat psfEnergy;                                     // psf energy
	cv::Mat mtf;                                           // MTF, ��ʱû�м���
	cv::Mat intensity;                                     // intensity, ��ʱû�м��㣬��СΪ��С��ΧԲ����Ӿ��εĴ�С
	cv::Rect mask_rect;                                    // ��Ĥ��ԭͼ�е�λ����Ϣ

	ERROR_TYPE errorType = ERROR_TYPE_NOT_ERROR;           // ��������
	TOTAL_RESULT totalResult;                              // ������ܵĽ��
	CONFIG_PARAMS configParams;                            // ���ò���

public:
	AlgorithmMain() { }

	AlgorithmMain(CONFIG_PARAMS userSets) : configParams(userSets) {}

	// �㷨����������
	void process(INPUT_DATAS inputDatas);

	// ��ʼ�������еľ��󣬷�ֹ����ʹ��ʱ����
	void initMemberVar(int rows, int cols);

	// �޸������ļ�,�������µ����㷨
	void modifyConfigParams(CONFIG_PARAMS userSets);

	// ͨ�������ͼ��õ�������ݺ��������
	void ResultFromUnwrapList(vector<cv::Mat> unwraplist, const cv::Mat mask);

	// ͨ������ͼ�õ��������
	TOTAL_RESULT ResultFromSurface(const cv::Mat fittingsurface, const cv::Mat surface_tilt, const cv::Mat mask);

private:
	// ����λ��ͼ���н������zernike����Լ����ֽ���ļ���
	void processInterferogram(INPUT_DATAS inputDatas);

	// ����λ��ͼ���н�������õ��������ͼ
	void processGetUnwrappedPhase(INPUT_DATAS inputDatas);

	// ����ȡ����asc�ļ�ʱ���㷨������
	void processASCFile(INPUT_DATAS inputDatas);

	// ����zernike��ؼ���ʱ�Ĳ���
	void setZernikeConfig(Zernike &zernike);

	// �ı��ⲿ����Ĵ����������ֹ�������
	void changeErrorParams();

	// �в��ͨ�˲�
	void residualHighPassFilter(Zernike &zernike);

	// ���zernikeϵ������Ͻ��
	void getZernikeResult(Zernike zernike);

	// ���ë�̵㴦��zernieke��Ͻ��
	void getSpikesValue(Zernike zernike, cv::Mat tmpMask);

	// ����ܵļ�����
	void getTotalResult(Result inputResult);

	// ��ü���powerX��powerY�Ĳ���
	void getPowerXYParams();

	/**
	* @brief removeNanPoint                     �Ƴ�zernikeValue�в�׼ȷ��ֵ
	* @param originalMask                       �˲���ȥë��֮ǰ��ԭʼmask
	* @param finalMask                          �˲���ȥë��֮ǰ������mask
	* @param zernike                            zernike
	* @param spikesZernikeValue                 ë�̵㴦��zernikeValue
	* @note  
	*/
	cv::Mat removeNanPoint(cv::Mat &originalMask,
		cv::Mat &finalMask,
		Zernike &zernike,
		cv::Mat &spikesZernikeValue);

	// ��ӡ�����������ʽ�汾�У�ȥ���ú���
	void PrintResult();
};

/***********************************************************
* �����ǹ��ڶ��̵߳Ĳ���
************************************************************/

/**
* @brief calcPsiFilterUnwrapForThread          ����psi,unwrap,filter���߳� 
* @param psi                                   Psi����
* @param unwrap                                Unwrap����
* @param images_roi                            ��������
* @param filterParams                          �����˲�����
* @param roiMask                               mask
* @note
*/
void calcPsiFilterUnwrapForThread(Psi &psi,
	Unwrap &unwrap,
	const std::vector<cv::Mat> &images_roi,
	const FILTER_PARAMS &filterParams,
	const cv::Mat &roiMask);

/**
* @brief calcZernikeForThread                  ����zernike����ʽ���߳�
* @param zernike                               Zernike����
* @param ang_roi                               ��λ
* @param mag_roi                               ��ֵ
* @param roiMask                               mask
* @note
*/
void calcZernikeForThread(Zernike &zernike,
	const cv::Mat &ang_roi,
	const cv::Mat& mag_roi,
	const cv::Mat &roiMask);

// ���㷨�п��ٶ��̣߳�
void mulHolesForThread(AlgorithmMain &algorithm,
	const INPUT_DATAS &inputDatas);

// ���㷨�п��ٶ��̣߳��ٷ�װһ���㷨
class AlgorithmMainForThread {
public:
	std::vector<AlgorithmMain> algorithmList;                  // �㷨�����Ķ���
	std::vector<cv::Mat> imageList;                            // ԭʼ����,
	std::vector<cv::Rect> roiList;                             // ��ȡͼ�����
	cv::Mat mask;
public:
	AlgorithmMainForThread() {}
	void operator()();
};

#endif