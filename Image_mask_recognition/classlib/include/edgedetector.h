/*****************************************************************
*
* fileName:  edgedetector.h
*
* function:  ����λͼ�����������
*
* author:    sjt&ztb
*
* date:      2021.1.26
*
* notes:     �������ģ��

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/

#pragma once
#ifndef DETECTOR_H
#define DETECTOR_H

#include "baseFunc.h"

// ��Ĥ����״
enum MASK_SHAPE {
	MASK_SHAPE_MANUAL = 0,                       // �ֶ���Ĥ
	MASK_SHAPE_RECT,                             // ����
	MASK_SHAPE_ROTATED_RECT,                     // ����ת�Ƕȵľ���
	MASK_SHAPE_ELLIPSE,                          // ��Բ
	MASK_SHAPE_SQUARE,                           // ������
	MASK_SHAPE_CIRCLE,                           // Բ
	MASK_SHAPE_ANNULUS,                          // ͬ��Բ
	MASK_SHAPE_POLYGON,                          // �����
	MASK_SHAPE_PILL,                             // Բ�˾��Σ����μ�Բ,(�����߱���Բȡ�������ƽ���)
	MASK_SHAPE_RECT_CHAMFER,                     // Բ�Ǿ��Σ��Ľ�Ϊ1/4Բ
	MASK_SHAPE_CIRCLE_RECT,                      // Բ�߾��Σ�������Բ�Ľ���
	MASK_SHAPE_AUTO                              // ȫ�Զ�ʶ��
};

// ��Ե���ʱ������Ĳ���(cv::Size2f����8byte��MASK_SHAPE����4�ֽ�)
struct EDGE_DETEC_PARAMS {
	bool isUseScale = true;                      // �Ƿ�ʹ�ñ������ӽ����޸�mask, true:ʹ�ã�false:��ʹ��
	float detecScale = 1.0f;                     // ���κ�Բ���ʱ�����ŵı�������
	float inputRadius = 0.0f;                    // ���Բ����Բ�߾��Ρ�Բ�Ǿ���ʱ���ֶ�����İ뾶�����ظ�����
	float preScale = 0.98f;                      // Ԥ�����������     
	MASK_SHAPE maskShape = MASK_SHAPE_MANUAL;    // �����׵���״
	cv::Size2f rectSize = cv::Size2f(0, 0);      // �ֶ������ͼ�εĿ�͸ߡ�����Ͷ��ᡢ�ߵ�Բ�ĵľ���ȣ������ظ�����
};

// ��Ե�����
class EdgeDetector {
public:
	bool isSingleHole = false;                   // ���ױ�־��true�����ף�false:���
	int detecRadius = 0;                         // ���������С��ΧԲ�İ뾶�����ظ�����
	cv::Mat detecMask = cv::Mat();               // �������mask
	cv::Mat edgeMask = cv::Mat();                // �������mask��Ե������ǰ��
	cv::Rect detecSquare = cv::Rect(0, 0, 0, 0); // ���������С��ΧԲ����Ӿ���
	cv::Point detecCenter = cv::Point(0, 0);     // ���������С��ΧԲ��Բ��
	ERROR_TYPE errorType = ERROR_TYPE_NOT_ERROR; // ��Ե���ʱ���ֵĴ���
	EDGE_DETEC_PARAMS edgeDetecParams;           // ��Ե���ʱ���������ز���

public:
	/**
	 * @brief EdgeDetector                       ��ʼ��������������ز���
	 * @param inputParams                        ��Ե���ʱ����Ĳ���
	 */
	EdgeDetector(EDGE_DETEC_PARAMS inputParams) : edgeDetecParams(inputParams) { }

	/**
	 * @brief EdgeDetectorProcess                ��Ե��⣬���ҵ�mask����С��ΧԲ����Ӿ�������
	 * @param srcs  CV_8UC1 CV32FC1              ͼ��
	 * @param mask                               ���ɵ�mask
	 * @note
	 */
	void EdgeDetectorProcess(const std::vector<cv::Mat> srcs,
		const cv::Mat& mask);

	/**
	 * @brief getMaskEdge                        ��ȡ��Ĥ�ı�Ե��Ĭ�Ϻ�����
	 * @param retMask                            ������Ĥ
	 * @param maskEdge                           ��Ĥ��Ե
	 * @param roiMask                            ��Ĥ
	 * @param roi                                λ����Ϣ
	 */
	void getMaskEdge(cv::Mat &retMask, cv::Mat &maskEdge, const cv::Mat roiMask, cv::Rect roi);

