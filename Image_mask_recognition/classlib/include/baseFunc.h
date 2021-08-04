/*****************************************************************
*
* fileName:  baseFunc.h
*
* function:  存放基础函数
*
* author:    sjt&ztb（Steven）
*
* date:      2021.1.26
*
* notes:     存放基础函数模块

* Copyright: Shanghai Shineoptics Technology Co. , Ltd
*
*********************************************************************/

#pragma once
#ifndef BASEFUNC_H
#define BASEFUNC_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>    
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/core/core.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stdio.h>
#include <io.h>
#include <chrono> 
#include <time.h>
#include <ctime>
#include <direct.h>
#include <functional>
#include <omp.h>
#include <Windows.h>

using namespace std;
#pragma warning(disable:4996)

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

#define RAD_TO_ANGLE  57.29577951f               // 弧度转化为角度的系数
#define PI            3.14159265358979f          // pi
#define PI_2          6.28318530717959f          // 2pi

// 打印函数
#define TURN_ON_DEBUG                            // 打印总开关
#ifdef TURN_ON_DEBUG
#define TURN_ON_DEBUG_VAR                    // 打印变量的开关
#define TURN_ON_DEBUG_STR                    // 打印字符串的开关
#define TURN_ON_DEBUG_DETAILS                // 打印变量的详细信息的开关
// 打印变量
#ifdef TURN_ON_DEBUG_VAR
	#define DEBUG(_PARAMS_) std::cout << "func: " << __FUNCTION__ << ", " << #_PARAMS_ << " = " << _PARAMS_<< std::endl;
#else
	#define DEBUG(_PARAMS_);
#endif
// 打印字符串
#ifdef TURN_ON_DEBUG_STR
	#define DEBUG_STR(_PARAMS_) std::cout << "algorithm: " << _PARAMS_ << std::endl;
#else
	#define DEBUG_STR(_PARAMS_);
#endif
// 打印变量的详细信息
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

// 错误类型
enum ERROR_TYPE {
	ERROR_TYPE_NOT_ERROR = 0,             
	ERROR_TYPE_NO_ENOUGH_IMAGES=1,
	ERROR_TYPE_NO_MASK=2,
	ERROR_TYPE_ROI_TOO_SMALL=3,
	ERROR_TYPE_PSI_ERROR=4,
	ERROR_TYPE_UNWRAP_ERROR=5,
	ERROR_TYPE_ZERNIKE_ERROR=6,
	ERROR_TYPE_CHECK=7,
	ERROR_TYPE_SPIKES_TOO_MUCH=8,
	ERROR_TYPE_NOT_AUTOMASK=9,
	ERROR_TYPE_PIC_SIZE=10,
	ERROR_TYPE_NO_RESULT=11,
	ERROR_TYPE_SETTING=12,
	ERROR_TYPE_PIC_NUM=13,
	ERROR_TYPE_FILTER_TOO_LARGE=14,
	ERROR_TYPE_FILTER_TOO_SMALL=15,
	ERROR_TYPE_FILTER_ERROR=16,
};

// 基础函数
class BaseFunc {
public:
	/*--------------------- edgeDetector -------------------------*/
	/**
	 * @brief UnitCart                         产生指定范围内的指定数量点数，相邻数据跨度相同
	 * @param squaresize                       矩形的尺寸（正方形）
	 * @param x return CV_32FC1                列方向的向量
	 * @param y                                行方向的向量
	 */
	static void UnitCart(int squaresize, cv::Mat& x, cv::Mat& y) {
		CV_Assert(squaresize % 2 == 1);
		x.create(squaresize, squaresize, CV_32FC1);
		y.create(squaresize, squaresize, CV_32FC1);
		//设置边界
		x.col(0).setTo(-1.0);
		x.col(squaresize - 1).setTo(1.0f);
		y.row(0).setTo(1.0);
		y.row(squaresize - 1).setTo(-1.0f);

		float delta = 2.0f / (squaresize - 1.0f);  //两个元素的间隔

		//计算其他位置的值
		for (int i = 1; i < squaresize - 1; ++i) {
			x.col(i) = -1.0f + i * delta;
			y.row(i) = 1.0f - i * delta;
		}
	}

	/**
	 * @brief UnitPolar                        直角坐标转换为极坐标
	 * @param x                                UnitCart计算得到的x
	 * @param y                                UnitCart计算得到的y
	 * @param mag                              幅值
	 * @param ang                              角度
	 * @param indegree                         标志位（false：弧度，true:角度）
	 */
	static void UnitPolar(const cv::Mat& x, const cv::Mat& y,
		cv::Mat& mag, cv::Mat& ang, bool indegree = false) {
		//cv::cartToPolar(x, y, mag, ang, indegree);   //直角坐标转换为极坐标
		mag = cv::Mat(x.size(), x.type());
		ang = cv::Mat(x.size(), x.type());
		int row = mag.rows;
		int col = mag.cols;
		for (int i = 0; i < row; ++i)
		{
			for (int j = 0; j < col; ++j)
			{
				mag.at<float>(i, j) = sqrt(x.at<float>(i, j)*x.at<float>(i, j) + y.at<float>(i, j)*y.at<float>(i, j));
				ang.at<float>(i, j) = atan2(y.at<float>(i, j), x.at<float>(i, j));
			}
		}
	}

	/**
	 * @brief UnitPolar                        直角坐标转换为极坐标
	 * @param squaresize                       矩形的尺寸（正方形）
	 * @param mag                              幅值
	 * @param ang                              角度
	 * @param indegree                         标志位（false：弧度，true:角度）
	 */
	static void UnitPolar(int squaresize, cv::Mat& mag,
		cv::Mat& ang, bool indegree = false) {
		cv::Mat x;
		cv::Mat y;
		UnitCart(squaresize, x, y);                //产生指定范围内的指定数量点数，相邻数据跨度相同
		// OpenCV自带的转换有精度限制，导致结果有一定差异性
		//cv::cartToPolar(x, y, mag, ang, false); //坐标转换

		mag = cv::Mat(x.size(), x.type());
		ang = cv::Mat(x.size(), x.type());
		int row = mag.rows;
		int col = mag.cols;
		float *m, *a, *xx, *yy;
		for (int i = 0; i < row; ++i)
		{
			m = mag.ptr<float>(i);
			a = ang.ptr<float>(i);
			xx = x.ptr<float>(i);
			yy = y.ptr<float>(i);
			for (int j = 0; j < col; ++j)
			{
				m[j] = sqrt(xx[j] * xx[j] + yy[j] * yy[j]);
				a[j] = atan2(yy[j], xx[j]);
			}
		}

	}

	/**
	* @brief RectToChild                       获得矩形
	* @param r                                 矩形的尺寸（正方形）
	* @param roi                               幅值
	*/
	static cv::Rect RectToChild(const cv::Rect& r, const cv::Rect& roi) {
		return cv::Rect(r.x - roi.x, r.y - roi.y, r.width, r.height);
	}

