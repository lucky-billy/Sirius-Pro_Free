/*****************************************************************
*
* fileName:  baseFunc.h
*
* function:  ��Ż�������
*
* author:    sjt&ztb
*
* date:      2021.1.26
*
* notes:     ��Ż�������ģ��

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/

#pragma once
#ifndef BASEFUNC_H
#define BASEFUNC_H

#include <opencv2/opencv.hpp>
#include <fstream>
#include <sstream>
#include <opencv2/imgproc/imgproc.hpp>    
#include <opencv2/highgui/highgui.hpp> 
#include <iostream>
#include <algorithm>
#include <vector>

#define SQRT_1  1.00000000f
#define SQRT_2  1.41421356f
#define SQRT_3  1.73205081f
#define SQRT_4  2.00000000f
#define SQRT_5  2.23606798f
#define SQRT_6  2.44948974f
#define SQRT_7  2.64575131f
#define SQRT_8  2.82842712f
#define SQRT_9  3.00000000f
#define SQRT_10  3.16227766f
#define SQRT_11  3.31662479f
#define SQRT_12  3.46410162f
#define SQRT_13  3.60555128f
#define SQRT_14  3.74165739f
#define SQRT_15  3.87298335f
#define SQRT_16  4.00000000f
#define SQRT_17  4.12310563f
#define SQRT_18  4.24264069f
#define SQRT_19  4.35889894f
#define SQRT_20  4.47213595f
#define SQRT_21  4.58257569f
#define SQRT_22  4.69041576f
#define SQRT_23  4.79583152f
#define SQRT_24  4.89897949f
#define SQRT_25  5.00000000f
#define SQRT_26  5.09901951f
#define SQRT_27  5.19615242f
#define SQRT_28  5.29150262f
#define SQRT_29  5.38516481f
#define SQRT_30  5.47722558f

#define Min(a, b) a < b ? a : b
#define Max(a, b) a > b ? a : b

#define RAD_TO_ANGLE  57.29577951f               // ����ת��Ϊ�Ƕȵ�ϵ��
#define PI            3.14159265358979f          // pi
#define PI_2          6.28318530717959f          // 2pi

// ��ӡ����
#define TURN_ON_DEBUG                            // ��ӡ�ܿ���
#ifdef TURN_ON_DEBUG
#define TURN_ON_DEBUG_VAR                    // ��ӡ�����Ŀ���
#define TURN_ON_DEBUG_STR                    // ��ӡ�ַ����Ŀ���
#define TURN_ON_DEBUG_DETAILS                // ��ӡ��������ϸ��Ϣ�Ŀ���
// ��ӡ����
#ifdef TURN_ON_DEBUG_VAR
	#define DEBUG(_PARAMS_) std::cout << "func: " << __FUNCTION__ << ", " << #_PARAMS_ << " = " << _PARAMS_<< std::endl;
#else
	#define DEBUG(_PARAMS_);
#endif
// ��ӡ�ַ���
#ifdef TURN_ON_DEBUG_STR
	#define DEBUG_STR(_PARAMS_) std::cout << "algorithm: " << _PARAMS_ << std::endl;
#else
	#define DEBUG_STR(_PARAMS_);
#endif
// ��ӡ��������ϸ��Ϣ
#ifdef TURN_ON_DEBUG_DETAILS
	#define DEBUG_DETAILS(_PARAMS_) std::cout << "file: " << __FILE__ << ", line: " <<  __LINE__ << ", func: " << __FUNCTION__ << ", " << #_PARAMS_ << " = " << _PARAMS_<< std::endl;
#else
	#define DEBUG_DETAILS(_PARAMS_);
#endif
#else
	#define DEBUG(_PARAMS_);
	#define DEBUG_STR(_PARAMS_);
	#define DEBUG_DETAILS(_PARAMS_);
#endif

// ��������
enum ERROR_TYPE {
	ERROR_TYPE_NOT_ERROR = 0,
	ERROR_TYPE_NO_ENOUGH_IMAGES,
	ERROR_TYPE_NO_MASK,
	ERROR_TYPE_ROI_TOO_SMALL,
	ERROR_TYPE_PSI_ERROR,
	ERROR_TYPE_UNWRAP_ERROR,
	ERROR_TYPE_ZERNIKE_ERROR,
	ERROR_TYPE_CHECK,
	ERROR_TYPE_SPIKES_TOO_MUCH,
	ERROR_TYPE_NOT_AUTOMASK,
	ERROR_TYPE_PIC_SIZE,
	ERROR_TYPE_NO_RESULT
};

// check���ܵ��ж���ֵ
struct CHECK_THR {
	int phaseShiftThr = 10;                       // ����ƫ��Ƕ���ֵ
	float stdPhaseHistThr = 0.01f;               // ���ƽǶ�ֱ��ͼ�ı�׼����ֵ
	float resPvThr = 0.095f;                     // �в�pv����ֵ
	float resRmsThr = 0.018f;                    // �в�rms����ֵ
};

// ��������
class BaseFunc {
public:
	/*--------------------- edgeDetector -------------------------*/
	/**
	 * @brief UnitCart                         ����ָ����Χ�ڵ�ָ�������������������ݿ����ͬ
	 * @param squaresize                       ���εĳߴ磨�����Σ�
	 * @param x return CV_32FC1                �з��������
	 * @param y                                �з��������
	 */
	static void UnitCart(int squaresize, cv::Mat& x, cv::Mat& y) {
		CV_Assert(squaresize % 2 == 1);
		x.create(squaresize, squaresize, CV_32FC1);
		y.create(squaresize, squaresize, CV_32FC1);
		//���ñ߽�
		x.col(0).setTo(-1.0);
		x.col(squaresize - 1).setTo(1.0f);
		y.row(0).setTo(1.0);
		y.row(squaresize - 1).setTo(-1.0f);

		float delta = 2.0f / (squaresize - 1.0f);  //����Ԫ�صļ��

		//��������λ�õ�ֵ
		for (int i = 1; i < squaresize - 1; ++i) {
			x.col(i) = -1.0f + i * delta;
			y.row(i) = 1.0f - i * delta;
		}
	}

	/**
	 * @brief UnitPolar                        ֱ������ת��Ϊ������
	 * @param x                                UnitCart����õ���x
	 * @param y                                UnitCart����õ���y
	 * @param mag                              ��ֵ
	 * @param ang                              �Ƕ�
	 * @param indegree                         ��־λ��false�����ȣ�true:�Ƕȣ�
	 */
	static void UnitPolar(const cv::Mat& x, const cv::Mat& y,
		cv::Mat& mag, cv::Mat& ang, bool indegree = false) {
		cv::cartToPolar(x, y, mag, ang, indegree);   //ֱ������ת��Ϊ������
	}

	/**
	 * @brief UnitPolar                        ֱ������ת��Ϊ������
	 * @param squaresize                       ���εĳߴ磨�����Σ�
	 * @param mag                              ��ֵ
	 * @param ang                              �Ƕ�
	 * @param indegree                         ��־λ��false�����ȣ�true:�Ƕȣ�
	 */
	static void UnitPolar(int squaresize, cv::Mat& mag,
		cv::Mat& ang, bool indegree = false) {
		cv::Mat x;
		cv::Mat y;
		UnitCart(squaresize, x, y);                //����ָ����Χ�ڵ�ָ�������������������ݿ����ͬ
		cv::cartToPolar(x, y, mag, ang, indegree); //����ת��
	}

	/**
	* @brief RectToChild                       ��þ���
	* @param r                                 ���εĳߴ磨�����Σ�
	* @param roi                               ��ֵ
	*/
	static cv::Rect RectToChild(const cv::Rect& r, const cv::Rect& roi) {
		return cv::Rect(r.x - roi.x, r.y - roi.y, r.width, r.height);
	}

	/**
	 * @brief UnitCircleSquare                 ����Բ����Ӿ���
	 * @param center                           UnitCircle �õ��� center
	 * @param radius                           UnitCircle �õ��� radius
	 * @return Rect                            ��Ӿ���
	 * @note                                   ����֤rect��mask��С��
	 */
	static cv::Rect UnitCircleSquare(const cv::Point& center, int& radius) {
		return cv::Rect(center.x - radius, center.y - radius, radius * 2 + 1, radius * 2 + 1);
	}

	/**
	 * @brief UnitRectCircleSquare             ������ε���С��ΧԲ����Ӿ���
	 * @param center                           UnitCircle �õ��� center
	 * @param radius                           UnitCircle �õ��� radius
	 * @return Rect                            ��Ӿ����Լ�Բ�ĺͰ뾶
	 * @note                                   ����֤rect��mask��С��
	 */
	static cv::Rect UnitRectCircleSquare(cv::Rect rect, cv::Point& center, int& radius) {
		center.x = (int)(rect.x + rect.width / 2.0f);
		center.y = (int)(rect.y + rect.height / 2.0f);
		radius = (int)(std::sqrt(rect.width * rect.width + rect.height * rect.height) / 2.0f);
		return cv::Rect(center.x - radius, center.y - radius, radius * 2 + 1, radius * 2 + 1);
	}

	/**
	 * @brief UnitRotatedRectCircleSquare      ������ת���ε���С��ΧԲ����Ӿ���
	 * @param center                           UnitCircle �õ��� center
	 * @param radius                           UnitCircle �õ��� radius
	 * @return Rect                            ��Ӿ����Լ�Բ�ĺͰ뾶
	 * @note                                   ����֤rotatedRect��mask��С��
	 */
	static cv::Rect UnitRotatedRectCircleSquare(
		cv::RotatedRect rotatedRect, cv::Point& center, int& radius) {
		center.x = (int)rotatedRect.center.x;
		center.y = (int)rotatedRect.center.y;
		radius = (int)(std::sqrtf(rotatedRect.size.width * rotatedRect.size.width
			+ rotatedRect.size.height * rotatedRect.size.height) / 2.0f);
		return cv::Rect(center.x - radius, center.y - radius, radius * 2 + 1, radius * 2 + 1);
	}

	/**
	 * @brief UnitRotatedRectCircleSquare      ������ת���ε���С��ΧԲ����Ӿ���
	 * @param center                           UnitCircle �õ��� center
	 * @param radius                           UnitCircle �õ��� radius
	 * @param chamferRadius                    ���Ǿ��εĵ��ǰ뾶
	 * @return Rect                            ��Ӿ���
	 * @note                                   ����֤rotatedRect��mask��С��
	 */
	static cv::Rect UnitChamferRectCircleSquare(
		cv::RotatedRect rotatedRect, cv::Point& center,
		int& radius, float chamferRadius) {
		center.x = (int)rotatedRect.center.x;
		center.y = (int)rotatedRect.center.y;

		float newWidth = rotatedRect.size.width - chamferRadius * 2.0f;
		float newHeight = rotatedRect.size.height - chamferRadius * 2.0f;

		radius = (int)(std::sqrtf(newWidth * newWidth + newHeight * newHeight) / 2.0f + chamferRadius * 2.0f);
		return cv::Rect(center.x - radius, center.y - radius, radius * 2 + 1, radius * 2 + 1);
	}

	/*--------------------- zernike -------------------------*/
	/**
	 * @brief GridSampling                     ���ò�����λ�õ�
	 * @param size                             �ز����ķ�Χ�ߴ�
	 * @param rowinterval                      �м��
	 * @param colinterval                      �м��
	 * @return                                 �ز�����mask
	 */
	static cv::Mat GridSampling(const cv::Size& size, int rowinterval, int colinterval) {
		cv::Mat dst(size, CV_8UC1, cv::Scalar(0));
		//���ò�����λ�õ�
		for (int row = 0; row < dst.rows; row += rowinterval) {
			for (int col = 0; col < dst.cols; col += colinterval) {
				dst.at<uchar>(row, col) = 255;
			}
		}
		return dst;
	}

	//����ת������ʱ�ļ�������λ
	static cv::Mat InitialAng129() {
		cv::Mat ang;
		cv::Mat mag;
		UnitPolar(129, mag, ang);
		return ang;
	}

	//����ת������ʱ�ļ��������
	static cv::Mat InitialMag129() {
		cv::Mat ang;
		cv::Mat mag;
		UnitPolar(129, mag, ang);
		return mag;
	}

	//����ת������ʱ�ļ�������λ
	static cv::Mat InitialAng259() {
		cv::Mat ang;
		cv::Mat mag;
		UnitPolar(259, mag, ang);
		return ang;
	}

	//����ת������ʱ�ļ��������
	static cv::Mat InitialMag259() {
		cv::Mat ang;
		cv::Mat mag;
		UnitPolar(259, mag, ang);
		return mag;
	}

	/*--------------------- other functions -------------------------*/
	/**
	* @brief MorphOpenMask                     Ϊ����������С�㣬ë�̺�С��Ӱ�죬��mask���п�����
	* @param mask                              mask
	* @param shape                             ��״
	* @param times                             ��������
	* @return
	*/
	static void MorphOpenMask(cv::Mat &mask, int shape, int size, int times)
	{
		//���shape�Σ���СΪsize*size��ģ��
		cv::Mat element = cv::getStructuringElement(shape, cv::Size(size, size));
		//���п����㣬����times��
		morphologyEx(mask, mask, cv::MORPH_OPEN, element, cv::Point(-1, -1), times);
	}

	// ����pv
	static float getPv(cv::Mat src) {
		double minv = 0;
		double maxv = 0;
		float pv = 0.0f;
		cv::minMaxIdx(src, &minv, &maxv, nullptr, nullptr, src == src);
		pv = (float)(maxv - minv);
		return pv;
	}

	// ����rms
	static float getRms(cv::Mat src) {
		cv::Mat tmp;
		float rms = 0.0f;
		cv::pow(src, 2.0, tmp);
		float mean_tmp = (float)(cv::mean(tmp, tmp == tmp)[0]);     // �����ֵ(nan != nan)
		rms = std::sqrt(mean_tmp);          //�õ�rms

		return rms;
	}

	// �����׼��
	static  float getStdDev(std::vector<float> input) {
		float sum = 0.0f;
		std::for_each(std::begin(input), std::end(input), [&](const float d) {
			sum += d;
		});
		float mean = sum / input.size(); //��ֵ
		float accum = 0.0f;
		std::for_each(std::begin(input), std::end(input), [&](const float d) {
			accum += (d - mean)*(d - mean);
		});
		float stdev = sqrt(accum / (input.size()-1)); 
		return stdev;
	}

	// ����2sigma
	static float getTwoSigma(std::vector<float> input) {
		float std = getStdDev(input);
		return (2 * std);
	}

	/**
	* @brief CalcTheta                         ����x or y����ĽǶ�
	* @param z1                                ������棬mask1��4��zernikeϵ������б�x:z2, y:z3��
	* @param z2                                �ο����棬mask2��4��zernikeϵ������б�x:z2, y:z3��
	* @param aperture1                         mask1�Ŀھ�(mm)
	* @param aperture2                         mask2�Ŀھ�(mm)
	* @param refractiveIndex                   ������
	* @param testWaveLength                    ���Բ�����nm��
	* @return                                  x or y����ĽǶ�(��)
	*/
	static float CalcTheta(float z1, float z2, float aperture1,
		float aperture2, float refractiveIndex, float testWaveLength) {
		if (refractiveIndex == 1.0f || aperture1 == 0.0f || aperture2 == 0.0f) {
			return 0.0f;
		}
		float retTheta = 0.0f;
		float tmp = z1 / aperture1 / (refractiveIndex - 1.0f) - 2.0f * z2 / aperture2;
		// retTheta = 410.0544f * tmp / PI / 2.0f / (refractiveIndex - 1.0f); // testWaveLength == 632.8
		// retTheta = 3.6f * testWaveLength  * 0.18f * tmp / PI / 2.0f / (refractiveIndex - 1.0f);
		// retTheta = RAD_TO_ANGLE * testWaveLength  * 0.0036f * tmp / 2.0f / (refractiveIndex - 1.0f);
		retTheta = RAD_TO_ANGLE * testWaveLength  * 0.0036f * tmp / (refractiveIndex - 1.0f);
		return retTheta;
	} 

	/**
	* @brief CalcParallelDegree                ����ƽ�ж�
	* @param zernikeCoef4_1                    ������棬mask1��4��zernikeϵ����x:z2, y:z3��
	* @param zernikeCoef4_2                    �ο����棬mask2��4��zernikeϵ����x:z2, y:z3��
	* @param aperture1                         mask1�Ŀھ�(mm)
	* @param aperture2                         mask2�Ŀھ�(mm)
	* @param refractiveIndex                   ������
	* @param testWaveLength                    ���Բ�����nm��
	* @return                                  ƽ�жȣ���λ���룩
	*/
	static float CalcParallelDegree(cv::Mat zernikeCoef4_1, cv::Mat zernikeCoef4_2,
		float aperture1, float aperture2, float refractiveIndex, float testWaveLength) {
		float retTheta = 0.0f;

		// ����x or y����ĽǶ�
		float theta_x = CalcTheta(zernikeCoef4_1.at<float>(1),
			zernikeCoef4_2.at<float>(1), aperture1, aperture2, refractiveIndex, testWaveLength);
		float theta_y = CalcTheta(zernikeCoef4_1.at<float>(2),
			zernikeCoef4_2.at<float>(2), aperture1, aperture2, refractiveIndex, testWaveLength);

		// ����ƽ�ж�theta
		retTheta = sqrtf(theta_x * theta_x + theta_y * theta_y);
		
		return retTheta;
	}

	/**
	* @brief detecThreeFlat                    ��ƽ���⣨a, b, c��
	* @param xSlice                            �����������귽�����Ƭ(a+b, a+c, b+c, br/cr)@������+�ο��棬br����b��ת180�ȣ�cr=c���ο��治�䣩
	* @param ySlice                            ������������귽�����Ƭ(a+b, a+c, b+c, br/cr)@������+�ο��棬br����b��ת180�ȣ�cr=c���ο��治�䣩
	* @param pv                                ����ƽ���x,y������PV��˳��Ϊ��a,b,c����һ��x,�ڶ���y��
	* @param rms                               ����ƽ���x,y������rms��˳��Ϊ��a,b,c����һ��x,�ڶ���y��
	* @return                                  ����ƽ��ļ�����(��x(abc)����y(abc))
	*/
	static std::vector<cv::Mat> detecThreeFlat(std::vector<cv::Mat>xSlice,
		std::vector<cv::Mat>ySlice, float pv[2][3], float rms[2][3]) {
		CV_Assert(xSlice.size() == 4);
		CV_Assert(ySlice.size() == 4);

		std::vector<cv::Mat> result;

		cv::Mat tmp,rxa,rxb,rxc,rya,ryb,ryc;
		// ����x����Ľ��
		tmp = (xSlice[0] + xSlice[1] - xSlice[3]) / 2.0f;
		rxa = tmp.clone();
		result.emplace_back(rxa);
		pv[0][0] = getPv(rxa);
		rms[0][0] = getRms(rxa);

		tmp = (xSlice[0] + xSlice[3] - xSlice[1]) / 2.0f;
		rxb = tmp.clone();
		result.emplace_back(rxb);
		pv[0][1] = getPv(rxb);
		rms[0][1] = getRms(rxb);

		tmp = (xSlice[3] + xSlice[1] - xSlice[0]) / 2.0f;
		rxc = tmp.clone();
		result.emplace_back(rxc);
		pv[0][2] = getPv(rxc);
		rms[0][2] = getRms(rxc);

		// ����y����Ľ��
		tmp = (ySlice[0] + ySlice[1] - ySlice[2]) / 2.0f;
		rya = tmp.clone();
		result.emplace_back(rya);
		pv[1][0] = getPv(rya);
		rms[1][0] = getRms(rya);

		tmp = (ySlice[0] + ySlice[2] - ySlice[1]) / 2.0f;
		ryb = tmp.clone();
		result.emplace_back(ryb);
		pv[1][1] = getPv(ryb);
		rms[1][1] = getRms(ryb);

		tmp = (ySlice[2] + ySlice[1] - ySlice[0]) / 2.0f;
		ryc = tmp.clone();
		result.emplace_back(ryc);
		pv[1][2] = getPv(ryc);
		rms[1][2] = getRms(ryc);

		/*std::cout << "pv = ";
		for (int i = 0; i < 2; ++i) {
			std::cout << std::endl;
			for (int j = 0; j < 3; ++j) {
				std::cout << pv[i][j] << ", ";
			}
		}

		std::cout << std::endl << "rms = ";
		for (int i = 0; i < 2; ++i) {
			std::cout << std::endl;
			for (int j = 0; j < 3; ++j) {
				std::cout << rms[i][j] << ", ";
			}
		}*/

		return result;
	}
	
	/**
	* @brief Rotate                            ͼ����ת
	* @param srcImage                          ����תͼ��
	* @param dstImage                          ��ת���ͼ��
	* @param angle                             ��ת�Ƕȣ�����Ϊ��ʱ�룬����Ϊ˳ʱ�룩
	* @return 
	*/
	static void Rotate(const cv::Mat &srcImage, cv::Mat &dstImage, double angle)
	{
		cv::Point2f center(srcImage.cols / 2.0f, srcImage.rows / 2.0f);//����
		cv::Mat M = cv::getRotationMatrix2D(center, angle, 1);//������ת�ķ���任���� 
		cv::warpAffine(srcImage, dstImage, M, cv::Size(srcImage.cols, srcImage.rows));//����任  
	}

	/**
    * @brief Rotate2                           ͼ����ת
    * @param srcImage                          ����תͼ��
    * @param dstImage                          ��ת���ͼ��
    * @param angle                             ��ת�Ƕȣ�����Ϊ��ʱ�룬����Ϊ˳ʱ�룩
	* @param center                            ��ת����
    * @return
    */
	static void Rotate2(const cv::Mat &srcImage, cv::Mat &dstImage, double angle,cv::Point2f center)
	{
		cv::Mat M = cv::getRotationMatrix2D(center, angle, 1);//������ת�ķ���任���� 
		cv::warpAffine(srcImage, dstImage, M, cv::Size(srcImage.cols, srcImage.rows));//����任  
	}
	
	/**
	* @brief analysisDoubleSpherSur            ���㱻����ͱ�׼��ľ�����˫������Լ�⣩
	* @param fittedSurface                     �������(M4:è�۵�, M0:������0��, M1:90��, M2:180��, M3:270��)
	* @param pv                                pv(0:������, 1:��׼��)
	* @param rms                               rms(0:������, 1:��׼��)
	* @param testSurError                      ���������
	* @param standSurError                     ��׼�����
	* @return                                  
	*/
	static void analysisDoubleSpherSur(std::vector<cv::Mat> fittedSurface,
		float pv[2], float rms[2], cv::Mat &testSurError, cv::Mat &standSurError) {
		CV_Assert(fittedSurface.size() == 5);

		cv::Mat ret;
		// ��è�۵����˳ʱ����ת
		cv::Mat M4_90, M4_180, M4_270;            // ˳ʱ����ת
		Rotate(fittedSurface[0], M4_90, -90);     // ˳ʱ����ת
		Rotate(fittedSurface[0], M4_180, -180);   // ˳ʱ����ת
		Rotate(fittedSurface[0], M4_270, -270);   // ˳ʱ����ת
		// �Թ����������ʱ����ת
		cv::Mat M1_90, M2_180, M3_270;            // ��ʱ����ת
		Rotate(fittedSurface[2], M1_90, 90);      // ��ʱ����ת
		Rotate(fittedSurface[3], M2_180, 180);    // ��ʱ����ת
		Rotate(fittedSurface[4], M3_270, 270);    // ��ʱ����ת

		// ���㱻��������pv��rms
		testSurError = (fittedSurface[1] + M1_90 + M2_180 + M3_270
			- fittedSurface[0] - M4_90 - M4_180 - M4_270) / 4.0f;
		pv[0] = getPv(testSurError);
		rms[0] = getRms(testSurError);
		// �����׼������pv��rms
		standSurError = fittedSurface[1] - testSurError;
		pv[1] = getPv(standSurError);
		rms[1] = getRms(standSurError);

		/*std::cout << "pv = ";
		for (int i = 0; i < 2; ++i) {
			std::cout << pv[i] << ", ";
		}

		std::cout << std::endl << "rms = ";
		for (int i = 0; i < 2; ++i) {
			std::cout << rms[i] << ", ";
		}
		std::cout << std::endl;*/

		return;
	}

	/**
	* @brief calcOpticalHomogeneity            �����ѧ������
	* @param refractiveIndex                   ������
	* @param thickness				           �������
	* @param unitType                          ��λ
	* @param testWaveLength                    ����
	* @param fittedSurface                     �������(C,T,S1,S2)��ǻ��͸����ǰ���棬�����
	* @param pvh                               pvh(pv���Ժ��)
	* @param rmsh                              rmsh(rms���Ժ��)
	* @param pv                                pv
	* @param rms                               rms
	* @return
	*/
	static int calcOpticalHomogeneity(float refractiveIndex, float thickness, int unitType,
		float testWaveLength, std::vector<cv::Mat> &fittedSurface, float *pvh, float *rmsh,float *pv,float *rms,cv::Mat &result)
	{
	    if (fittedSurface.size() < 4) {
			return 1;                            //������ͼ����ͼƬ��������
		}
		
		cv::Mat C = fittedSurface[0];
		cv::Mat T = fittedSurface[1];
		cv::Mat S1 = fittedSurface[2];
		cv::Mat S2 = fittedSurface[3];

		if (C.empty() || T.empty() || S1.empty() || S2.empty())
			return 2;                            //���ڿ�ͼ

		if (C.size() != T.size() || C.size() != S1.size() || C.size() != S2.size())
			return 3;                            //��С��һ��

		cv::Mat deltaN = cv::Mat();
		deltaN = (refractiveIndex * (T - C) - (refractiveIndex - 1) * (S2 - S1));

		*pv = getPv(deltaN);
		*rms = getRms(deltaN);

		float coef = 1.0f;
		switch (unitType)
		{
		case 0:									 // UNIT_TYPE_WAVE, �Բ���Ϊ��λ
			coef = testWaveLength / 1000000.0f;
			break;
		case 1:									 // UNIT_TYPE_FR, ��FrΪ��λ
			coef = testWaveLength / 2000000.0f;
			break;
		case 2:                                  // UNIT_TYPE_NM, ��nmΪ��λ
			coef = 1 / 1000000.0f;
			break;
		case 3:                                  // UNIT_TYPE_UM, ��umΪ��λ
			coef = 1 / 1000.0f;;
			break;
		case 4:                                  // UNIT_TYPE_MM,��mmΪ��λ:
			coef = 1.0f;
			break;
		default:
			coef = testWaveLength / 1000000.0f;
			break;
		}

		*pvh = *pv * coef / thickness;
		*rmsh = *rms * coef / thickness;
		deltaN.copyTo(result);
		return 0;                               //��ѧ������ģ������
	}

	//����һ��չ������3*3|A|
	static float getA(float arcs[3][3], int n)
	{
		if (n == 1)
		{
			return arcs[0][0];
		}
		float ans = 0;
		float temp[3][3] = { 0.0 };
		int i, j, k;
		for (i = 0; i < n; ++i)
		{
			for (j = 0; j < n - 1; ++j)
			{
				for (k = 0; k < n - 1; ++k)
				{
					temp[j][k] = arcs[j + 1][(k >= i) ? k + 1 : k];

				}
			}

			float t = getA(temp, n - 1);

			if (i % 2 == 0)
			{
				ans += arcs[0][i] * t;
			}
			else
			{
				ans -= arcs[0][i] * t;
			}
		}
		return ans;
	}
	
	//����ÿһ��ÿһ�е�ÿ��Ԫ������Ӧ������ʽ�����A*
	static void getAStart(float arcs[3][3], int n, float ans[3][3])
	{
		if (n == 1)
		{
			ans[0][0] = 1;
			return;
		}
		int i, j, k, t;
		float temp[3][3];
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				for (k = 0; k < n - 1; k++)
				{
					for (t = 0; t < n - 1; t++)
					{
						temp[k][t] = arcs[k >= i ? k + 1 : k][t >= j ? t + 1 : t];
					}
				}

				ans[j][i] = getA(temp, n - 1);  //�˴�˳�������ת��
				if ((i + j) % 2 == 1)
				{
					ans[j][i] = -ans[j][i];
				}
			}
		}
	}
	
	//�õ���������src������󱣴浽des��(3*3)��
	static void GetMatrixInverse(float src[3][3], int n, float des[3][3])
	{
		float value = getA(src, n);
		float t[3][3];

		getAStart(src, n, t);

		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				des[i][j] = t[i][j] / value;
			}
		}
	}

	/**
	* @brief calculator_add                    ��λ���������ӷ�
	* @param inImage1                          ����λ��ͼ1
	* @param inImage2                          ����λ��ͼ2
	* @return                                  ���ͼ
	*/
	static cv::Mat calculator_add(const cv::Mat &inImage1,
		const cv::Mat &inImage2) {
		CV_Assert(inImage1.type() == inImage2.type());
		CV_Assert(inImage1.size() == inImage2.size());
		return inImage1 + inImage2;
	}

	/**
    * @brief calculator_sub                    ��λ������������
    * @param inImage1                          ����λ��ͼ1
	* @param inImage2                          ����λ��ͼ2
	* @return                                  ���ͼ
	*/
	static cv::Mat calculator_sub(const cv::Mat &inImage1,
		const cv::Mat &inImage2) {
		CV_Assert(inImage1.type() == inImage2.type());
		CV_Assert(inImage1.size() == inImage2.size());
		return inImage1 - inImage2;
	}

	/**
	* @brief calculator_mul                    ��λ���������˷�
	* @param inImage                           ����λ��ͼ
	* @param scale                             �˷�ϵ��
	* @return                                  ���ͼ
	*/
    static cv::Mat calculator_mul(const cv::Mat &inImage, const float scale) {
		return scale * inImage;
	}

	/**
	* @brief calculator_rev                    ��λ����������ת
	* @param inImage                           ����λ��ͼ
	* @return                                  ���ͼ
	*/
	static cv::Mat calculator_rev(const cv::Mat &inImage) {
		return -1*inImage;
	}

	/**
	* @brief calculator_rotate                 ��λ����������ת
	* @param inImage                           ����λ��ͼ
	* @param angle                             �Ƕ�
	* @return                                  ���ͼ
	*/
    static cv::Mat calculator_rotate(const cv::Mat &inImage, cv::Mat &mask,const float angle) {
		CV_Assert(inImage.type() == CV_32FC1);
		int left = 10000;
		int up = 10000;
		int right = 0;
		int down = 0;
		int row = inImage.rows;
		int col = inImage.cols;
		for (int i = 0; i < row; ++i)
		{
            uchar *temp = mask.ptr<uchar>(i);
			for (int j = 0; j < col; ++j)
			{
                if (temp[j]!=0)
				{
					if (i < up) up = i;
					if (i > down) down = i;
					if (j < left) left = j;
					if (j > right) right = j;
				}
			}
		}
		cv::Point2i center;
		center.x = (right + left) / 2;
		center.y = (up + down) / 2;
		cv::Mat out = cv::Mat();
		Rotate2(inImage,out,angle,center);
		return out;
	}
};

