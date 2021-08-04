/*****************************************************************
*
* fileName:  unwrap.h
*
* function:  ��λ�����
*
* author:    sjt&ztb��Steven��
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
	UNWRAP_METHOD_HISTOGRAM=1,             //����ͼ
	UNWRAP_METHOD_SPAN_BRANCH_CUT=2,       //����֦�з�
	UNWRAP_METHOD_DCT=3,                   //DCTȫ�ַ��������ƣ�
	UNWRAP_METHOD_HISTOGRAM_PRO=4          //����ͼ��ȥ���쳣����
};
struct UNWRAP_PARAMS {
	// ֦�з�������㷨���
	int maxBoxRadius=5;                                     // ֦�з�����������������İ뾶
	int residuesNumThresh=500;                              // �в�������ֵ
	// ����������㷨���
	float unwrap_span_adjust =-0.1f;                        // ����֦�з������ڵ�������
	int span_automask_thresh=40;
	// ����ͼ������㷨���
    float histThresh = float(3 * CV_PI * CV_PI);
	int nbrOfSmallBins = 10;
	int nbrOfLargeBins = 5;
	// ���������
	UNWRAP_METHOD unwrapMethod= UNWRAP_METHOD_BRANCH_CUT;   // ������ķ���
};

class Unwrap {
public:
	int rowref;                            // �ο���ʼ���������
	int colref;                            // �ο���ʼ���������
	cv::Mat unwrappedPhase;                // ����������λ
	cv::Mat unwrappedPhase_original;       // �洢ԭʼ����������λ
	cv::Mat histogram_mask;
	UNWRAP_PARAMS unwrapParams;           // ���������
	ERROR_TYPE errorType;                  // ��������	

public:
	Unwrap(UNWRAP_PARAMS params): unwrapParams(params){}
	/**
	* @brief UnwrapProcess            ����������г���
	* @param phase                    ������λ
	* @param modulation               ���ƶ�
	* @Param mask                     ��Ĥ
	*/
	void UnwrapProcess(cv::Mat &phase, const cv::Mat &modulation, const cv::Mat &mask);

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
	void UnwrapBranchCuts(cv::Mat &phase,
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

	/*--------------------- ����ͼ�����-------------------------*/
	/**
	* @brief UnwrapHistogram          ����ͼ�����
	* @param phase                    ������λ
	* @Param mask                     ��Ĥ
	* @Param histthresh  
	* @Param nbrOfSmallBins
	* @Param nbrOfLargeBins
	*/
	int UnwrapHistogram(cv::Mat &phase, const cv::Mat &mask,float histthresh, int nbrOfSmallBins,int nbrOfLargeBins);

	void computePixelsReliability(cv::Mat &phase, cv::Mat &mask);

	void computeEdgesReliabilityAndCreateHistogram(float histthresh, int nbrOfSmallBins, int nbrOfLargeBins);

	void createAndSortEdge(int idx1, int idx2,int nbrOfSmallBins);

	void unwrapHistogram();

	void addIncrement(cv::Mat &unwrappedPhaseMap);

	int findInc(float a, float b);

	float wrap(float a, float b);

	class Pixel
	{
	private:
		// Value from the wrapped phase map
		float phaseValue;
		// Id of a pixel. Computed from its position in the Mat
		int idx;
		// Pixel is valid if it's not in a shadow region
		bool valid;
		// "Quality" parameter. See reference paper
		float inverseReliability;
		// Number of 2pi  that needs to be added to the pixel to unwrap the phase map
		int increment;
		// Number of pixels that are in the same group as the current pixel
		int nbrOfPixelsInGroup;
		// Group id. At first, group id is the same value as idx
		int groupId;
		// Pixel is alone in its group
		bool singlePixelGroup;
	public:
		Pixel();
		Pixel(float pV, int id, bool v, float iR, int inc);
		float getPhaseValue();
		int getIndex();
		bool getValidity();
		float getInverseReliability();
		int getIncrement();
		int getNbrOfPixelsInGroup();
		int getGroupId();
		bool getSinglePixelGroup();
		void setIncrement(int inc);
		// When a pixel which is not in a single group is added to a new group, we need to keep the previous increment and add "inc" to it.
		void changeIncrement(int inc);
		void setNbrOfPixelsInGroup(int nbr);
		void setGroupId(int gId);
		void setSinglePixelGroup(bool s);
	};

	class Edge
	{
	private:
		// Id of the first pixel that forms the edge
		int pixOneId;
		// Id of the second pixel that forms the edge
		int pixTwoId;
		// Number of 2pi that needs to be added to the second pixel to remove discontinuities
		int increment;
	public:
		Edge();
		Edge(int p1, int p2, int inc);
		int getPixOneId();
		int getPixTwoId();
		int getIncrement();
	};

	// Class describing a bin from the histogram
	class HistogramBin
	{
	private:
		float start;
		float end;
		std::vector<Edge> edges;
	public:
		HistogramBin();
		HistogramBin(float s, float e);
		void addEdge(Edge e);
		std::vector<Edge> getEdges();
	};

	// Class describing the histogram. Bins before "thresh" are smaller than the one after "thresh" value
	class Histogram
	{
	private:
		std::vector<HistogramBin> bins;
		float thresh;
		float smallWidth;
		float largeWidth;
		int nbrOfSmallBins;
		int nbrOfLargeBins;
		int nbrOfBins;
	public:
		Histogram();
		void createBins(float t, int nbrOfBinsBeforeThresh, int nbrOfBinsAfterThresh);
		void addBin(HistogramBin b);
		void addEdgeInBin(Edge e, int binIndex);
		float getThresh();
		float getSmallWidth();
		float getLargeWidth();
		int getNbrOfBins();
		std::vector<Edge> getEdgesFromBin(int binIndex);
	};

private:
	vector< Pixel> pixels;
	Histogram histogram;
};

#endif // UNWRAP_H

