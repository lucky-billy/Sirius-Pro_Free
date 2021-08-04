/*****************************************************************
*
* fileName:  unwrap.h
*
* function:  相位解包裹
*
* author:    sjt&ztb（Steven）
*
* date:      2021.1.26
*
* notes:     解包裹算法

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
	UNWRAP_METHOD_BRANCH_CUT = 0,          //枝切法
	UNWRAP_METHOD_HISTOGRAM=1,             //质量图
	UNWRAP_METHOD_SPAN_BRANCH_CUT=2,       //跨区枝切法
	UNWRAP_METHOD_DCT=3,                   //DCT全局法（待完善）
	UNWRAP_METHOD_HISTOGRAM_PRO=4          //质量图（去除异常区）
};
struct UNWRAP_PARAMS {
	// 枝切法解包裹算法相关
	int maxBoxRadius=5;                                     // 枝切法解包裹的最大搜索框的半径
	int residuesNumThresh=500;                              // 残差点个数阈值
	// 跨区解包裹算法相关
	float unwrap_span_adjust =-0.1f;                        // 跨区枝切法的周期调整因子
	int span_automask_thresh=40;
	// 质量图解包裹算法相关
    float histThresh = float(3 * CV_PI * CV_PI);
	int nbrOfSmallBins = 10;
	int nbrOfLargeBins = 5;
	// 解包裹方法
	UNWRAP_METHOD unwrapMethod= UNWRAP_METHOD_BRANCH_CUT;   // 解包裹的方法
};

class Unwrap {
public:
	int rowref;                            // 参考起始点的行坐标
	int colref;                            // 参考起始点的列坐标
	cv::Mat unwrappedPhase;                // 解包裹后的相位
	cv::Mat unwrappedPhase_original;       // 存储原始解包裹后的相位
	cv::Mat histogram_mask;
	UNWRAP_PARAMS unwrapParams;           // 解包裹参数
	ERROR_TYPE errorType;                  // 错误类型	

public:
	Unwrap(UNWRAP_PARAMS params): unwrapParams(params){}
	/**
	* @brief UnwrapProcess            解包裹主运行程序
	* @param phase                    包裹相位
	* @param modulation               调制度
	* @Param mask                     掩膜
	*/
	void UnwrapProcess(cv::Mat &phase, const cv::Mat &modulation, const cv::Mat &mask);

	// 点与点的求余运算 a / b
	inline float modf(float a, float b) {
		if (b == 0.0f) {
			return a;
		}
		else {
			return a - b * floorf(a / b);
		}
	}

	/**
	* @brief getUnwrappedPhase        获得解包裹数据
	* @param phase                    包裹相位
	* @param modulation               调制度
	* @param branchcuts               枝切线
	* @param mask                     掩膜
	* @return                         解包裹后的结果
	*/
	inline cv::Mat getUnwrappedPhase(const cv::Mat& phase,
		const cv::Mat& modulation,
		const cv::Mat& branchcuts,
		const cv::Mat& mask) {

		// 找到起始参考点坐标，取的是调制度大于某阈值的所有点的中心点
		GetRefPoint(modulation, branchcuts);

		// 图像全白或者全黑时，会出现崩溃的Bug
		if (errorType == ERROR_TYPE_UNWRAP_ERROR) {
			return phase.clone();
		}
		// 四邻域洪水填充法解包裹
		// cv::Mat unwrappedphase = ptrGoldsteinFloodFill(phase, modulation, branchcuts, mask);
		// cv::Mat unwrappedphase = ptrGoldsteinFloodFill2(phase, modulation, branchcuts, mask);
		cv::Mat unwrappedphase = ptrGoldsteinFloodFill3(phase, modulation, branchcuts, mask);
		return unwrappedphase;
	}