namespace calc {
	template<typename T, int num>
	/**
	 * @brief foreach_x
	 * @param srcs
	 * @param dst
	 * @param func
	 * @param mask
	 */
	void foreach_x(const std::vector<cv::Mat>& srcs,
		cv::Mat& dst,
		const std::function<T(const T*)>& func,
		const cv::Mat& mask = cv::Mat()) {
		CV_Assert(srcs.size() >= num);
		CV_Assert(srcs[0].elemSize1() == sizeof(T) && srcs[0].channels() == 1);
		CV_Assert(srcs[0].size() == dst.size());
		CV_Assert(srcs[0].type() == dst.type());
		CV_Assert(mask.empty() || srcs[0].size() == mask.size());
		CV_Assert(mask.type() == CV_8UC1);

		if (mask.empty()) {
			dst.forEach<T>([&srcs, &func](T& p, const int* position) {
				T vs[num];
				for (unsigned int i = 0; i != num; ++i) {
					vs[i] = srcs[i].at<T>(position[0], position[1]);
				}
				p = func(vs);
			});
		}
		else {
			dst.forEach<T>([&srcs, &func, &mask](T& p, const int* position) {
				if (mask.at<uchar>(position[0], position[1]) != 0) {
					T vs[num];
					for (unsigned int i = 0; i != num; ++i) {
						vs[i] = srcs[i].at<T>(position[0], position[1]);
					}
					p = func(vs);
				}
			});
		}
	}