	/**
	 * @brief UnitCircleSquare                 计算圆的外接矩形
	 * @param center                           UnitCircle 得到的 center
	 * @param radius                           UnitCircle 得到的 radius
	 * @return Rect                            外接矩形
	 * @note                                   不保证rect在mask大小内
	 */
	static cv::Rect UnitCircleSquare(const cv::Point& center, int& radius) {
		return cv::Rect(center.x - radius, center.y - radius, radius * 2 + 1, radius * 2 + 1);
	}

	/**
	 * @brief UnitRectCircleSquare             计算矩形的最小包围圆的外接矩形
	 * @param center                           UnitCircle 得到的 center
	 * @param radius                           UnitCircle 得到的 radius
	 * @return Rect                            外接矩形以及圆心和半径
	 * @note                                   不保证rect在mask大小内
	 */
	static cv::Rect UnitRectCircleSquare(cv::Rect rect, cv::Point& center, int& radius) {
		center.x = (int)(rect.x + rect.width / 2.0f);
		center.y = (int)(rect.y + rect.height / 2.0f);
		radius = (int)(std::sqrt(rect.width * rect.width + rect.height * rect.height) / 2.0f);
		return cv::Rect(center.x - radius, center.y - radius, radius * 2 + 1, radius * 2 + 1);
	}

	/**
	 * @brief UnitRotatedRectCircleSquare      计算旋转矩形的最小包围圆的外接矩形
	 * @param center                           UnitCircle 得到的 center
	 * @param radius                           UnitCircle 得到的 radius
	 * @return Rect                            外接矩形以及圆心和半径
	 * @note                                   不保证rotatedRect在mask大小内
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
	 * @brief UnitRotatedRectCircleSquare      计算旋转矩形的最小包围圆的外接矩形
	 * @param center                           UnitCircle 得到的 center
	 * @param radius                           UnitCircle 得到的 radius
	 * @param chamferRadius                    倒角矩形的倒角半径
	 * @return Rect                            外接矩形
	 * @note                                   不保证rotatedRect在mask大小内
	 */
	static cv::Rect UnitChamferRectCircleSquare(
		cv::RotatedRect rotatedRect, cv::Point& center,
		int& radius, float chamferRadius) {
		center.x = (int)rotatedRect.center.x;
		center.y = (int)rotatedRect.center.y;

		float newWidth = rotatedRect.size.width - chamferRadius * 2.0f;
		float newHeight = rotatedRect.size.height - chamferRadius * 2.0f;

		radius = (int)(std::sqrtf(newWidth * newWidth + newHeight * newHeight) / 2.0f + chamferRadius);
		return cv::Rect(center.x - radius, center.y - radius, radius * 2 + 1, radius * 2 + 1);
	}

	/*--------------------- zernike -------------------------*/
	/**
	 * @brief GridSampling                     设置采样的位置点
	 * @param size                             重采样的范围尺寸
	 * @param rowinterval                      行间隔
	 * @param colinterval                      列间隔
	 * @return                                 重采样的mask
	 */
	static cv::Mat GridSampling(const cv::Size& size, int rowinterval, int colinterval) {
		cv::Mat dst(size, CV_8UC1, cv::Scalar(0));
		//设置采样的位置点
		int Row = dst.rows;
		int Col = dst.cols;
		for (int row = 0; row < Row; row += rowinterval) {
			for (int col = 0; col < Col; col += colinterval) {
				dst.at<uchar>(row, col) = 255;
			}
		}
		return dst;
	}

	//计算转换矩阵时的极坐标相位
	static cv::Mat InitialAng129() {
		cv::Mat ang;
		cv::Mat mag;
		UnitPolar(129, mag, ang);
		return ang;
	}

	//计算转换矩阵时的极坐标幅度
	static cv::Mat InitialMag129() {
		cv::Mat ang;
		cv::Mat mag;
		UnitPolar(129, mag, ang);
		return mag;
	}

	//计算转换矩阵时的极坐标相位
	static cv::Mat InitialAng259() {
		cv::Mat ang;
		cv::Mat mag;
		UnitPolar(259, mag, ang);
		return ang;
	}

	//计算转换矩阵时的极坐标幅度
	static cv::Mat InitialMag259() {
		cv::Mat ang;
		cv::Mat mag;
		UnitPolar(259, mag, ang);
		return mag;
	}

	/*--------------------- Image Processing functions -------------------------*/
	/**
	 * @brief MorphOpenMask                     为消除孤立的小点，毛刺和小桥影响，对mask进行开运算
	 * @param mask                              mask
	 * @param shape                             形状
	 * @param times                             迭代次数
	 * @return
	 */
	static void MorphOpenMask(cv::Mat &mask, int shape, int size, int times)
	{
		//获得shape形，大小为size*size的模板
		cv::Mat element = cv::getStructuringElement(shape, cv::Size(size, size));
		//进行开运算，迭代times次
		morphologyEx(mask, mask, cv::MORPH_OPEN, element, cv::Point(-1, -1), times);
	}

	/**
	 * @brief MorphErodeMask                    腐蚀操作
	 * @param mask                              mask
	 * @param shape                             形状
	 * @param times                             迭代次数
	 * @return
	 */
	static void MorphErodeMask(cv::Mat &mask, int shape, int size, int times)
	{
		//获得shape形，大小为size*size的模板
		cv::Mat element = cv::getStructuringElement(shape, cv::Size(size, size));
		//进行开运算，迭代times次
		morphologyEx(mask, mask, cv::MORPH_ERODE, element, cv::Point(-1, -1), times);
	}

	/**
	 * @brief Rotate                            图像旋转
	 * @param srcImage                          待旋转图像
	 * @param dstImage                          旋转后的图像
	 * @param angle                             旋转角度（正数为逆时针，负数为顺时针）
	 * @return
	 */
	static void Rotate(const cv::Mat &srcImage, cv::Mat &dstImage, double angle)
	{
		cv::Point2f center(srcImage.cols / 2.0f, srcImage.rows / 2.0f);//中心
		cv::Mat M = cv::getRotationMatrix2D(center, angle, 1);//计算旋转的仿射变换矩阵 
		cv::warpAffine(srcImage, dstImage, M, cv::Size(srcImage.cols, srcImage.rows));//仿射变换  
	}

	/**
	 * @brief Rotate2                           图像旋转
	 * @param srcImage                          待旋转图像
	 * @param dstImage                          旋转后的图像
	 * @param angle                             旋转角度（正数为逆时针，负数为顺时针）
	 * @param center                            旋转中心
	 * @return
	 */
	static void Rotate2(const cv::Mat &srcImage, cv::Mat &dstImage, double angle, cv::Point2f center)
	{
		cv::Mat M = cv::getRotationMatrix2D(center, angle, 1);//计算旋转的仿射变换矩阵 
		cv::warpAffine(srcImage, dstImage, M, cv::Size(srcImage.cols, srcImage.rows));//仿射变换  
	}