private:
	/**
	* @brief GetAllContours                      ����ͼ�����ͼ�������
	* @param allcontours                         ��⵽����������
	* @param _src0 CV_8UC1 CV32FC1               ͼ��0
	* @param _src2 CV_8UC1 CV32FC1               ͼ��2
	* @note  sub02 = abs(_src0 - _src2)
	*/
	void GetAllContours(std::vector<cv::Point>& allcontours,
		const cv::Mat& _src0,
		const cv::Mat& _src2);

	/**
	* @brief GetAllContours                      �ķ�ͼ�����ͼ�������
	* @param allcontours                         ��⵽����������
	* @param _src0 CV_8UC1 CV32FC1               ͼ��0
	* @param _src1 CV_8UC1 CV32FC1               ͼ��1
	* @param _src2 CV_8UC1 CV32FC1               ͼ��2
	* @param _src3 CV_8UC1 CV32FC1               ͼ��3
	* @note  add02_13 = (abs(_src0 - _src2) + abs(_src1 - _src3)) / 2
	*/
	void GetAllContours(std::vector<cv::Point>& allcontours,
		const cv::Mat& _src0,
		const cv::Mat& _src1,
		const cv::Mat& _src2,
		const cv::Mat& _src3);

	// �ķ�ͼ�����ͼ�������
	std::vector<std::vector<cv::Point>> GetAllContours2(std::vector<cv::Point> &allcontours,
		const cv::Mat& _src0,
		const cv::Mat& _src1,
		const cv::Mat& _src2,
		const cv::Mat& _src3);

	// ����mask�������С��ΧԲ
	void UnitCircle();

	/**
	 * @brief UnitCircle                         ����mask�������С��ΧԲ
	 * @param center                             Բ��
	 * @param radius                             �뾶
	 * @param mask                               mask
	 * @return none
	 * @note                                     circle(����֤Բ��mask��С��)
	 */
	void UnitCircle(cv::Point& center,
		int& radius,
		const cv::Mat& mask);

	/**
	 * @brief DetectCircle                       ���Բ���õ���С��ΧԲ��Բ�ĺͰ뾶
	 * @param center                             Բ��
	 * @param radius                             �뾶
	 * @param _src0 CV_8UC1 CV32FC1              ͼ��0
	 * @param _src1 CV_8UC1 CV32FC1              ͼ��1
	 * @param _src2 CV_8UC1 CV32FC1              ͼ��2
	 * @param _src3 CV_8UC1 CV32FC1              ͼ��3
	 */
	bool DetectCircle(cv::Point2f& center,
		float& radius,
		const cv::Mat& _src0,
		const cv::Mat& _src1,
		const cv::Mat& _src2 = cv::Mat(),
		const cv::Mat& _src3 = cv::Mat());

	/**
	 * @brief DetectAnnulus                      ���ͬ��Բ���õ���С��ΧԲ��Բ�ġ��뾶��mask
	 * @param center                             Բ��
	 * @param radius                             �뾶
	 * @param _src0 CV_8UC1 CV32FC1              ͼ��0
	 * @param _src1 CV_8UC1 CV32FC1              ͼ��1
	 * @param _src2 CV_8UC1 CV32FC1              ͼ��2
	 * @param _src3 CV_8UC1 CV32FC1              ͼ��3
	 */
	bool DetectAnnulus(cv::Point2f &center,
		float &radius,
		const cv::Mat& _src0,
		const cv::Mat& _src1,
		const cv::Mat& _src2,
		const cv::Mat& _src3);

	/**
	 * @brief DetectPolygons                     ������Σ����õ���С��ΧԲ��Բ�ġ��뾶��mask
	 * @param center                             Բ��
	 * @param radius                             �뾶
	 * @param _src0 CV_8UC1 CV32FC1              ͼ��0
	 * @param _src1 CV_8UC1 CV32FC1              ͼ��1
	 * @param _src2 CV_8UC1 CV32FC1              ͼ��2
	 * @param _src3 CV_8UC1 CV32FC1              ͼ��3
	 */
	bool DetectPolygons(cv::Point2f &center,
		float &radius,
		const cv::Mat& _src0,
		const cv::Mat& _src1,
		const cv::Mat& _src2,
		const cv::Mat& _src3);

	/**
	 * @brief DetectRect                         ���������Ĵ�ֱ�߽���С���Σ���������ͼ�����±߽�ƽ�еģ�
	 * @param rect                               ��Ӿ���
	 * @param _src0 CV_8UC1 CV32FC1              ͼ��0
	 * @param _src1 CV_8UC1 CV32FC1              ͼ��1
	 * @param _src2 CV_8UC1 CV32FC1              ͼ��2
	 * @param _src3 CV_8UC1 CV32FC1              ͼ��3
	 * @note                                     �õ���Ӿ��ε����ϵ����꼰���ο��
	 */
	bool DetectRect(cv::Rect& rect,
		const cv::Mat& _src0,
		const cv::Mat& _src1,
		const cv::Mat& _src2,
		const cv::Mat& _src3);

	/**
	 * @brief DetectRotatedRect                  ��������㼯��С����ľ��Σ�������ƫת�Ƕȵģ�������ͼ��ı߽粻ƽ��
	 * @param rotatedrect                        ��Ӿ���
	 * @param _src0 CV_8UC1 CV32FC1              ͼ��0
	 * @param _src1 CV_8UC1 CV32FC1              ͼ��1
	 * @param _src2 CV_8UC1 CV32FC1              ͼ��2
	 * @param _src3 CV_8UC1 CV32FC1              ͼ��3
	 * @note                                     �õ���Ӿ��ε����ĵ�����꣬��͸ߣ��Լ���ת�Ƕ�
	 */
	bool DetectRotatedRect(cv::RotatedRect& rotatedrect,
		const cv::Mat& _src0,
		const cv::Mat& _src1,
		const cv::Mat& _src2,
		const cv::Mat& _src3);

	/**
	 * @brief DetectEllipse                      ��������㼯��С����ľ��Σ�������ƫת�Ƕȵģ�������ͼ��ı߽粻ƽ��
	 * @param ellipse                            ��Բ�Ĳ���
	 * @param _src0 CV_8UC1 CV32FC1              ͼ��0
	 * @param _src1 CV_8UC1 CV32FC1              ͼ��1
	 * @param _src2 CV_8UC1 CV32FC1              ͼ��2
	 * @param _src3 CV_8UC1 CV32FC1              ͼ��3
	 * @note                                     �õ���Ӿ��ε����ĵ�����꣬��͸ߣ��Լ���ת�Ƕ�
	 */
	bool DetectEllipse(cv::RotatedRect& ellipse,
		const cv::Mat& _src0,
		const cv::Mat& _src1,
		const cv::Mat& _src2,
		const cv::Mat& _src3);

	// �����û����õĲ������޸�mask�Ĵ�С
	void changeMaskRect();                                    // ����
	void changeMaskRatatedRect(cv::RotatedRect& rotatedRect); // ��ת����
	void changeMaskCircle(float fRadius);                     // Բ
	void changeMaskEllipse(cv::RotatedRect& ellipse);         // ��Բ
	void changeMaskPill(cv::RotatedRect& pill);               // ����
	void changeMaskChamfer(cv::RotatedRect& rotatedRect);     // Բ�Ǿ���
	void changeMaskCircleRect(cv::RotatedRect& rotatedRect);  // Բ�߾���

}; // class 

#endif     // DETECTOR_H