	template<typename T, int num>
	/**
	 * @brief foreach_x
	 * @param srcs
	 * @param func
	 * @param initialvalue
	 * @param mask
	 * @return
	 */
	cv::Mat foreach_x(const std::vector<cv::Mat>& srcs,
		const std::function<T(const T*)>& func,
		T initialvalue = 0,
		const cv::Mat& mask = cv::Mat()) {
		cv::Mat dst(srcs[0].size(), srcs[0].type(), initialvalue);
		foreach_x<T, num>(srcs, dst, func, mask);
		return dst;
	}

	/**
	 * @brief  bitwise_and()ͬ &,  opencv�Դ��İ�λ��ĺ�����
	 * @param  src1                ����ͼ��1
	 * @param  src2                ����ͼ��2
	 * @param  mask                ��Ĥ
	 * @return dst                 �������� dst = (src1 & src2) && mask != 0
	 */
	inline cv::Mat bitwise_and(cv::InputArray src1, cv::InputArray src2,
		cv::InputArray mask = cv::noArray()) {
		cv::Mat dst;
		cv::bitwise_and(src1, src2, dst, mask);
		return dst;
	}

	/**
	 * @brief  bitwise_or()ͬ |,   opencv�Դ��İ�λ��ĺ�����
	 * @param  src1                ����ͼ��1
	 * @param  src2                ����ͼ��2
	 * @param  mask                ��Ĥ
	 * @return dst                 �������� dst = (src1 | src2) && mask != 0
	 */
	inline cv::Mat bitwise_or(cv::InputArray src1, cv::InputArray src2,
		cv::InputArray mask = cv::noArray()) {
		cv::Mat dst;
		cv::bitwise_or(src1, src2, dst, mask);
		return dst;
	}