	/**
	 * @brief FindTestArea                      寻找测试区域(图像错位相加再叠加)
	 * @param images                            图像组
	 * @param mask                              掩膜
	 * @return                                  测试区域
	 */
	static cv::Mat FindTestArea(vector<cv::Mat> images, const cv::Mat mask)
	{
		CV_Assert(images.size() > 3);
		// 克隆，防止对原始数据破坏
		cv::Mat src0 = images[0].clone();
		cv::Mat src1 = images[1].clone();
		cv::Mat src2 = images[2].clone();
		cv::Mat src3 = images[3].clone();
		cv::Mat element = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
		// 开运算是为了让图像整体性提高
		cv::morphologyEx(src0, src0, cv::MORPH_OPEN, element, { -1,-1 }, 1);
		cv::morphologyEx(src1, src1, cv::MORPH_OPEN, element, { -1,-1 }, 1);
		cv::morphologyEx(src2, src2, cv::MORPH_OPEN, element, { -1,-1 }, 1);
		cv::morphologyEx(src3, src3, cv::MORPH_OPEN, element, { -1,-1 }, 1);

		cv::Mat abs_sub_13, abs_sub_24;
		abs_sub_13 = abs(src0 - src2);
		abs_sub_24 = abs(src1 - src3);
		cv::Mat dst = abs_sub_13 + abs_sub_24;
		dst.convertTo(dst, CV_8UC1);
		cv::medianBlur(dst, dst, 3);
		cv::threshold(dst, dst, 100, 255, cv::THRESH_TRIANGLE);      //阈值运算(三角法)
		dst = dst & mask;
		return dst;
	}

	/**
	 * @brief calcBRMS                          计算BRMS-图像亮度均方值
	 * @param images                            图组
	 * @param mask                              掩膜
	 * @return                                  SRMS
	 */
	static float calcBRMS(vector<cv::Mat> images, cv::Mat mask)
	{
		vector<cv::Point> points;
		cv::findNonZero(mask, points);
		if (points.size() == 0)
		{
			return -1;
		}
		int row = mask.rows;
		int col = mask.cols;
		float BRMSsum = 0.0f;
		cv::Mat temp;
		int number_size = (int)(images.size());
		for (int i = 0; i < number_size; ++i)
		{
			temp= images[i].clone();
			for (int m = 0; m < row; ++m)
			{
				for (int n = 0; n < col; ++n)
				{
					if (mask.at<uchar>(m, n) == 255)
					{
						temp.at<float>(m, n) = pow(temp.at<float>(m, n), 2);
					}
				}
			}
			BRMSsum += std::sqrt((float)(cv::mean(temp, mask == 255)[0]));
		}
		float BRMS = BRMSsum / (int)(images.size());
		return BRMS;
	}

	/**
     * @brief calcBRMS_Mean                     计算calcBRMS_Mean-图像亮度均方值除以平均值
     * @param images                            图组
     * @param mask                              掩膜
     * @return                                  SRMS
     */
	static float calcBRMS_Mean(vector<cv::Mat> images, cv::Mat mask)
	{
		vector<cv::Point> points;
		cv::findNonZero(mask, points);
		if (points.size() == 0)
		{
			return -1;
		}
		int row = mask.rows;
		int col = mask.cols;
		float BRMS_Meansum = 0.0f;
		cv::Mat temp;
		int number_size = (int)(images.size());
		for (int i = 0; i < number_size; ++i)
		{
			float average = (float)(cv::mean(images[i], mask == 255)[0]);
			temp = images[i].clone();
			for (int m = 0; m < row; ++m)
			{
				for (int n = 0; n < col; ++n)
				{
					if (mask.at<uchar>(m, n) == 255)
					{
						temp.at<float>(m, n) = pow(temp.at<float>(m, n), 2);
					}
				}
			}
			BRMS_Meansum += std::sqrt((float)(cv::mean(temp, mask == 255)[0]))/average;
			cout << "rms:" << std::sqrt((float)(cv::mean(temp, mask == 255)[0])) << endl;
			cout << "average:" << average << endl;
		}
		float BRMS_Mean = BRMS_Meansum / (int)(images.size());
		return BRMS_Mean;
	}

	/**
	 * @brief calcBRMS_Median                   计算calcBRMS_Median-图像亮度均方值除以中值
	 * @param images                            图组
	 * @param mask                              掩膜
	 * @return                                  SRMS
	 */
	static float calcBRMS_Median(vector<cv::Mat> images, cv::Mat mask)
	{
		vector<cv::Point> points;
		cv::findNonZero(mask, points);
		if (points.size() == 0)
		{
			return -1;
		}
		int row = mask.rows;
		int col = mask.cols;
		float BRMS_Mediansum = 0.0f;
		cv::Mat temp;
		int number_size = (int)(images.size());
		for (int i = 0; i < number_size; ++i)
		{
			temp = images[i].clone();
			cv::Mat tmp;
			for (int m = 0; m < row; ++m)
			{
				for (int n = 0; n < col; ++n)
				{
					if (mask.at<uchar>(m, n) == 255)
					{
						tmp.push_back(temp.at<float>(m, n));
					}
				}
			}
			cv::Mat sorted; 
			cv::sort(tmp, sorted, cv::SORT_EVERY_COLUMN+cv::SORT_ASCENDING);
			float median = sorted.at<float>(sorted.rows / 2);
			for (int m = 0; m < row; ++m)
			{
				for (int n = 0; n < col; ++n)
				{
					if (mask.at<uchar>(m, n) == 255)
					{
						temp.at<float>(m, n) = pow(temp.at<float>(m, n), 2);
					}
				}
			}
			BRMS_Mediansum += std::sqrt((float)(cv::mean(temp, mask == 255)[0]))/median;
		}
		float BRMS_Median = BRMS_Mediansum / (int)(images.size());
		return BRMS_Median;
	}

	/**
	 * @brief Hanning                          汉宁窗（非0开始,默认参数0.5）
	 * @param number                           窗尺寸
	 * @param a                                窗参数
	 * @return                                 窗数组
	 */
	static vector<float> Hanning(int number,float a=0.5)
	{
		vector<float> result;
		for (int i = 0; i < number; i++)
		{
			float w = (1 - a) - a*cos(2 * PI*(i + 1) / (number + 1));
			result.emplace_back(w);
		}
		return result;
	}

	/**
	 * @brief QuadFlip                         四象翻转法（傅里叶变换用）
	 * @param picture                          输入图像
	 * @return                                 输出
	 */
	static cv::Mat QuadFlip(const cv::Mat &picture)
	{
		cv::Mat result(picture.rows * 2, picture.cols * 2, picture.type());
		cv::Mat Upperleft, Lowerleft, Upperright, Lowerright;
		Upperleft = picture.clone();
		cv::flip(picture, Upperright, 1);//即沿Y轴翻转
		cv::flip(picture, Lowerleft, 0);//即沿X轴翻转
		cv::flip(picture, Lowerright, -1);//即沿Y轴翻转
		Upperleft.copyTo(result(cv::Range(0, picture.rows), cv::Range(0, picture.cols)));
		Lowerleft.copyTo(result(cv::Range(picture.rows, 2*picture.rows), cv::Range(0, picture.cols)));
		Upperright.copyTo(result(cv::Range(0, picture.rows), cv::Range(picture.cols, 2*picture.cols)));
		Lowerright.copyTo(result(cv::Range(picture.rows, 2*picture.rows), cv::Range(picture.cols, 2*picture.cols)));
		return result;
	}