private:
	// 矩阵与矩阵的求余运算 a ./ b
	cv::Mat modf(const cv::Mat& a, const cv::Mat& b);
	
	// 矩阵与点的求余运算 a / b 
	cv::Mat modf(const cv::Mat& a, float b);

	/*--------------------- BranchCuts -------------------------*/
	/**
	* @brief UnwrapBranchCuts         枝切法相位解包裹
	* @param phase                    包裹相位
	* @param modulation               调制度
	* @Param mask                     掩膜
	* @return                         
	*/
	void UnwrapBranchCuts(cv::Mat &phase,
		const cv::Mat &modulation,
		const cv::Mat &mask);

	/**
	* @brief PhaseResidues            计算包裹相位残差
	* @param phase CV_32FC1           包裹相位
	* @return CV_8SC1 -127,0,127      残差
	*/
	cv::Mat PhaseResidues(const cv::Mat& phase);

	/**
	* @brief ResiduesCount            统计残差点的个数
	* @param residues                 PhaseResidues计算得到的残差
	* @Param mask                     掩膜
	* @return                         残差点个数
	*/
	inline int ResiduesCount(const cv::Mat& residues,
		const cv::Mat& mask);

	/**
	* @brief ResiduesCount            根据残差得到枝切线
	* @param residues                 PhaseResidues计算得到的残差
	* @Param mask                     掩膜
	* @Param maxboxradius             最大搜索框的半径
	* @return                         枝切线
	*/
	cv::Mat BranchCuts(const cv::Mat& residues,
		const cv::Mat& mask,
		int maxboxradius = 20);

	/**
	* @brief PlaceBranchCutInternal   在两点之间放置枝切线
	* @param branchcuts               枝切线
	* @param r0                       0点行坐标
	* @param c0                       0点列坐标
	* @param r1                       1点行坐标
	* @param c1                       1点列坐标
	* @return BranchCuts              枝切线
	*/
	void PlaceBranchCutInternal(cv::Mat& branchcuts,
		int r0,
		int c0,
		int r1,
		int c1);

	/**
	* @brief getCoordinates           查找不为0的点的坐标
	* @param adjoin                   近邻点
	* @param rows                     mask的rows
	* @param cols                     mask的cols
	* @param Coordinate               近邻点的坐标
	* @param count                    偏离解包裹起始点的个数
	* @retur                          不为0的点的个数
	*/
	int getCoordinates(uchar **adjoin,
		int rows,
		int cols,
		int **Coordinate,
		int count);

	/**
	* @brief getCoordinates           查找不为0的点的坐标
	* @param adjoin                   近邻点
	* @param rows                     mask的rows
	* @param cols                     mask的cols
	* @param Coordinate               近邻点的坐标
	* @param count                    偏离解包裹起始点的个数
	* @retur                          不为0的点的个数
	*/
	int getCoordinates2(uchar *adjoin,
		int rows,
		int cols,
		int **Coordinate,
		int count);

	/**
	* @brief GoldsteinFloodFill       四邻域洪水填充法解包裹
	* @param phase                    包裹相位
	* @param modulation               调制度
	* @param branchcuts               枝切线
	* @param mask                     掩膜
	* @return                         解包裹后的结果
	*/
	cv::Mat GoldsteinFloodFill(const cv::Mat& phase,
		const cv::Mat& modulation,
		const cv::Mat& branchcuts,
		const cv::Mat& mask);

	/**
	* @brief ptrGoldsteinFloodFill    四邻域洪水填充法解包裹,用ptr替换at
	* @param phase                    包裹相位
	* @param modulation               调制度
	* @param branchcuts               枝切线
	* @param mask                     掩膜
	* @return                         解包裹后的结果
	*/
	cv::Mat ptrGoldsteinFloodFill(const cv::Mat& phase,
		const cv::Mat& modulation,
		const cv::Mat& branchcuts,
		const cv::Mat& mask);

	/**
	* @brief ptrGoldsteinFloodFill2   四邻域洪水填充法解包裹，用ptr替换at,用getCoordinates替换findNonZero；
	* @param phase                    包裹相位
	* @param modulation               调制度
	* @param branchcuts               枝切线
	* @param mask                     掩膜
	* @return                         解包裹后的结果
	*/
	cv::Mat ptrGoldsteinFloodFill2(const cv::Mat &phase,
		const cv::Mat &modulation, 
		const cv::Mat &branchcuts,
		const cv::Mat &mask);

	/**
	* @brief ptrGoldsteinFloodFill2   四邻域洪水填充法解包裹，用ptr替换at,用getCoordinates替换findNonZero,用一维数组替换二维数组
	* @param phase                    包裹相位
	* @param modulation               调制度
	* @param branchcuts               枝切线
	* @param mask                     掩膜
	* @return                         解包裹后的结果
	*/
	cv::Mat ptrGoldsteinFloodFill3(const cv::Mat &phase,
		const cv::Mat &modulation,
		const cv::Mat &branchcuts,
		const cv::Mat &mask);

	/**
	* @brief GetRefPoint              获得起始参考点的坐标
	* @param modulation               调制度
	* @param branchcuts               枝切线
	*/
	void GetRefPoint(const cv::Mat& modulation,
		const cv::Mat& branchcuts);

	/*--------------------- SpanBranchCuts -------------------------*/
	/**
	* @brief UnwrapSpanBranchCuts     跨区枝切法解包裹
	* @param phase                    包裹相位
	* @param modulation               调制度
	* @Param mask                     掩膜
	*/
	int UnwrapSpanBranchCuts(cv::Mat &phase,
		const cv::Mat &modulation,
		const cv::Mat &mask,
		const float adjust);

	/**
    * @brief UnwrapSpanCalcTile       跨区解包裹法计算参考连通区的拟合面(平面)
    * @param unwrap_phase             参考区的解包裹后相位
    * @Param mask                     参考区的掩膜
    */
	cv::Mat UnwrapSpanCalcTile(const cv::Mat &unwrap_phase,
		const cv::Mat &mask);

	/*--------------------- DCT（待完善） -------------------------*/
	//void DctProcess(cv::Mat &phase,
	//	const cv::Mat &modulation,
	//	const cv::Mat &mask);

	/*--------------------- 质量图解包裹-------------------------*/
	/**
	* @brief UnwrapHistogram          质量图解包裹
	* @param phase                    包裹相位
	* @Param mask                     掩膜
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