	/**
	 * @brief  bitwise_xor()ͬ ^,  opencv�Դ��İ�λ���ĺ�����0��0=0��1��0=1��0��1=1��1��1=0(ͬΪ0����Ϊ1)
	 * @param  src1                ����ͼ��1
	 * @param  src2                ����ͼ��2
	 * @param  mask                ��Ĥ
	 * @return dst                 �������� dst = (src1 ^ src2) && mask != 0
	 */
	inline cv::Mat bitwise_xor(cv::InputArray src1, cv::InputArray src2,
		cv::InputArray mask = cv::noArray()) {
		cv::Mat dst;
		cv::bitwise_xor(src1, src2, dst, mask);
		return dst;
	}

	/**
	 * @brief  bitwise_not ͬ ~,   opencv�Դ��ķǺ�����
	 * @param  src                 ����ͼ��
	 * @param  mask                ��Ĥ
	 * @return dst                 �������� dst = (~src) && mask != 0
	 */
	inline cv::Mat bitwise_not(cv::InputArray src,
		cv::InputArray mask = cv::noArray()) {
		cv::Mat dst;
		cv::bitwise_not(src, dst, mask);
		return dst;
	}

	/**
	 * @brief  sqrt,               ���������ÿ��Ԫ�ص�ƽ������
	 * @param  src                 ����ͼ��
	 * @return dst                 �������� dst = sqrt(src)
	 */
	inline cv::Mat sqrt(cv::InputArray src) {
		cv::Mat dst;
		cv::sqrt(src, dst);
		return dst;
	}