	/**
	 * @brief MatExpand                        扩展图像（原图居于中心，扩展区为nan值）
	 * @param picture                          输入图像
	 * @param level                            扩展倍数
 	 * @return                                 输出
	 */
	static cv::Mat MatExpand(const cv::Mat &picture,float level)
	{
		cv::Mat result(int(picture.rows * level), int(picture.cols * level), picture.type(), nan(""));
		cv::Mat Upperleft;
		Upperleft = picture.clone();
		Upperleft.copyTo(result(cv::Range(int((level-1) * picture.rows / 2), int((level + 1) * picture.rows / 2)), cv::Range(int((level - 1) * picture.cols / 2), int((level + 1) * picture.cols / 2))));
		return result;
	}

	/**
	 * @brief GrayToColor                      灰度图上色
	 * @param phase                            输入的灰色图像，通道为1
	 * @return                                 上色后的图像
	 */
	static cv::Mat GrayToColor(cv::Mat &phase)
	{
		CV_Assert(phase.channels() == 1);

		if (phase.empty())
		{
			cv::Mat result = cv::Mat::zeros(10,10, CV_8UC3);
			return result;
		}
		cv::Mat temp, result, mask;
		// 将灰度图重新归一化至0-255
		cv::normalize(phase, temp, 255, 0, cv::NORM_MINMAX);
		temp.convertTo(temp, CV_8UC1);
		// 创建掩膜，目的是为了隔离nan值的干扰
		mask = cv::Mat::zeros(phase.size(), CV_8UC1);
		mask.setTo(255, phase == phase);

		// 初始化三通道颜色图
		cv::Mat color1, color2, color3;
		color1 = cv::Mat::zeros(temp.size(), temp.type());
		color2 = cv::Mat::zeros(temp.size(), temp.type());
		color3 = cv::Mat::zeros(temp.size(), temp.type());
		int row = phase.rows;
		int col = phase.cols;

		// 基于灰度图的灰度层级，给其上色，最底的灰度值0为蓝色（255，0,0），最高的灰度值255为红色（0,0,255），中间的灰度值127为绿色（0,255,0）
		for (int i = 0; i < row; ++i)
		{
			uchar *c1 = color1.ptr<uchar>(i);
			uchar *c2 = color2.ptr<uchar>(i);
			uchar *c3 = color3.ptr<uchar>(i);
			uchar *r = temp.ptr<uchar>(i);
			uchar *m = mask.ptr<uchar>(i);
			for (int j = 0; j < col; ++j)
			{
				if (m[j] == 255)
				{
					if (r[j] > (3 * 255 / 4) && r[j] <= 255)
					{
						c1[j] = 255;
						c2[j] = 4 * (255 - r[j]);
						c3[j] = 0;
					}
					else if (r[j] <= (3 * 255 / 4) && r[j] > (255 / 2))
					{
						c1[j] = 255 - 4 * (3 * 255 / 4 - r[j]);
						c2[j] = 255;
						c3[j] = 0;
					}
					else if (r[j] <= (255 / 2) && r[j] > (255 / 4))
					{
						c1[j] = 0;
						c2[j] = 255;
						c3[j] = 4 * (255 / 2 - r[j]);
					}
					else if (r[j] <= (255 / 4) && r[j] >= 0)
					{
						c1[j] = 0;
						c2[j] = 255 - 4 * (255 / 4 - r[j]);
						c3[j] = 255;
					}
					else {
						c1[j] = 0;
						c2[j] = 0;
						c3[j] = 0;
					}
				}
			}
		}

		// 三通道合并，得到颜色图
		vector<cv::Mat> images;
		images.push_back(color3);
		images.push_back(color2);
		images.push_back(color1);
		cv::merge(images, result);
		
		return result;
	}

	/**
	 * @brief GrayToColorFromOther             灰度图上色，基于另一灰度图的色板
	 * @param phase1                           输入的灰色图像，通道为1，提供色板
	 * @param phase2                           输入的灰色图像，通道为1，基于phase1的色板绘色
	 * @return                                 上色后的图像
	 */
	static cv::Mat GrayToColorFromOther(cv::Mat &phase1,cv::Mat &phase2)
	{
		CV_Assert(phase1.channels() == 1);
		CV_Assert(phase2.channels() == 1);
		if (phase1.empty()|| phase2.empty())
		{
			cv::Mat result = cv::Mat::zeros(10, 10, CV_8UC3);
			return result;
		}
		cv::Mat temp, result, mask;
		double max1, min1;
		int row = phase2.rows;
		int col = phase2.cols;
		cv::minMaxIdx(phase1, &min1, &max1, nullptr, nullptr, phase1 == phase1);
		// 将灰度图重新归一化至0-255
		temp = phase2.clone();
		for (int i = 0; i < row; ++i)
		{
			float *t2 = temp.ptr<float>(i);
			for (int j = 0; j < col; ++j)
			{
				t2[j] = 255.0f*(phase2.at<float>(i, j) - (float)min1) / ((float)max1 - (float)min1);
			}
		}
		temp.convertTo(temp, CV_8UC1);
		// 创建掩膜，目的是为了隔离nan值的干扰
		mask = cv::Mat::zeros(phase2.size(), CV_8UC1);
		mask.setTo(255, phase2 == phase2);

		// 初始化三通道颜色图
		cv::Mat color1, color2, color3;
		color1 = cv::Mat::zeros(temp.size(), temp.type());
		color2 = cv::Mat::zeros(temp.size(), temp.type());
		color3 = cv::Mat::zeros(temp.size(), temp.type());

		// 基于灰度图的灰度层级，给其上色，最底的灰度值0为蓝色（255，0,0），最高的灰度值255为红色（0,0,255），中间的灰度值127为绿色（0,255,0）
		for (int i = 0; i < row; ++i)
		{
			uchar *c1 = color1.ptr<uchar>(i);
			uchar *c2 = color2.ptr<uchar>(i);
			uchar *c3 = color3.ptr<uchar>(i);
			uchar *r = temp.ptr<uchar>(i);
			uchar *m = mask.ptr<uchar>(i);
			for (int j = 0; j < col; ++j)
			{
				if (m[j] == 255)
				{
					if (r[j] > (3 * 255 / 4) && r[j] <= 255)
					{
						c1[j] = 255;
						c2[j] = 4 * (255 - r[j]);
						c3[j] = 0;
					}
					else if (r[j] <= (3 * 255 / 4) && r[j] > (255 / 2))
					{
						c1[j] = 255 - 4 * (3 * 255 / 4 - r[j]);
						c2[j] = 255;
						c3[j] = 0;
					}
					else if (r[j] <= (255 / 2) && r[j] > (255 / 4))
					{
						c1[j] = 0;
						c2[j] = 255;
						c3[j] = 4 * (255 / 2 - r[j]);
					}
					else if (r[j] <= (255 / 4) && r[j] >= 0)
					{
						c1[j] = 0;
						c2[j] = 255 - 4 * (255 / 4 - r[j]);
						c3[j] = 255;
					}
					else {
						c1[j] = 0;
						c2[j] = 0;
						c3[j] = 0;
					}
				}
			}
		}

		// 三通道合并，得到颜色图
		vector<cv::Mat> images;
		images.push_back(color3);
		images.push_back(color2);
		images.push_back(color1);
		cv::merge(images, result);

		return result;
	}

