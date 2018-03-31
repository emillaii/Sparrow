#pragma once

// 图像 .
typedef struct{
	int nX;
	int nY;
	int nDem;
	BYTE* pGray;
}tImage;

// 模板匹配参数 .
typedef struct tMatch_Param
{
	tMatch_Param()
	{
		nMaxPyramidLevel = -1;
		fRoiFactor = 0.65;
		fStartAngle = 0;
		fEndAngle = 360;
		nRetTemplate = 1;
		fMatchValueThreshold = 0.87;
	}
	int nMaxPyramidLevel; //
	double fRoiFactor; // 匹配区域 0 -- 1
	double fStartAngle; // 度
	double fEndAngle; // 度
	int nRetTemplate; // nRetTemplate
	double fMatchValueThreshold; // 匹配阀值
}tMatch_Param;

// 匹配结果 .
typedef struct
{
	double fMatch_Value;
	double fAngle;
	double fPos_X;
	double fPos_Y;
	double fSize_X;
	double fSize_Y;
}tMatch_Result;

typedef struct{
	int nMatch_Num;
	tMatch_Result* pMatch_Result;
}tMatch_All_Result;

// 灰度圆匹配参数 .
typedef struct{
	double fGray;// 灰度 , ( 0 , 1 ] .
	double fCircularity;// 圆度 , ( 0 , 1 ] .
	double fRound_Dem;// 圆直径 , > 2 .
	double fRound_Dem_Error;// 圆直径容差 , > 0 .
	int bInvert;// 反色 , 0 / 1 .
}tGray_Param;
