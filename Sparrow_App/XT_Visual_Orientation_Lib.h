#pragma once
#include "CXTComputerVisionInterface_Data.h"

typedef int(__cdecl*pXT_Visual_Orientation_Callback)(int iIndex, double fDelta_X, double fDelta_Y, double fDelta_C, int bWait_Finish, int bResume);

#ifdef _DLLAPI
#undef _DLLAPI
#endif
#ifdef _DLL_Provider
#define _DLLAPI  _declspec(dllexport)  
#else  
#define _DLLAPI  _declspec(dllimport)  
#endif

extern "C"
{
	// 初始化 .
	int _DLLAPI XT_Visual_Orientation_Init(int iIndex, LPWSTR pServer_IP, LPWSTR pFile_Path, pXT_Visual_Orientation_Callback pCallBack, LPWSTR pFingerPrint, int bSilence);

	// 反初始化 , 否则会漏内存 .
	int _DLLAPI XT_Visual_Orientation_DeInit(int iIndex);

	LPWSTR _DLLAPI XT_Visual_Orientation_Get_Last_Error(int iIndex);

	// 只获取图像 .
	int _DLLAPI XT_Visual_Orientation_Get_Image(int iIndex, tImage** pImage);

	// 获取定位位置 .
	int _DLLAPI XT_Visual_Orientation_Get_Pos(int iIndex, double& fPhy_X, double& fPhy_Y, double& fPhy_C, double& fPix_X, double& fPix_Y, tImage** pImage, int bUse_Proper_Template);

	// 跟踪目标 .
	int _DLLAPI XT_Visual_Orientation_Track(int iIndex, int nMax_Count, double fDistance, int bUse_Proper_Template);
	int _DLLAPI XT_Visual_Orientation_Wait_Stable(int iIndex);

	// 调用窗口 .
	int _DLLAPI XT_Visual_Orientation_DoDlg(int iIndex, LPWSTR strDlg_Caption_Name, LPWSTR strLanguageFile);
	int _DLLAPI XT_Visual_Orientation_CloseDLg(int iIndex, int bOK);

	// 获取模板数量 .
	int _DLLAPI XT_Visual_Orientation_Get_Template_Amount(int iIndex, int& nAmount);

	// 获取和编辑模板名称 .
	LPWSTR _DLLAPI XT_Visual_Orientation_Get_Template(int iIndex, int iTemplate, tImage** pImage);
	int _DLLAPI XT_Visual_Orientation_Set_Template_Name(int iIndex, int iTemplate, LPWSTR pstrName);


	// 设置模板 .
	int _DLLAPI XT_Visual_Orientation_Set_Template(int iIndex, LPWSTR pstrName);
	int _DLLAPI XT_Visual_Orientation_Add_Template(int iIndex, const LPWSTR pstrName, const tImage* pImage);

	// 设置曝光时间 .
	int _DLLAPI XT_Visual_Orientation_Set_Camera_Manual_Exposure_Time_Abs(int iIndex, double fExposureTimeAbs);
	int _DLLAPI XT_Visual_Orientation_Get_Camera_Manual_Exposure_Time_Abs(int iIndex, double& fExposureTimeAbs);

	// 设置 AOI .
	int _DLLAPI XT_Visual_Orientation_Set_Camera_AOI(int iIndex, long long nWidth, long long nHeight, long long OffsetX, long long OffsetY);
	int _DLLAPI XT_Visual_Orientation_Get_Camera_AOI(int iIndex, long long& nWidth, long long& nHeight, long long& OffsetX, long long& OffsetY, long long& nMaxWidth, long long& nMaxHeight);

	//////////////////////////////////////////////////////////////////////////
	// 数学工具 .

	// 在点集中找出距离最大的点 , 用于跟踪 .
	// nOutput_Point  2 输出距离最大的两点 , 3 , 4 输出面积最大的 n 点 .
	// 如果 nInput_Point <= nOutput_Point 会输出全部索引 .
	int _DLLAPI XT_Visual_Orientation_Get_Max_Distance_Point(int iIndex, int nDem, int nInput_Point, double* pfPoints, int nOutput_Point, int* pnPoint_Index);

	// 根据 pSrc_Points 到 pDst_Points 的映射 , 计算 pCal_Points 的映射结果 .
	// nMode 模式 , 暂时只能是 0 .
	// nDem 维度 , 暂时只能是 2 .
	// nConv_Point 坐标转换点数量 , 对应 pSrc_Points 和 pDst_Points 的长度 .
	// pSrc_Points 原始点 .
	// pDst_Points 目标点 .
	// pCal_Points 需要计算的映射点 , 长度 nCal_Point , 结果保存在同一位置 .
	int _DLLAPI XT_Visual_Orientation_Conv_Coord(int iIndex, int nMode, int nDem, int nConv_Point, const double* pfSrc_Points, const double* pfDst_Points, int nCal_Point, double* pfCal_Points);

	// TSP .
	int _DLLAPI XT_Visual_Orientation_Get_TSP_Result(int iIndex, int nDem, int nPoint, const double* pfPoints, const double* pfStart_Point, int* piResult_Index);
};