	/**
	 * @brief  pow,                �Ծ�����Ԫ��ȡpower����,��powerΪ������ֱ�ӽ��������㣻
	 *                             �����ȼ���ԭ����ľ���ֵ���ٽ���power����
	 * @param  src                 ����ͼ��
	 * @param  power               power����
	 * @return dst                 �������� dst = (src)^power
	 */
	inline cv::Mat pow(cv::InputArray src, double power) {
		cv::Mat dst;
		cv::pow(src, power, dst);
		return dst;
	}

	/**
	 * @brief  exp,                ��src������Ԫ�ص�ָ���������������dst��
	 * @param  src                 ����ͼ��
	 * @return dst                 ������,�� dst = e^src
	 */
	inline cv::Mat exp(cv::InputArray src) {
		cv::Mat dst;
		cv::exp(src, dst);
		return dst;
	}

	/**
	 * @brief  sinf,               ��src������Ԫ�ص����ң������������dst��
	 * @param  src                 ����ͼ��
	 * @return dst                 ������,�� dst = sin(src)
	 */
	inline cv::Mat sinf(const cv::Mat& src) {
		CV_Assert(src.type() == CV_32FC1);
		cv::Mat dst(src.size(), src.type());

		int cols = src.cols;
		int rows = src.rows;

		//����boolֵ���жϴ洢�Ƿ�������
		if (src.isContinuous() && dst.isContinuous())
		{
			cols *= rows;
			rows = 1;
		}
		//����ÿ��Ԫ�ص�sin()
		for (int i = 0; i < rows; i++)
		{
			const float* srci = src.ptr<float>(i);
			float* dsti = dst.ptr<float>(i);
			for (int j = 0; j < cols; j++) {
				dsti[j] = std::sinf(srci[j]);
			}
		}
		return dst;
	}