	/*--------------------- other functions -------------------------*/
	// 计算pv
	static float getPv(cv::Mat src) {
		double minv = 0;
		double maxv = 0;
		float pv = 0.0f;
		cv::minMaxIdx(src, &minv, &maxv, nullptr, nullptr, src == src);
		pv = (float)(maxv - minv);
		return pv;
	}

	// 计算rms
	static float getRms(cv::Mat src) {
		cv::Mat tmp;
		float rms = 0.0f;
		cv::pow(src, 2.0, tmp);
		float mean_tmp = (float)(cv::mean(tmp, tmp == tmp)[0]);     // 计算均值(nan != nan)
		rms = std::sqrt(mean_tmp);          //得到rms

		return rms;
	}

	// 计算标准差
	static  float getStdDev(std::vector<float> input) {
		float sum = 0.0f;
		std::for_each(std::begin(input), std::end(input), [&](const float d) {
			sum += d;
		});
		float mean = sum / input.size(); //均值
		float accum = 0.0f;
		std::for_each(std::begin(input), std::end(input), [&](const float d) {
			accum += (d - mean)*(d - mean);
		});
		float stdev = sqrt(accum / (input.size()-1)); 
		return stdev;
	}

	// 计算2sigma
	static float getTwoSigma(std::vector<float> input) {
		float std = getStdDev(input);
		return (2 * std);
	}

	/**
	 * @brief analysisDoubleSpherSur            计算被测面和标准面的绝对误差（双球面绝对检测）
	 * @param fittedSurface                     拟合曲面(M4:猫眼点, M0:共焦点0度, M1:90度, M2:180度, M3:270度)
	 * @param pv                                pv(0:被测面, 1:标准面)
	 * @param rms                               rms(0:被测面, 1:标准面)
	 * @param testSurError                      被测面误差
	 * @param standSurError                     标准面误差
	 * @return                                  
	 */
	static void analysisDoubleSpherSur(std::vector<cv::Mat> fittedSurface,
		float pv[2], float rms[2], cv::Mat &testSurError, cv::Mat &standSurError) {
		CV_Assert(fittedSurface.size() == 5);

		cv::Mat ret;
		// 对猫眼点进行顺时针旋转
		cv::Mat M4_90, M4_180, M4_270;            // 顺时针旋转
		Rotate(fittedSurface[0], M4_90, -90);     // 顺时针旋转
		Rotate(fittedSurface[0], M4_180, -180);   // 顺时针旋转
		Rotate(fittedSurface[0], M4_270, -270);   // 顺时针旋转
		// 对共焦点进行逆时针旋转
		cv::Mat M1_90, M2_180, M3_270;            // 逆时针旋转
		Rotate(fittedSurface[2], M1_90, 90);      // 逆时针旋转
		Rotate(fittedSurface[3], M2_180, 180);    // 逆时针旋转
		Rotate(fittedSurface[4], M3_270, 270);    // 逆时针旋转

		// 计算被测面误差及其pv、rms
		testSurError = (fittedSurface[1] + M1_90 + M2_180 + M3_270
			- fittedSurface[0] - M4_90 - M4_180 - M4_270) / 4.0f;
		pv[0] = getPv(testSurError);
		rms[0] = getRms(testSurError);
		// 计算标准面误差及其pv、rms
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
	 * @brief calcOpticalHomogeneity            计算光学均匀性
	 * @param refractiveIndex                   折射率
	 * @param thickness				           器件厚度
	 * @param unitType                          单位
	 * @param testWaveLength                    波长
	 * @param fittedSurface                     拟合曲面(C,T,S1,S2)空腔，透过，前表面，后表面
	 * @param pvh                               pvh(pv除以厚度)
	 * @param rmsh                              rmsh(rms除以厚度)
	 * @param pv                                pv
	 * @param rms                               rms
	 * @return
	 */
	static int calcOpticalHomogeneity(float refractiveIndex, float thickness, int unitType,
		float testWaveLength, std::vector<cv::Mat> &fittedSurface, float *pvh, float *rmsh,float *pv,float *rms,cv::Mat &result)
	{
	    if (fittedSurface.size() < 4) {
			return 1;                            //均匀性图组中图片个数不够
		}
		
		cv::Mat C = fittedSurface[0];
		cv::Mat T = fittedSurface[1];
		cv::Mat S1 = fittedSurface[2];
		cv::Mat S2 = fittedSurface[3];

		if (C.empty() || T.empty() || S1.empty() || S2.empty())
			return 2;                            //存在空图

		if (C.size() != T.size() || C.size() != S1.size() || C.size() != S2.size())
			return 3;                            //大小不一致

		cv::Mat deltaN = cv::Mat();
		deltaN = (refractiveIndex * (T - C) - (refractiveIndex - 1) * (S2 - S1));

		*pv = getPv(deltaN);
		*rms = getRms(deltaN);

		float coef = 1.0f;
		switch (unitType)
		{
		case 0:									 // UNIT_TYPE_WAVE, 以波长为单位
			coef = testWaveLength / 1000000.0f;
			break;
		case 1:									 // UNIT_TYPE_FR, 以Fr为单位
			coef = testWaveLength / 2000000.0f;
			break;
		case 2:                                  // UNIT_TYPE_NM, 以nm为单位
			coef = 1 / 1000000.0f;
			break;
		case 3:                                  // UNIT_TYPE_UM, 以um为单位
			coef = 1 / 1000.0f;;
			break;
		case 4:                                  // UNIT_TYPE_MM,以mm为单位:
			coef = 1.0f;
			break;
		default:
			coef = testWaveLength / 1000000.0f;
			break;
		}

		*pvh = *pv * coef / thickness;
		*rmsh = *rms * coef / thickness;
		deltaN.copyTo(result);
		return 0;                               //光学均匀性模块正常
	}

	/**
	 * @brief calculator_add                    相位计算器—加法
	 * @param inImage1                          输入位相图1
	 * @param inImage2                          输入位相图2
	 * @return                                  结果图
	 */
	static cv::Mat calculator_add(const cv::Mat &inImage1,
		const cv::Mat &inImage2) {
		CV_Assert(inImage1.type() == inImage2.type());
		CV_Assert(inImage1.size() == inImage2.size());
		return inImage1 + inImage2;
	}

	/**
	 * @brief calculator_sub                    相位计算器—减法
	 * @param inImage1                          输入位相图1
	 * @param inImage2                          输入位相图2
	 * @return                                  结果图
	 */
	static cv::Mat calculator_sub(const cv::Mat &inImage1,
		const cv::Mat &inImage2) {
		CV_Assert(inImage1.type() == inImage2.type());
		CV_Assert(inImage1.size() == inImage2.size());
		return inImage1 - inImage2;
	}

	/**
	 * @brief calculator_mul                    相位计算器—乘法
	 * @param inImage                           输入位相图
	 * @param scale                             乘法系数
	 * @return                                  结果图
	 */
	static cv::Mat calculator_mul(const cv::Mat &inImage, const float scale) {
		return scale * inImage;
	}

	/**
	 * @brief calculator_rev                    相位计算器—反转
	 * @param inImage                           输入位相图
	 * @return                                  结果图
	 */
	static cv::Mat calculator_rev(const cv::Mat &inImage) {
		return -1 * inImage;
	}

	/**
	 * @brief calculator_rotate                 相位计算器—旋转
	 * @param inImage                           输入位相图
	 * @param angle                             角度
	 * @return                                  结果图
	 */
	static cv::Mat calculator_rotate(const cv::Mat &inImage, cv::Mat &mask, const float angle) {
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
				if (temp[j] != 0)
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
		Rotate2(inImage, out, angle, center);
		return out;
	}
	
	/*------------------------ math ----------------------------*/
	//按第一行展开计算3*3|A|
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
	
	//计算每一行每一列的每个元素所对应的余子式，组成A*
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

				ans[j][i] = getA(temp, n - 1);  //此处顺便进行了转置
				if ((i + j) % 2 == 1)
				{
					ans[j][i] = -ans[j][i];
				}
			}
		}
	}
	
	//得到给定矩阵src的逆矩阵保存到des中(3*3)。
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

	/*--------------------- file functions -------------------------*/
	/**
	 * @brief GetProgramDir                     获取当前工作目录路径
	 * @return                                  路径
	 */
	static string GetProgramDir()
	{
		char exeFullPath[MAX_PATH]; // Full path 
		string strPath = "";

		GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);
		strPath = (string)exeFullPath;    // Get full path of the file 

		int pos = static_cast<int>(strPath.find_last_of('\\', strPath.length()));
		return strPath.substr(0, pos);  // Return the directory without the file name 
	}

	/**
	 * @brief DatetimeToString                  获取当前时间
	 * @return                                  时间
	 */
	static std::string DatetimeToString(tm tm_in)
	{
		tm *tm_ = &tm_in;						  // 将time_t格式转换为tm结构体
		int year, month, day, hour, minute, second;// 定义时间的各个int临时变量。
		year = tm_->tm_year + 1900;                // 临时变量，年，由于tm结构体存储的是从1900年开始的时间，所以临时变量int为tm_year加上1900。
		month = tm_->tm_mon + 1;                   // 临时变量，月，由于tm结构体的月份存储范围为0-11，所以临时变量int为tm_mon加上1。
		day = tm_->tm_mday;
		hour = tm_->tm_hour;
		minute = tm_->tm_min;
		second = tm_->tm_sec;

		char s[20];                                // 定义总日期时间char*变量。
		sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d—— ", year, month, day, hour, minute, second);// 将年月日时分秒合并。
		std::string str(s);                       // 定义string变量，并将总日期时间char*变量作为构造函数的参数传入。
		return std::move(str);                    // 返回转换日期时间后的string变量。
	}

	/**
	 * @brief WriteLog                          写进日志
     * @param msg                               内容
	 * @return                                  状态码
	 */
	static int WriteLog(string msg)
	{
		struct tm *local;
		time_t t;
		t = time(NULL);
		local = localtime(&t);
		string dtime = DatetimeToString(*local);
		ofstream outfile;
		outfile.open(GetProgramDir() + "\\" + "algorithm-log.txt", ios::app); //文件的物理地址，文件的打开方式
		if (outfile.is_open())
		{
			outfile << dtime << msg << "\n";
			outfile.close();
			return 0;
		}
		else
		{
			return 1;
		}
	}

};

