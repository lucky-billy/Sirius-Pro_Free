/*****************************************************************
*
* fileName:  result.h
*
* function:  ���Խ��
*
* author:    sjt&ztb
*
* date:      2021.1.26
*
* notes:     ������ֲ�����

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/

#pragma once
#ifndef RESULT_H
#define RESULT_H

#include <string>
#include "zernike.h"
#include "baseFunc.h"

// �������ĵ�λ���ͣ�ISO�ĵ�λ�ǹ̶��ģ������û������ISO������
enum UNIT_TYPE {
	UNIT_TYPE_WAVE = 0,                          // �Բ���Ϊ��λ
	UNIT_TYPE_FR,                                // ��FrΪ��λ
	UNIT_TYPE_NM,                                // ��nmΪ��λ
	UNIT_TYPE_UM,                                // ��umΪ��λ
	UNIT_TYPE_MM                                 // ��mmΪ��λ
};

// GRMS���㷽��
enum GRMS_METHOD {
	GRMS_BASIC_DIFF = 0,                         // ��򵥲�ַ�
	GRMS_CENTRAL_DIFF=1                          // ���Ĳ�ַ�
};

// ������ؽ��ʱ���õ����û����ò���
struct CALC_RST_INPUT_PARAMS {
	bool isUseAngleUnit = false;                 // �ԽǶ�Ϊ��λ�ı�־��true:�Ƕȣ�false:����,Ĭ��false��TiltClock, AstClock, ComaClock��
	int row = 0;                                 // ����׾�apertureʱ���õ��Ŀ��
	int width = 0;                               // ����sizeXʱ�õ��Ŀ��
	int height = 0;                              // ����sizeYʱ�õ��ĸ߶�
	float powerX = 0.0f;						 // ����powerXʱ�Ĳ���x^2
	float powerY = 0.0f;						 // ����powerYʱ�Ĳ���y^2
	float px_1mm = 60.0f;                        // ÿmm��������ظ���
	float refractiveIndex = 1.5f;                // ����Ĳ�����������
	float testWavelength = 632.8f;               // ����ļ������ķ����Ĺ�Ĳ���
	float ISOWavelength = 546.0f;                // ����ISO����ʱ�������ISO����
	float disPlayWavelength = 632.8f;            // �Բ�������FRΪ��ʾ��λʱ������Ĳ���
	float scaleFactorForHoleType = 1.0f;         // ���ݲ�ͬ�׵����ͣ����õı�������(�����:1����ī��:1.485461��940/632.8)
	UNIT_TYPE unitType = UNIT_TYPE_WAVE;         // ��ʾ����ĵ�λ��Ĭ��ֵΪ����
	GRMS_METHOD grmsMethod = GRMS_CENTRAL_DIFF;  // GRMS���㷽����Ĭ��ΪZygo���õ����Ĳ�ַ�
};

// ����pv,rms�����ı�־��trueΪ����
struct CALC_PV_RMS_RST_FLAGS {
	// pv
	bool pvFlag = true;
	bool pvxFlag = false;
	bool pvyFlag = false;
	bool pvxyFlag = false;
	bool pvrFlag = false;
	bool pvresFlag = false;
	// rms
	bool rmsFlag = true;
	bool rmsxFlag = false;
	bool rmsyFlag = false;
	bool rmsxyFlag = false;
	bool rmsresFlag = false;
};

// Pv��rms����ؼ�����
struct PV_RMS_RESULT {
	float pv;                                    // zernikeϵ����Ͻ����pv,͸����ǰ�Ĺ�̲�
	float pvr;                                   // pvr = pv(zernike)+ 3 * rms(res)
	float pv_res;                                // �в��pv
	float pv_x;                                  // zernikeϵ����Ͻ��ĳһ�е�pv
	float pv_y;                                  // zernikeϵ����Ͻ��ĳһ�е�pv
	float pv_xy;                                 // zernikeϵ����Ͻ��ĳһ�к�ĳһ�е�pv�� pv_xy = (pv_x + pv_y) / 2

	float rms;                                   // zernikeϵ����Ͻ���ľ�����
	float rms_res;                               // �в�ľ�����
	float rms_x;                                 // zernikeϵ����Ͻ��ĳһ�еľ�����
	float rms_y;                                 // zernikeϵ����Ͻ��ĳһ�еľ�����
	float rms_xy;                                // zernikeϵ����Ͻ��ĳһ�к�ĳһ�е�rms�� rms_xy = (rms_x + rms_y) / 2
};

// ����zernike,seidel�����ı�־��trueΪ����
struct CALC_ZER_SEI_RST_FLAGS {
	// zernike
	bool zernikeTiltFlag = false;
	bool zernikePowerFlag = false;
	bool zernikePowerXFlag = false;
	bool zernikePowerYFlag = false;
	bool zernikeAstFlag = false;
	bool zernikeComaFlag = false;
	bool zernikeSphericalFlag = false;
	// seidel
	bool seidelTiltFlag = false;
	bool seidelFocusFlag = false;
	bool seidelAstFlag = false;
	bool seidelComaFlag = false;
	bool seidelSphericalFlag = false;
	// angle
	bool seidelTiltClockFlag = false;
	bool seidelAstClockFlag = false;
	bool seidelComaClockFlag = false;
	// rms
	bool rmsPowerFlag = false;
	bool rmsAstFlag = false;
	bool rmsComaFlag = false;
	bool rmsSaFlag = false;
};

// zernike��seidel����ؼ�����
struct ZERNIKE_SEIDEL_RESULT {
	// zernike ���
	float zernikeTilt;                           // zernike tilt
	float zernikePower;                          // zernike power
	float zernikePowerX;                         // zernike power x
	float zernikePowerY;                         // zernike power y
	float zernikeAst;                            // zernike ast
	float zernikeComa;                           // zernike coma
	float zernikeSpherical;                      // zernike Spherical
	// seidel ���
	float seidelTilt;                            // seidel tilt
	float seidelFocus;                           // seidel focus
	float seidelAst;                             // seidel Astigmatism
	float seidelComa;                            // seidel coma
	float seidelSpherical;                       // seidel Spherical
	// seidel ���Ƕ�
	float seidelTiltClock;                       // seidel tilt clock
	float seidelAstClock;                        // seidel ast clock
	float seidelComaClock;                       // seidel coma clock
	// rms
	float rms_power;                             // zernikePower / 2 / 3^0.5
	float rms_ast;                               // seidelAst / 2 / 6^0.5
	float rms_coma;                              // seidelComa / 6 / 2^0.5
	float rms_sa;                                // seidelSpherical / 6 / 5^0.5
};

// ����ISO�����ı�־��trueΪ����
struct CALC_ISO_RST_FLAGS {
	bool sagFlag = false;
	bool irrFlag = false;
	bool rsiFlag = false;
	bool rmstFlag = false;
	bool rmsaFlag = false;
	bool rmsiFlag = false;
};

// ISOָ�����ؼ�����
struct ISO_RESULT {
	float sag;                                   // ��ʸ���
	float irr;                                   // ������ƫ��
	float rsi;                                   // ��ת�ԳƲ�����ƫ��
	float rmst;                                  // �ܱ���ƫ��ľ�����
	float rmsa;                                  // ������ƫ��ľ�����
	float rmsi;                                  // ����ת�Գ�ƫ��ľ�����
};

// �����ܽ�������ı�־��trueΪ����
struct CALC_TOTAL_RST_FLAGS {
	bool ttvFlag = false;                        // ���������ĺ�Ȳ�
	bool fringesFlag = false;                    // ������
	bool strehlFlag = false;                     // ����˹���ж��ȵı�־
	bool apertureFlag = false;                   // �ھ�
	bool parallelDegreeFlag = false;             // ƽ�жȲ��Ա�־���ò���Ϊtrueʱ���������ھ�
	bool sizeXFlag = false;                      // x����ĳߴ�
	bool sizeYFlag = false;                      // y����ĳߴ�
	bool parallelThetaFlag = false;              // ƽ�нǶ�
	bool thicknessFlag = false;                  // �������
	bool crinkleFlag = false;                    // ��
	bool grmsFlag = false;                       // �Ƿ����grms   
	bool categoryFlag = false;                   // �Ƿ�ϸ�
	bool concavityFlag = false;                  // ���ȣ�-1��ʾ͹��1��ʾ��           
	CALC_ISO_RST_FLAGS isoResultFlags;           // ISOָ�����ؼ�����
	CALC_PV_RMS_RST_FLAGS pvRmsResultFlags;      // pv��rms����ؼ�����	
	CALC_ZER_SEI_RST_FLAGS zerSeiResultFlags;    // zernike��seidel����ؼ�����
};

// ������
struct TOTAL_RESULT {
	ISO_RESULT isoResult;                        // ISOָ�����ؼ�����
	PV_RMS_RESULT pvRmsResult;                   // pv��rms����ؼ�����
	ZERNIKE_SEIDEL_RESULT zerSeiResult;          // zernike��seidel����ؼ�����
	// others 
	float mean;                                  // ���ε�ƽ��ֵ
	float ttv;                                   // ���������ĺ�Ȳ�
	float fringes;                               // ������
	float strehl;                                // ˹���ж���
	float aperture;                              // �ھ�
	float sizeX;                                 // x����ĳߴ�
	float sizeY;                                 // y����ĳߴ�
	float parallelTheta;                         // ƽ�нǶ�
	float thickness;                             // �������
	float crinkle;                               // ��
	float grms;                                  // grms����ǰ�ݶȾ�������
	bool category;                               // �Ƿ�ϸ�
	int concavity;                               // ���ȣ�-1��ʾ͹��1��ʾ��
	int pointnumber;                             // �������ص����
};

// Result��
class Result {
public:
	CHECK_THR checkThr;                          // check����ʹ�õ���ֵ
	TOTAL_RESULT totalResult;                    // ���Խ��
	CALC_TOTAL_RST_FLAGS calcTotalResultFlags;   // �Ƿ����������ı�־��true�����㣬false:������
	CALC_RST_INPUT_PARAMS calcResultInputParams; // ������ؽ��ʱ���õ����û����ò���
	ERROR_TYPE errorType = ERROR_TYPE_NOT_ERROR; // ��������
	bool isSingleHole = true;                    // �Ƿ��ǵ���ģʽ��true:���ף�false:���
	float ISOResultCoef;                         // ����ISO���ʱ��ת��ϵ��
	float otherResultCoef;                       // �������ISO�������������ʱ��ת��ϵ��
	float angleCoef;                             // ����TiltClock, AstClock, ComaClockʱ�ĽǶ�ת��ϵ��
	cv::Mat psdx;                                // X��Ƭ��Ӧ��psd��ȡ������
	cv::Mat psdy;                                // Y��Ƭ��Ӧ��psd��ȡ������

public:
	Result(CALC_TOTAL_RST_FLAGS inputFlags, CALC_RST_INPUT_PARAMS inputParams, CHECK_THR thr):
		calcTotalResultFlags(inputFlags), calcResultInputParams(inputParams), checkThr(thr) {}

	// ��ʼ������
	void initConfig(CALC_TOTAL_RST_FLAGS inputFlags, CALC_RST_INPUT_PARAMS inputParams);

	// ����mask���ü���sizeX��sizeY�Ĳ���
	void setSize(cv::Mat& mask);

	// ������ֽ��
	void ResultProcess(Zernike zernike);

	// ��������ͼ���
	void GetSurfaceResult(const cv::Mat surface, const cv::Mat surface_tilt, const cv::Mat mask);

	/**
	* @brief Pv                            ������������Pv
	* @param src                           �������
	* @param pv                            Pv = max - min
	* @param min                           ��Сֵ
	* @param max                           ���ֵ
	*/
	inline void PvMinMax(const cv::Mat& src,
		float& pv,
		float& min,
		float& max) {
		double minv;
		double maxv;
		//����mask��������ֵ����Сֵ;
		cv::minMaxIdx(src, &minv, &maxv, nullptr, nullptr, src == src);
		pv = (float)(maxv - minv);
		min = (float)minv;
		max = (float)maxv;
	}

	/**
	* @brief Pvxy                          �ֱ�����м�һ�к�һ�е�Pv��Ȼ��ȡ���ߵľ�ֵ
	* @param src                           �������
	* @param pv                            ������
	*/
	inline void Pvxy(const cv::Mat& src,
		float &pv) {
		double minv;
		double maxv;
		cv::Mat t;
		t = src.row(src.rows / 2);          //ѡȡ�м��һ������
		//�������ֵ����Сֵ
		cv::minMaxIdx(t, &minv, &maxv, nullptr, nullptr, t == t);
		float pvx = (float)(maxv - minv);
		t = src.col(src.cols / 2);          //ѡȡ�м��һ������
		//�������ֵ����Сֵ
		cv::minMaxIdx(t, &minv, &maxv, nullptr, nullptr, t == t);
		float pvy = (float)(maxv - minv);

		pv = (float)(pvx + pvy) / 2.0f;          //ȡ���ߵľ�ֵ
	}

	/**
	* @brief Rms                           ����rms������
	* @param src                           �������
	* @param rms                           ������
	*/
	inline void Rms(const cv::Mat& src,
		float &rms) {
		cv::Mat tmp;
		cv::pow(src, 2.0, tmp);
		float mean_tmp = (float)(cv::mean(tmp, tmp == tmp)[0]);     // �����ֵ(nan != nan)
		rms = std::sqrt(mean_tmp);          //�õ�rms
	}

	/**
	* @brief Rmsxy                         �ֱ�����м�һ�к�һ�е�rms��������Ȼ��ȡ���ߵľ�ֵ
	* @param src                           �������
	* @param rms                           ������
	*/
	inline void Rmsxy(const cv::Mat& src,
		float &rms) {
		cv::Mat tmp;
		cv::pow(src.row(src.rows / 2), 2.0, tmp);             //ѡȡ�м��һ������
		double mean_tmp_x = cv::mean(tmp, tmp == tmp)[0];     // ����x��ֵ(nan != nan)
		float rms_x = (float)std::sqrt(mean_tmp_x);

		cv::pow(src.col(src.cols / 2), 2.0, tmp);             //ѡȡ�м��һ������
		double mean_tmp_y = cv::mean(tmp, tmp == tmp)[0];     // ����y��ֵ(nan != nan)
		float rms_y = (float)std::sqrt(mean_tmp_y);
		rms = (rms_x + rms_y) / 2.0f;
	}

	/**
	* @brief Mean                          ��������ͼ�ľ�ֵ
	* @param mean                          ������
	*/
	inline void Mean(const cv::Mat& src,
		float &mean) {
		mean= (float)(cv::mean(src, src == src)[0]);        //�õ���nan���ľ�ֵ
	}

	/**
	* @brief ZernikeTilt                    ����Zernike Tilt
	* @param coefficient                    zernikeϵ��
	* @param zerniketilt                    zernike tilt
	*/
	inline void ZernikeTilt(const cv::Mat& coefficient,
		float& zerniketilt) {
		zerniketilt = 2.0f * std::sqrtf(
			std::powf(coefficient.at<float>(1), 2.0f) +
			std::powf(coefficient.at<float>(2), 2.0f));
	}

	/**
	* @brief ZernikePower                   ����Zernike power
	* @param coefficient_4                  �Ľ׶���ʽ��ϣ��������ϵ��
	* @param zernikepower                   zernike power
	*/
	inline void ZernikePower(const cv::Mat& coefficient_4,
		float& zernikepower,
		int& concavity) {
		zernikepower = 2.0f * coefficient_4.at<float>(3);
		if (zernikepower > 0) {
			concavity = -1;
		}
		else {
			concavity = 1;
		}
	}

	/**
	* @brief ZernikeAst                     ����Zernike Ast
	* @param coefficient                    zernikeϵ��
	* @param zernikeast                     Zernike Ast
	*/
	inline void ZernikeAst(const cv::Mat& coefficient,
		float& zernikeast) {
		zernikeast = std::sqrtf(std::powf(coefficient.at<float>(4), 2.0) +
			std::powf(coefficient.at<float>(5), 2.0));
	}

	/**
	* @brief ZernikeComa                    ����Zernike Coma
	* @param coefficient                    zernikeϵ��
	* @param zernikecoma                    Zernike coma
	*/
	inline void ZernikeComa(const cv::Mat& coefficient,
		float& zernikecoma) {
		zernikecoma;
		zernikecoma = std::sqrtf(std::powf(coefficient.at<float>(6), 2.0) +
			std::powf(coefficient.at<float>(7), 2.0));
	}

	/**
	* @brief ZernikeSpher                   ����Zernike Spherical
	* @param coefficient                    zernikeϵ��
	* @param zernikespher                   Zernike Spherical
	*/
	inline void ZernikeSpher(const cv::Mat& coefficient,
		float& zernikespher) {
		zernikespher = coefficient.at<float>(8);
	}

	/**
	* @brief SeidelTilt                     ����Seidel Tilt
	* @param coefficient                    zernikeϵ��
	* @param seideltilt                     Seidel Tilt
	*/
	inline void SeidelTilt(const cv::Mat& coefficient,
		float& seideltilt) {

		seideltilt = std::sqrtf(
			std::powf((coefficient.at<float>(1) - 2 * coefficient.at<float>(6)), 2.0) +
			std::powf((coefficient.at<float>(2) - 2 * coefficient.at<float>(7)), 2.0));
	}

	/**
	* @brief SeidelTiltClock                ����Seidel Tilt Clock
	* @param coefficient                    zernikeϵ��
	* @param seideltiltclock                Seidel Tilt Clock
	*/
	inline void SeidelTiltClock(const cv::Mat& coefficient,
		float& seideltiltclock) {
		// ����ֵ�ǻ���
		seideltiltclock = std::atan2f(
			(coefficient.at<float>(2) - 2 * coefficient.at<float>(7)),
			(coefficient.at<float>(1) - 2 * coefficient.at<float>(6)));
	}

	/**
	* @brief SeidelFocus                    ����Seidel Focus
	* @param coefficient                    zernikeϵ��
	* @param seidelfocus                    Seidel Focus
	*/
	inline void SeidelFocus(const cv::Mat& coefficient,
		float& seidelfocus) {
		seidelfocus = 2.0f * coefficient.at<float>(3) - 6.0f * coefficient.at<float>(8);
	}

	/**
	* @brief SeidelAst                      ����Seidel Ast
	* @param coefficient                    zernikeϵ��
	* @param seidelast                      Seidel Ast
	*/
	inline void SeidelAst(const cv::Mat& coefficient,
		float& seidelast) {
		seidelast = 2.0f * std::sqrtf(std::powf(coefficient.at<float>(4), 2.0f) +
			std::powf(coefficient.at<float>(5), 2.0f));
	}

	/**
	* @brief SeidelAstClock                 ����Seidel Ast Clock
	* @param coefficient                    zernikeϵ��
	* @param seidelastclock                 Seidel Ast Clock
	*/
	inline void SeidelAstClock(const cv::Mat& coefficient,
		float& seidelastclock) {
		// ���Ϊ����
		seidelastclock = 0.5f * std::atan2f(coefficient.at<float>(5), coefficient.at<float>(4));
	}

	/**
	* @brief SeidelComa                     ����Seidel Coma
	* @param coefficient                    zernikeϵ��
	* @param seidelcoma                     Seidel Coma
	*/
	inline void SeidelComa(const cv::Mat& coefficient,
		float& seidelcoma) {
		seidelcoma = 3.0f * std::sqrtf(std::powf(coefficient.at<float>(6), 2.0) +
			std::powf(coefficient.at<float>(7), 2.0));
	}

	/**
	* @brief SeidelComaClock                ����Seidel Coma Clock
	* @param coefficient                    zernikeϵ��
	* @param seidelcomaclock                Seidel Coma Clock
	*/
	inline void SeidelComaClock(const cv::Mat& coefficient,
		float& seidelcomaclock) {
		seidelcomaclock = std::atan2f(coefficient.at<float>(7),
			coefficient.at<float>(6));
	}

	/**
	* @brief SeidelSpher                    ����Seidel Spher
	* @param coefficient                    zernikeϵ��
	* @param seidelspher                    Seidel Spher
	*/
	inline void SeidelSpher(const cv::Mat& coefficient,
		float& seidelspher) {
		seidelspher = 6.0f * coefficient.at<float>(8);
	}

	/**
	* @brief Ttv                            ����Ttv
	* @param fitting_all                    zernikeϵ����Ͻ���������� = terms��
	* @param refractiveindex                ������
	* @param ttv                            ttv
	*/
	inline void Ttv(const cv::Mat& fitting_all,
		float refractiveindex,
		float& ttv) {
		double minv;
		double maxv;
		cv::minMaxIdx(fitting_all, &minv, &maxv, nullptr, nullptr, fitting_all == fitting_all);
		ttv = (float)(maxv - minv) / (refractiveindex - 1.0f);
	}

	/**
	* @brief Fringes fr in testwavelengthnm
	* @param fitting_all
	* @param fringes
	* @param in
	* @param testwavelengthnm
	*/
	inline void Fringes(const cv::Mat& fitting_all,
		float& fringes) {
		double minv;
		double maxv;
		cv::minMaxIdx(fitting_all, &minv, &maxv,
			nullptr, nullptr, fitting_all == fitting_all);
		fringes = (float)(maxv - minv);
	}

	/**
	* @brief IsoSag fr in ISO wavelength_nm
	* @param coefficient_4
	* @param sag
	*/
	inline void IsoSag(const cv::Mat& coefficient_4,
		float& sag) {
		sag = abs(2.0f * coefficient_4.at<float>(3));
	}

	/**
	* @brief IsoIrr fr in ISO wavelength_nm
	* @param fitting_r0_r1_r2_r3
	* @param irr
	*/
	inline void IsoIrr(const cv::Mat& fitting_r0_r1_r2_r3,
		float& irr) {
		double minv;
		double maxv;
		cv::minMaxIdx(fitting_r0_r1_r2_r3, &minv, &maxv,
			nullptr, nullptr, fitting_r0_r1_r2_r3 == fitting_r0_r1_r2_r3);
		irr = (float)(maxv - minv);
	}

	/**
	* @brief IsoRsi fr in ISO wavelength_nm
	* @param fitting_8_15_24_35_36
	* @param rsi
	*/
	inline void IsoRsi(const cv::Mat& fitting_z8_z15_z24_z35_z36,
		float& rsi) {
		double minv;
		double maxv;
		cv::minMaxIdx(fitting_z8_z15_z24_z35_z36, &minv, &maxv, nullptr,
			nullptr, fitting_z8_z15_z24_z35_z36 == fitting_z8_z15_z24_z35_z36);
		rsi = (float)(maxv - minv);
	}

	/**
	* @brief ParallelTheta
	* @param paralleltheta unit: s
	* @param fringes unit: fr
	* @param refractiveindex
	* @param diameter unit: mm
	*/
	inline void ParallelTheta(float& paralleltheta,
		float tilt,
		float refractiveindex,
		float diameter) {
		paralleltheta = 206.246f *0.6328* tilt / diameter / (refractiveindex - 1.0f);
		//paralleltheta = 0.206246f * 632.8f *fringes / diameter / (2*refractiveindex );//��������2*n��̩������2*��n-1����������ģ���ƽ�岣����ƽ�жȿ��ټ���о���
	}

private:
	// ���㵥λת��ϵ��
	void calcUintConvertCoef();

	// ����pv��rms
	void calcPvRmsResult(Zernike zernike);

	// ����zernike��seidel����
	void calcZerSeiResult(Zernike zernike);

	// ����ISO��ز���
	void calcISOResult(Zernike zernike);

	// ����������������ز���
	void calcOthersResult(Zernike zernike);

	// ����psd��psdδͳһ��λ����ǰ��λΪ����^2*mm��
	void calcPsd(const cv::Mat fitting_rem_zer);

	// ������Ƭpsd
	cv::Mat calcPsdSlice(const cv::Mat Slice, float pixel_mm);

	// ���㲨ǰ�ݶȾ�������Grms����������λΪ����ʱ����ֵ��λΪ����^2*mm��
	void calcGrms(const cv::Mat fitting_rem_zer);

	// Grms��򵥲���㷨
	float Grms_Basic_Diff(cv::Mat fitting_rem_zer,cv::Mat mask);

	// Grms���Ĳ���㷨
	float Grms_CENTRAL_Diff(cv::Mat fitting_rem_zer, cv::Mat mask);

};

#endif