	/**
	 * @brief  cosf,               ��src������Ԫ�ص����ң������������dst��
	 * @param  src                 ����ͼ��
	 * @return dst                 ������,�� dst = cos(src)
	 */
	inline cv::Mat cosf(const cv::Mat& src) {
		CV_Assert(src.type() == CV_32FC1);
		cv::Mat dst(src.size(), src.type());

		int cols = src.cols;
		int rows = src.rows;

		//����boolֵ���жϴ洢�Ƿ�������
		if (src.isContinuous() && dst.isContinuous())
		{
			cols *= rows;
			rows = 1;
		}
		//����ÿ��Ԫ�ص�cos()
		for (int i = 0; i < rows; i++)
		{
			const float* srci = src.ptr<float>(i);
			float* dsti = dst.ptr<float>(i);
			for (int j = 0; j < cols; j++) {
				dsti[j] = std::cosf(srci[j]);
			}
		}
		return dst;
	}
	
	/**
	 * @brief  expf_cx,            ��exp(src)(��ͨ��)����һͨ��Ϊʵ�����ڶ�����Ϊ�鲿
	 * @param  src                 ����ͼ��
	 * @return dst                 ������,�� dst = exp(src[0] + j * src[1])
	 */
	inline cv::Mat expf_cx(const cv::Mat& src) {
		CV_Assert(src.type() == CV_32FC2);
		cv::Mat dst(src.size(), src.type(), 0.0);

		int cols = src.cols;
		int rows = src.rows;

		if (src.isContinuous() && dst.isContinuous())
		{
			cols *= rows;
			rows = 1;
		}
		for (int i = 0; i < rows; i++)
		{
			const cv::Vec2f* srci = src.ptr<cv::Vec2f>(i);
			cv::Vec2f* dsti = dst.ptr<cv::Vec2f>(i);
			for (int j = 0; j < cols; j++) {
				std::complex<float> v(srci[j][0], srci[j][1]);
				std::complex<float> r = std::exp(v);
				dsti[j] = cv::Vec2f(r.real(), r.imag());
			}
		}
		return dst;
	}

	/**
	 * @brief  SetToNan,           ��src�еķ�mask��������ΪNaN
	 * @param  src                 ����ͼ��
	 * @return mask                mask
	 */
	inline void SetToNan(cv::Mat& src, const cv::Mat& mask) {
		CV_Assert(src.type() == CV_32FC1);
		cv::Mat _nan(src.size(), src.type(), nan(""));
		_nan.setTo(0, ~mask);
		src = src + _nan;
	}

	/**
	 * @brief  find,               �������������������������������idx��
	 * @param  condition           ��������
	 * @return idx                 ������,
	 */
	inline void find(const cv::Mat& condition,
		std::vector<cv::Point>& idx) {
		cv::findNonZero(condition, idx);  //Returns the list of locations of non-zero pixels
	}

	/**
	 * @brief  find_nan,           ��������ΪNaN��Ԫ�أ���������������idx�У�(NaN != NaN) = true
	 * @param  src                 ��������
	 * @return idx                 ������,
	 */
	inline void find_nan(const cv::Mat& src,
		std::vector<cv::Point>& idx) {
		cv::findNonZero(~(src == src), idx);  //Returns the list of locations of non-zero pixels
	}

	/**
	 * @brief  find_notnan,        �������в�ΪNaN��Ԫ�أ���������������idx�У�(NaN == NaN) = false
	 * @param  src                 ��������
	 * @return idx                 ������,
	 */
	inline void find_notnan(const cv::Mat& src,
		std::vector<cv::Point>& idx) {
		cv::findNonZero(src == src, idx);  //Returns the list of locations of non-zero pixels
	}

	template <typename T>
	/**
	 * @brief  get in a col         ��src�У�idx����ת��Ϊһ���о���
	 * @param  src                  ��������
	 * @param  idx                  ���������������
	 * @return dst                  �о���
	 */
	inline cv::Mat get(const cv::Mat& src,
		const std::vector<cv::Point>& idx) {
		CV_Assert(cv::DataType<T>::type == src.type());
		int num = (int)idx.size();
		cv::Mat dst(num, 1, src.type());

		/* pragma omp parallel for ��OpenMP�е�һ��ָ�
		��ʾ��������forѭ���������߳�ִ�У�����ÿ��ѭ��֮�䲻���й�ϵ */
#pragma omp parallel for
		for (int i = 0; i < num; ++i) {
				dst.at<T>(i, 0) = src.at<T>(idx[i]);
		}
		return dst;
	}

	/**
	 * @brief  get_notnan in a col  �����в�ΪNaN��Ԫ��ת��Ϊһ��������
	 * @param  src CV_32FC1         ��������
	 * @return dst                  ������
	 */
	inline cv::Mat get_notnan(const cv::Mat& src) {
		CV_Assert(src.type() == CV_32FC1);
		std::vector<cv::Point> idx;

		//�������в�ΪNaN��Ԫ�أ���������������idx��
		find_notnan(src, idx);
		cv::Mat dst((int)(idx.size()), 1, src.type());
		for (int i = 0; i != idx.size(); ++i) {
			dst.at<float>(i, 0) = src.at<float>(idx[i]); //ת��Ϊ������
		}
		return dst;
	}

	// ��ֱ��ͼ
	inline void drawHistImg(cv::Mat &hist)
	{
		cv::Mat histImage = cv::Mat::zeros(540, 720, CV_8UC1);
		const int bins = 180;
		double maxValue;
		cv::Point2i maxLoc;
		cv::minMaxLoc(hist, 0, &maxValue, 0, &maxLoc);
		int scale = 4;
		int histHeight = 540;

		/*std::string name_ang = std::to_string(maxLoc.y);
		std::string name_unit = "��";
		std::string name = "���ֵΪ��";
		name.append(name_ang);
		name.append(name_unit);*/

		for (int i = 0; i < bins; i++)
		{
			float binValue = (float)(hist.at<int>(i));
			int height = cvRound(binValue * histHeight / maxValue);
			cv::rectangle(histImage, cv::Point(i * scale, histHeight),
				cv::Point((i + 1) * scale, histHeight - height), cv::Scalar(255), -1);
			
		}
		cv::rectangle(histImage, cv::Point(90 * scale, histHeight),
			cv::Point((90 + 1) * scale, 0), cv::Scalar(150), -1);
		cv::imshow("hist", histImage);
		cv::waitKey(1000);
	}
	