namespace calc {
	/*--------------------- base functions -------------------------*/
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

	/*--------------------- Image Processing functions -------------------------*/
	/**
	 * @brief  bitwise_and()同 &,  opencv自带的按位与的函数，
	 * @param  src1                输入图像1
	 * @param  src2                输入图像2
	 * @param  mask                掩膜
	 * @return dst                 输出结果即 dst = (src1 & src2) && mask != 0
	 */
	inline cv::Mat bitwise_and(cv::InputArray src1, cv::InputArray src2,
		cv::InputArray mask = cv::noArray()) {
		cv::Mat dst;
		cv::bitwise_and(src1, src2, dst, mask);
		return dst;
	}

	/**
	 * @brief  bitwise_or()同 |,   opencv自带的按位或的函数，
	 * @param  src1                输入图像1
	 * @param  src2                输入图像2
	 * @param  mask                掩膜
	 * @return dst                 输出结果即 dst = (src1 | src2) && mask != 0
	 */
	inline cv::Mat bitwise_or(cv::InputArray src1, cv::InputArray src2,
		cv::InputArray mask = cv::noArray()) {
		cv::Mat dst;
		cv::bitwise_or(src1, src2, dst, mask);
		return dst;
	}

	/**
	 * @brief  bitwise_xor()同 ^,  opencv自带的按位异或的函数，0⊕0=0，1⊕0=1，0⊕1=1，1⊕1=0(同为0，异为1)
	 * @param  src1                输入图像1
	 * @param  src2                输入图像2
	 * @param  mask                掩膜
	 * @return dst                 输出结果即 dst = (src1 ^ src2) && mask != 0
	 */
	inline cv::Mat bitwise_xor(cv::InputArray src1, cv::InputArray src2,
		cv::InputArray mask = cv::noArray()) {
		cv::Mat dst;
		cv::bitwise_xor(src1, src2, dst, mask);
		return dst;
	}

	/**
	 * @brief  bitwise_not 同 ~,   opencv自带的非函数，
	 * @param  src                 输入图像
	 * @param  mask                掩膜
	 * @return dst                 输出结果即 dst = (~src) && mask != 0
	 */
	inline cv::Mat bitwise_not(cv::InputArray src,
		cv::InputArray mask = cv::noArray()) {
		cv::Mat dst;
		cv::bitwise_not(src, dst, mask);
		return dst;
	}

	/**
	 * @brief  SetToNan,           将src中的非mask区域设置为NaN
	 * @param  src                 输入图像
	 * @return mask                mask
	 */
	inline void SetToNan(cv::Mat& src, const cv::Mat& mask) {
		CV_Assert(src.type() == CV_32FC1);
		cv::Mat _nan(src.size(), src.type(), nan(""));
		_nan.setTo(0, ~mask);
		src = src + _nan;
	}

	/**
	 * @brief  find,               查找满足条件的索引，并将结果放在idx中
	 * @param  condition           输入条件
	 * @return idx                 输出结果,
	 */
	inline void find(const cv::Mat& condition,
		std::vector<cv::Point>& idx) {
		cv::findNonZero(condition, idx);  //Returns the list of locations of non-zero pixels
	}

	/**
	 * @brief  find_nan,           查找所有为NaN的元素，并将其索引放在idx中，(NaN != NaN) = true
	 * @param  src                 输入数据
	 * @return idx                 输出结果,
	 */
	inline void find_nan(const cv::Mat& src,
		std::vector<cv::Point>& idx) {
		cv::findNonZero(~(src == src), idx);  //Returns the list of locations of non-zero pixels
	}