	// ��Բ
	inline void DrawCircle(cv::Mat mask,
		const cv::Point2i &center, int radius, 
		const cv::Scalar &color, int lineType)
	{
		cv::circle(mask, center, radius, color, lineType);
	}
	
	// ������
	inline void DrawRect(cv::Mat mask,
		const cv::Rect &rect, const cv::Scalar &color, int lineType)
	{
		cv::rectangle(mask, rect, color, lineType);
	}
	
	inline void DrawRect(cv::Mat mask,
		const cv::Rect &rect, int newwidth,
		int newheight, const cv::Scalar &color, int lineType)
	{
		cv::Rect newrect(0, 0, newwidth, newheight);
		newrect.x = rect.x + (rect.width - newwidth) / 2;
		newrect.y = rect.y + (rect.height - newheight) / 2;
		DrawRect(mask, newrect, color, lineType);
	}
	
	// ����ת����
	inline void DrawRotatedRect(cv::Mat mask,
		const cv::RotatedRect &rotatedrect,
		const cv::Scalar &color, int lineType)
	{
		cv::Point2f ps[4];
		rotatedrect.points(ps);

		std::vector<std::vector<cv::Point>> tmpContours;    // ����һ��InputArrayOfArrays ���͵ĵ㼯
		std::vector<cv::Point> contours;
		for (int i = 0; i != 4; ++i) {
			contours.emplace_back(cv::Point2i(ps[i]));
		}
		tmpContours.insert(tmpContours.end(), contours);
		drawContours(mask, tmpContours, 0, color, lineType);  // ���mask
	}
	
	inline void DrawRotatedRect(cv::Mat mask,
		const cv::RotatedRect &rotatedrect, float newlong, 
		float newshort, const cv::Scalar &color, int lineType)
	{
		cv::RotatedRect newrotatedrect = rotatedrect;
		if (newrotatedrect.size.width > newrotatedrect.size.height) {
			newrotatedrect.size = cv::Size2f(newlong, newshort);
		}
		else {
			newrotatedrect.size = cv::Size2f(newshort, newlong);
		}
		DrawRotatedRect(mask, newrotatedrect, color, lineType);
	}
	
	// ����Բ
	inline void DrawEllipse(cv::Mat mask,
		const cv::RotatedRect &ellipse, 
		const cv::Scalar &color, int lineType)
	{
		cv::ellipse(mask, ellipse, color, lineType);
	}
	
	inline void DrawEllipse(cv::Mat mask,
		const cv::RotatedRect &ellipse, float newlong,
		float newshort, const cv::Scalar &color, int lineType)
	{
		cv::RotatedRect newellipse = ellipse;
		if (newellipse.size.width > ellipse.size.height) {
			newellipse.size = cv::Size2f(newlong, newshort);
		}
		else {
			newellipse.size = cv::Size2f(newshort, newlong);
		}
		DrawEllipse(mask, newellipse, color, lineType);
	}
	
	// ��������Բ
	inline void DrawPill(cv::Mat mask,
		const cv::RotatedRect &rotatedrect,
		const cv::Scalar &color, bool s, int lineType)
	{
		cv::RotatedRect rect = rotatedrect;
		float r = rect.size.height / 2.0f;
		if (rect.size.width > rect.size.height) {
			rect.size.width -= rect.size.height;
		}
		else {
			rect.size.height -= rect.size.width;
			r = rect.size.width / 2.0f;
		}
		cv::Point2f ps[4];
		rect.points(ps);
		// rotatedrect.points(ps);

		std::vector<std::vector<cv::Point>> tmpContours;
		std::vector<cv::Point> contours;
		for (int i = 0; i != 4; ++i) {
			contours.emplace_back(cv::Point2i(ps[i]));
		}
		tmpContours.insert(tmpContours.end(), contours);
		drawContours(mask, tmpContours, 0, color, lineType);  // ���mask

		// ���㳣������
		float a = rotatedrect.size.width;
		float b = rotatedrect.size.height;

		int point01_x = (int)((ps[0].x + ps[1].x) / 2.0f);
		int point01_y = (int)((ps[0].y + ps[1].y) / 2.0f);
		int point03_x = (int)((ps[0].x + ps[3].x) / 2.0f);
		int point03_y = (int)((ps[0].y + ps[3].y) / 2.0f);
		int point12_x = (int)((ps[1].x + ps[2].x) / 2.0f);
		int point12_y = (int)((ps[1].y + ps[2].y) / 2.0f);
		int point23_x = (int)((ps[2].x + ps[3].x) / 2.0f);
		int point23_y = (int)((ps[2].y + ps[3].y) / 2.0f);

		if (s) {
			// float r = a > b ? (b / 2.0f) : (a / 2.0f);
			cv::Point c0 = a < b ? cv::Point(point12_x, point12_y) : cv::Point(point23_x, point23_y);
			cv::Point c1 = a < b ? cv::Point(point03_x, point03_y) : cv::Point(point01_x, point01_y);

			// �������������ķ�ʽ��Բ��ֱ�����ڶ���
			cv::circle(mask, c0, (int)r, color, lineType);
			cv::circle(mask, c1, (int)r, color, lineType);
		}
		else {
			// float r = a < b ? (b / 2.0f) : (a / 2.0f);
			cv::Point c0 = a > b ? cv::Point(point12_x, point12_y) : cv::Point(point23_x, point23_y);
			cv::Point c1 = a > b ? cv::Point(point03_x, point03_y) : cv::Point(point01_x, point01_y);

			// �������������ķ�ʽ��Բ��ֱ�����ڳ���
			cv::circle(mask, c0, (int)r, color, lineType);
			cv::circle(mask, c1, (int)r, color, lineType);
		}
	}
	
	inline void DrawPill(cv::Mat mask,
		const cv::RotatedRect &rotatedrect, float newlong, 
		float newshort, const cv::Scalar &color, bool s, int lineType)
	{
		cv::RotatedRect newrotatedrect = rotatedrect;
		if (newrotatedrect.size.width > newrotatedrect.size.height) {
			newrotatedrect.size = cv::Size2f(newlong, newshort);
		}
		else {
			newrotatedrect.size = cv::Size2f(newshort, newlong);
		}
		DrawPill(mask, newrotatedrect, color, s, lineType);  //��������Բ
	}
	
	// ��Բ�ǵľ���
	inline void DrawRotatedRectChamfer(cv::Mat &mask, 
		const cv::RotatedRect &rotatedrect, float newlong, 
		float newshort, int radius, const cv::Scalar &color, int lineType)
	{
		cv::RotatedRect newrotatedrect = rotatedrect;
		if (newrotatedrect.size.width > newrotatedrect.size.height) {
			newrotatedrect.size = cv::Size2f(newlong, newshort);
		}
		else {
			newrotatedrect.size = cv::Size2f(newshort, newlong);
		}

		//����������ľ���
		cv::RotatedRect r1 = newrotatedrect;
		r1.size.width = r1.size.width - 2 * radius;
		DrawRotatedRect(mask, r1, color, lineType);

		//����������ľ���
		cv::RotatedRect r2 = newrotatedrect;
		r2.size.height = r2.size.height - 2 * radius;
		DrawRotatedRect(mask, r2, color, lineType);

		//���ĸ��ǵ�Բ
		cv::RotatedRect r3 = r2;
		r3.size.width = r1.size.width;

		cv::Point2f ps[4];
		r3.points(ps);

		for (int i = 0; i != 4; ++i) {
			cv::circle(mask, ps[i], radius, color, lineType);
		}
	}
	
	inline void DrawRotatedRectChamfer(cv::Mat &mask,
		const cv::RotatedRect &rotatedrect, float radius,
		const cv::Scalar &color, int lineType)
	{
		cv::RotatedRect newrotatedrect = rotatedrect;

		//����������ľ���
		cv::RotatedRect r1 = newrotatedrect;
		r1.size.width = r1.size.width - 2 * radius;
		DrawRotatedRect(mask, r1, color, lineType);

		//����������ľ���
		cv::RotatedRect r2 = newrotatedrect;
		r2.size.height = r2.size.height - 2 * radius;
		DrawRotatedRect(mask, r2, color, lineType);

		//���ĸ��ǵ�Բ
		cv::RotatedRect r3 = r2;
		r3.size.width = r1.size.width;

		cv::Point2f ps[4];
		r3.points(ps);

		for (int i = 0; i != 4; ++i) {
			cv::circle(mask, ps[i], (int)radius, color, lineType);
		}
	}

	// �������
	inline void DrawPolygon(cv::Mat& mask,
		const std::vector<cv::Point> &contours, 
		const cv::Scalar &color, int lineType) {
		if (contours.empty()) {
			return;
		}

		std::vector<std::vector<cv::Point>> tmpContours;    // ����һ��InputArrayOfArrays ���͵ĵ㼯
		tmpContours.insert(tmpContours.end(), contours);
		drawContours(mask, tmpContours, 0, color, lineType);  // ���mask
	}
	
	inline void DrawPolygon(cv::Mat& mask,
		const std::vector<std::vector<cv::Point>> &contours, const cv::Scalar &color, int lineType) {
		drawContours(mask, contours, 0, color, lineType);  // ���mask
	}

	// ��Circle Rectangle
	inline void DrawCircleRect(cv::Mat& mask,
		const cv::RotatedRect &rotatedrect, const cv::Point2i& center,
		int radius, const cv::Scalar &color, int lineType) {
		cv::Mat maskCircle = mask.clone();
		cv::Mat maskRect = mask.clone();
		cv::circle(maskCircle, center, radius, color, lineType);
		DrawRotatedRect(maskRect, rotatedrect, color, lineType);
		mask = maskRect & maskCircle;
	}

	/**
	 * @brief  ReadAsc              ��ȡasc�ļ�
	 * @param  file                 ����asc����
	 * @param  images               ԭʼ����ͼ
	 * @param  phase                ���������
	 */
	inline void ReadAsc(const std::string& file,
		std::vector<cv::Mat>& images,
		cv::Mat& phase) {
		std::ifstream in(file);
		std::string str;

		getline(in, str);
		getline(in, str);

		int intensoriginx = -1;
		int intensoriginy = -1;
		int intenswidth = -1;
		int intensheight = -1;
		int nbuckets = -1;
		int intensrange = -1;

		in >> intensoriginx
			>> intensoriginy
			>> intenswidth
			>> intensheight
			>> nbuckets
			>> intensrange;

		int phaseoriginx = -1;
		int phaseoriginy = -1;
		int phasewidth = -1;
		int phaseheight = -1;

		in >> phaseoriginx
			>> phaseoriginy
			>> phasewidth
			>> phaseheight;

		getline(in, str);
		getline(in, str);
		getline(in, str);
		getline(in, str);

		int source = -1;
		float intfscalefactor = -1.;
		float wavelengthin = -1.;
		float nunericaperture = -1.;
		float obliquityfactor = -1.;
		float magnification = -1.;
		float camerares = -1.;                            // ����ֱ��� m/pix
		int timestamp = -1;

		in >> source
			>> intfscalefactor
			>> wavelengthin
			>> nunericaperture
			>> obliquityfactor
			>> magnification
			>> camerares
			>> timestamp;

		getline(in, str);
		getline(in, str);
		getline(in, str);

		int phaseres = -1;
		int phaseavgs = -1;
		int minimumareasize = -1;
		int disconaction = -1;
		float disconfilter = -1;
		int connectionorder = -1;
		int removetiltbias = -1;
		int datasign = -1;
		int codevtype = -1;

		in >> phaseres
			>> phaseavgs
			>> minimumareasize
			>> disconaction
			>> disconfilter
			>> connectionorder
			>> removetiltbias
			>> datasign
			>> codevtype;

		getline(in, str);
		getline(in, str);
		getline(in, str);
		getline(in, str);
		getline(in, str);

		const int intensinvalidvalue = 65535;

		images.clear();
		int tmp;
		for (int k = 0; k < nbuckets; ++k) {
			cv::Mat img(intensheight, intenswidth, CV_32FC1);
			for (int i = 0; i < intensheight; ++i) {
				for (int j = 0; j < intenswidth; ++j) {
					in >> tmp;
					if (tmp >= intensinvalidvalue) {
						img.at<float>(i, j) = 0.;
					}
					else {
						img.at<float>(i, j) = tmp * 255.f / intensrange;
					}
				}
			}
			images.emplace_back(img);
		}

		getline(in, str);
		getline(in, str);

		const int phaseinvalidvalue = 2147483640;
		int R = 32768;
		if (phaseres == 0) {
			R = 4096;
		}
		else if (phaseres == 1) {
			R = 32768;
		}

		phase = cv::Mat(phaseheight, phasewidth, CV_32FC1);
		for (int i = 0; i < phaseheight; ++i) {
			for (int j = 0; j < phasewidth; ++j) {
				in >> tmp;
				if (tmp >= phaseinvalidvalue) {
					phase.at<float>(i, j) = nanf("");
				}
				else {
					phase.at<float>(i, j) = tmp * (intfscalefactor * obliquityfactor) / R;
				}
			}
		}

		getline(in, str);
		getline(in, str);

		in.close();
	} // end ReadAsc

	inline void WriteCsv2(std::string file, const cv::Mat& m) {
		std::ofstream f(file);
		f << cv::format(m, cv::Formatter::FMT_CSV);
		f.close();
	}

	//ɾ�������С����ͨ��
	inline void bwareaopen(cv::Mat src, cv::Mat &dst, double min_area) {
		dst = src.clone();
		std::vector<std::vector<cv::Point> > 	contours;
		std::vector<cv::Vec4i> 			hierarchy;
		cv::findContours(src, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());
		if (!contours.empty() && !hierarchy.empty()) {
			std::vector<std::vector<cv::Point> >::const_iterator itc = contours.begin();
			while (itc != contours.end()) {
				cv::Rect rect = cv::boundingRect(cv::Mat(*itc));
				double area = contourArea(*itc);
				if (area < min_area) {
					for (int i = rect.y; i < rect.y + rect.height; i++) {
						uchar *output_data = dst.ptr<uchar>(i);
						for (int j = rect.x; j < rect.x + rect.width; j++) {
							if (output_data[j] == 255) {
								output_data[j] = 0;
							}
						}
					}
				}
				itc++;
			}
		}
	}
} // end calc

#endif