	/**
	 * @brief  find_notnan,        查找所有不为NaN的元素，并将其索引放在idx中，(NaN == NaN) = false
	 * @param  src                 输入数据
	 * @return idx                 输出结果,
	 */
	inline void find_notnan(const cv::Mat& src,
		std::vector<cv::Point>& idx) {
		cv::findNonZero(src == src, idx);  //Returns the list of locations of non-zero pixels
	}

	template <typename T>
	/**
	 * @brief  get in a col         将src中，idx区域转化为一个列矩阵
	 * @param  src                  输入数据
	 * @param  idx                  待处理区域的索引
	 * @return dst                  列矩阵
	 */
	inline cv::Mat get(const cv::Mat& src,
		const std::vector<cv::Point>& idx) {
		CV_Assert(cv::DataType<T>::type == src.type());
		int num = (int)idx.size();
		cv::Mat dst(num, 1, src.type());

		/* pragma omp parallel for 是OpenMP中的一个指令，
		表示接下来的for循环将被多线程执行，另外每次循环之间不能有关系 */
#pragma omp parallel for
		for (int i = 0; i < num; ++i) {
				dst.at<T>(i, 0) = src.at<T>(idx[i]);
		}
		return dst;
	}

	/**
	 * @brief  get_notnan in a col  将所有不为NaN的元素转化为一个列向量
	 * @param  src CV_32FC1         输入数据
	 * @return dst                  列向量
	 */
	inline cv::Mat get_notnan(const cv::Mat& src) {
		CV_Assert(src.type() == CV_32FC1);
		std::vector<cv::Point> idx;

		//查找所有不为NaN的元素，并将其索引放在idx中
		find_notnan(src, idx);
		cv::Mat dst((int)(idx.size()), 1, src.type());
		for (int i = 0; i != idx.size(); ++i) {
			dst.at<float>(i, 0) = src.at<float>(idx[i]); //转化为列向量
		}
		return dst;
	}

	//删除面积过小的连通域
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
	
	/*--------------------- math -------------------------*/
	/**
	 * @brief  sqrt,               计算矩阵中每个元素的平方根，
	 * @param  src                 输入图像
	 * @return dst                 输出结果即 dst = sqrt(src)
	 */
	inline cv::Mat sqrt(cv::InputArray src) {
		cv::Mat dst;
		cv::sqrt(src, dst);
		return dst;
	}

	/**
	 * @brief  pow,                对矩阵逐元素取power次幂,若power为整数，直接进行幂运算；
	 *                             否则，先计算原矩阵的绝对值，再进行power次幂
	 * @param  src                 输入图像
	 * @param  power               power次幂
	 * @return dst                 输出结果即 dst = (src)^power
	 */
	inline cv::Mat pow(cv::InputArray src, double power) {
		cv::Mat dst;
		cv::pow(src, power, dst);
		return dst;
	}

	/**
	 * @brief  exp,                求src中所有元素的指数，并将结果放在dst中
	 * @param  src                 输入图像
	 * @return dst                 输出结果,即 dst = e^src
	 */
	inline cv::Mat exp(cv::InputArray src) {
		cv::Mat dst;
		cv::exp(src, dst);
		return dst;
	}

	/**
	 * @brief  exp,                求src中所有元素的指数，并将结果放在dst中
	 * @param  src                 输入图像
	 * @return dst                 输出结果,即 dst = e^src
	 */
	inline cv::Mat angle(cv::Mat src) {

		CV_Assert(src.type() == CV_32FC2);
		//创建双通道矩阵，分别存放实部和虚部
		cv::Mat plane[] = { cv::Mat::zeros(src.size() , CV_32FC1) , cv::Mat::zeros(src.size() , CV_32FC1) };
		// 分离通道（数组分离）
		cv::split(src, plane);

		cv::Mat dst(src.size(), CV_32FC1);
		int cols = src.cols;
		int rows = src.rows;
		//返回bool值，判断存储是否连续。
		if (src.isContinuous() && dst.isContinuous())
		{
			cols *= rows;
			rows = 1;
		}
		//计算每个元素的log10
		for (int i = 0; i < rows; i++)
		{
			const float* imag = plane[1].ptr<float>(i);
			const float* real = plane[0].ptr<float>(i);
			float* dsti = dst.ptr<float>(i);
			for (int j = 0; j < cols; j++) {
				dsti[j] = std::atan2(imag[j],real[j]);
			}
		}
		return dst;
	}

	/**
	 * @brief  logf,               求src中所有元素的loge，并将结果放在dst中
	 * @param  src                 输入图像
	 * @return dst                 输出结果,即 dst = log(src),自然对数
	 */
	inline cv::Mat logf(const cv::Mat& src) {
		CV_Assert(src.type() == CV_32FC1);
		cv::Mat dst;
		cv::log(src, dst);
		return dst;
	}

	/**
	 * @brief  log10f,             求src中所有元素的log10，并将结果放在dst中
	 * @param  src                 输入图像
	 * @return dst                 输出结果,即 dst = log10(src)
	 */
	inline cv::Mat log10f(const cv::Mat& src) {
		CV_Assert(src.type() == CV_32FC1);
		cv::Mat dst(src.size(), src.type());

		int cols = src.cols;
		int rows = src.rows;

		//返回bool值，判断存储是否连续。
		if (src.isContinuous() && dst.isContinuous())
		{
			cols *= rows;
			rows = 1;
		}
		//计算每个元素的log10
		for (int i = 0; i < rows; i++)
		{
			const float* srci = src.ptr<float>(i);
			float* dsti = dst.ptr<float>(i);
			for (int j = 0; j < cols; j++) {
				dsti[j] = std::log10(srci[j]);
			}
		}
		return dst;
	}

	/**
	 * @brief  log2f,              求src中所有元素的log2，并将结果放在dst中
	 * @param  src                 输入图像
	 * @return dst                 输出结果,即 dst = log2(src)
	 */
	inline cv::Mat log2f(const cv::Mat& src) {
		CV_Assert(src.type() == CV_32FC1);
		cv::Mat dst(src.size(), src.type());

		int cols = src.cols;
		int rows = src.rows;

		//返回bool值，判断存储是否连续。
		if (src.isContinuous() && dst.isContinuous())
		{
			cols *= rows;
			rows = 1;
		}
		//计算每个元素的log2
		for (int i = 0; i < rows; i++)
		{
			const float* srci = src.ptr<float>(i);
			float* dsti = dst.ptr<float>(i);
			for (int j = 0; j < cols; j++) {
				dsti[j] = std::log2(srci[j]);
			}
		}
		return dst;
	}

	/**
	 * @brief  sinf,               求src中所有元素的正弦，并将结果放在dst中
	 * @param  src                 输入图像
	 * @return dst                 输出结果,即 dst = sin(src)
	 */
	inline cv::Mat sinf(const cv::Mat& src) {
		CV_Assert(src.type() == CV_32FC1);
		cv::Mat dst(src.size(), src.type());

		int cols = src.cols;
		int rows = src.rows;

		//返回bool值，判断存储是否连续。
		if (src.isContinuous() && dst.isContinuous())
		{
			cols *= rows;
			rows = 1;
		}
		//计算每个元素的sin()
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
	 * @brief  cosf,               求src中所有元素的余弦，并将结果放在dst中
	 * @param  src                 输入图像
	 * @return dst                 输出结果,即 dst = cos(src)
	 */
	inline cv::Mat cosf(const cv::Mat& src) {
		CV_Assert(src.type() == CV_32FC1);
		cv::Mat dst(src.size(), src.type());

		int cols = src.cols;
		int rows = src.rows;

		//返回bool值，判断存储是否连续。
		if (src.isContinuous() && dst.isContinuous())
		{
			cols *= rows;
			rows = 1;
		}
		//计算每个元素的cos()
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
	 * @brief  expf_cx,            求exp(src)(两通道)，第一通道为实部，第二部分为虚部
	 * @param  src                 输入图像
	 * @return dst                 输出结果,即 dst = exp(src[0] + j * src[1])
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

	template <typename T>
	/**
	 * @brief  Unified_angle_cycle      统一角度周期，将角度集中在0-360°
	 * @param  angle                    输入角度
	 */
	inline void Unified_angle_cycle(T &angle)
	{
		while (angle < 0)
		{
			angle = angle + 360;
		}
		if (angle > 360)
		{
			angle -= (int(angle) / 360)*360.0f;
		}
	}

	/*--------------------- draw functions -------------------------*/
	// 画直方图
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
		std::string name_unit = "度";
		std::string name = "最大值为：";
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
	
	// 画圆
	inline void DrawCircle(cv::Mat mask,
		const cv::Point2i &center, int radius, 
		const cv::Scalar &color, int lineType)
	{
		cv::circle(mask, center, radius, color, lineType);
	}
	
	// 画矩形
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
	
	// 画旋转矩形
	inline void DrawRotatedRect(cv::Mat mask,
		const cv::RotatedRect &rotatedrect,
		const cv::Scalar &color, int lineType)
	{
		cv::Point2f ps[4];
		rotatedrect.points(ps);

		std::vector<std::vector<cv::Point>> tmpContours;    // 创建一个InputArrayOfArrays 类型的点集
		std::vector<cv::Point> contours;
		for (int i = 0; i != 4; ++i) {
			contours.emplace_back(cv::Point2i(ps[i]));
		}
		tmpContours.insert(tmpContours.end(), contours);
		drawContours(mask, tmpContours, 0, color, lineType);  // 填充mask
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
	
	// 画椭圆
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
	
	// 画矩形与圆
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
		drawContours(mask, tmpContours, 0, color, lineType);  // 填充mask

		// 计算常长短轴
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

			// 长轴两端以填充的方式画圆，直径等于短轴
			cv::circle(mask, c0, (int)r, color, lineType);
			cv::circle(mask, c1, (int)r, color, lineType);
		}
		else {
			// float r = a < b ? (b / 2.0f) : (a / 2.0f);
			cv::Point c0 = a > b ? cv::Point(point12_x, point12_y) : cv::Point(point23_x, point23_y);
			cv::Point c1 = a > b ? cv::Point(point03_x, point03_y) : cv::Point(point01_x, point01_y);

			// 短轴两端以填充的方式画圆，直径等于长轴
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
		DrawPill(mask, newrotatedrect, color, s, lineType);  //画矩形与圆
	}
	
	// 画圆角的矩形
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

		//画宽缩减后的矩形
		cv::RotatedRect r1 = newrotatedrect;
		r1.size.width = r1.size.width - 2 * radius;
		DrawRotatedRect(mask, r1, color, lineType);

		//画高缩减后的矩形
		cv::RotatedRect r2 = newrotatedrect;
		r2.size.height = r2.size.height - 2 * radius;
		DrawRotatedRect(mask, r2, color, lineType);

		//画四个角的圆
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

		//画宽缩减后的矩形
		cv::RotatedRect r1 = newrotatedrect;
		r1.size.width = r1.size.width - 2 * radius;
		DrawRotatedRect(mask, r1, color, lineType);

		//画高缩减后的矩形
		cv::RotatedRect r2 = newrotatedrect;
		r2.size.height = r2.size.height - 2 * radius;
		DrawRotatedRect(mask, r2, color, lineType);

		//画四个角的圆
		cv::RotatedRect r3 = r2;
		r3.size.width = r1.size.width;

		cv::Point2f ps[4];
		r3.points(ps);

		for (int i = 0; i != 4; ++i) {
			cv::circle(mask, ps[i], (int)radius, color, lineType);
		}
	}

	// 画多边形
	inline void DrawPolygon(cv::Mat& mask,
		const std::vector<cv::Point> &contours, 
		const cv::Scalar &color, int lineType) {
		if (contours.empty()) {
			return;
		}

		std::vector<std::vector<cv::Point>> tmpContours;    // 创建一个InputArrayOfArrays 类型的点集
		tmpContours.insert(tmpContours.end(), contours);
		drawContours(mask, tmpContours, 0, color, lineType);  // 填充mask
	}
	
	inline void DrawPolygon(cv::Mat& mask,
		const std::vector<std::vector<cv::Point>> &contours, const cv::Scalar &color, int lineType) {
		drawContours(mask, contours, 0, color, lineType);  // 填充mask
	}

	// 画Circle Rectangle
	inline void DrawCircleRect(cv::Mat& mask,
		const cv::RotatedRect &rotatedrect, const cv::Point2i& center,
		int radius, const cv::Scalar &color, int lineType) {
		cv::Mat maskCircle = mask.clone();
		cv::Mat maskRect = mask.clone();
		cv::circle(maskCircle, center, radius, color, lineType);
		DrawRotatedRect(maskRect, rotatedrect, color, lineType);
		mask = maskRect & maskCircle;
	}

	/*--------------------- file functions -------------------------*/
	/**
	 * @brief  ReadAsc              读取asc文件
	 * @param  file                 输入asc数据
	 * @param  images               原始干涉图
	 * @param  phase                解包裹数据
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
		float camerares = -1.;                            // 相机分辨率 m/pix
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

	inline void WritePicToExcel(string name, cv::Mat pic)
	{
		CV_Assert(pic.type() == CV_32FC1);
		ofstream outfile(name);
		int row = pic.rows;
		int col = pic.cols;
		for (int i = 0; i < row; i++)
		{
			float *p = pic.ptr<float>(i);
			for (int j = 0; j < col; j++)
			{
				if (p[j] == p[j])
				{
					outfile << p[j] << (j == (col - 1) ? '\n' : '\t');
				}
				else {
					outfile << "nan" << (j == (col - 1) ? '\n' : '\t');
				}

			}
		}
		outfile.close();
	}

	inline cv::Mat ReadPicFromExcel(string name)
	{
		cv::Mat result, pic;
		ifstream infile(name);
		string str;
		int col = 0;
		while (getline(infile, str))
		{
			string temp;
			stringstream input(str);
			col = 0;
			while (input >> temp)
			{
				if (temp == "nan")
				{
					pic.push_back(nan(""));
				}
				else {
					pic.push_back(float(atof(temp.c_str())));
				}
				col++;
			}
		}
		int number = result.rows;
		int row = number / col;
		result = pic.reshape(row, col);
		infile.close();
		return result;
	}


} // end calc

#endif
