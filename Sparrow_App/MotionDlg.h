#pragma once
#include "afxwin.h"
#include "resource.h"
#include "CXTMotionControler_Lib.h"
#include "XT_MotionControler_Client_Lib.h"
#include "XT_MotionControlerExtend_Client_Lib.h"
#include "XT_Visual_Orientation_Lib.h"

// MotionDlg dialog

class MotionDlg : public CDialog
{
	DECLARE_DYNAMIC(MotionDlg)

public:
	MotionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MotionDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MOTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	static MotionDlg* GetInstance();                    // Singleton MotionDlg
	HICON m_hIcon;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void UpdateParam_X();
	void UpdateParam_Y();
	void UpdateParam_Z();
	void UpdateParam_A();
	void UpdateParam_B();
	void UpdateParam_C();

	void Axis_SeekOrigin_PN(
		int iThreadID,
		unsigned int iAxis,
		unsigned int iIo_In_Origin,
		int bIo_In_Origin_Dir,
		double fPos,
		double fNeg,
		double fOffset,
		double fNew_Pos,
		double fSpeed_2Seek
	);


	void Example_Dispensing();
	void Example_Moving_Dispense();
	void Example_Flying_Dispense();

public:

	CStatic m_cCurPos_X;
	CStatic m_cCurPos_Y;
	CStatic m_cCurPos_Z;
	CStatic m_cCurVel_X;
	CStatic m_cCurVel_Y;
	CStatic m_cCurVel_Z;
	CStatic m_cCurAcc_X;
	CStatic m_cCurAcc_Y;
	CStatic m_cCurAcc_Z;
	CEdit m_cMaxVel_X;
	CEdit m_cMaxVel_Y;
	CEdit m_cMaxVel_Z;
	CEdit m_cMaxAcc_X;
	CEdit m_cMaxAcc_Y;
	CEdit m_cMaxAcc_Z;
	CEdit m_cMaxJerk_X;
	CEdit m_cMaxJerk_Y;
	CEdit m_cMaxJerk_Z;
	CButton m_cMove_X;
	CButton m_cMove_Y;
	CButton m_cMove_Z;
	CButton m_cOrigin_X;
	CButton m_cOrigin_Y;
	CButton m_cOrigin_Z;
	CButton m_cOutput;
	CEdit m_cPulseRatio_X;
	CEdit m_cPulseRatio_Y;
	CEdit m_cPulseRatio_Z;
	CEdit m_cStep;
	double m_fStep;

	typedef enum {
		Axis_X,
		Axis_Y,
		Axis_Z,
		Axis_A,
		Axis_B,
		Axis_C
	} MOTION_AXIS;
	typedef enum {
		Origin_X,
		Origin_Y,
		Origin_Z,
		Origin_A,
		Origin_B,
		Origin_C
	} MOTION_ORIGIN;
	typedef enum {
		Output_0,
		EN_X,
		EN_Y,
		EN_Z,
		En_A,
		En_B,
		En_C
	}MOTION_EN;
	typedef enum {
		Thread_Interpolation,
		Thread_X,
		Thread_Y,
		Thread_Z,
		Thread_A,
		Thread_B,
		Thread_C,
		Thread_Buffer_Sync
	}MOTION_THREAD;

#define Axis_X_Range (30)
#define Axis_Y_Range (30)
#define Axis_Z_Range (68)
#define Axis_A_Range (5)
#define Axis_B_Range (4)
#define Axis_C_Range (20)

	afx_msg void OnBnClickedButtonInit();
	afx_msg void OnBnClickedButtonZxyOrigin();
	afx_msg void OnBnClickedButtonXOrigin();
	afx_msg void OnBnClickedButtonYOrigin();
	afx_msg void OnBnClickedButtonZOrigin();
	afx_msg void OnBnClickedButtonAOrigin();
	afx_msg void OnBnClickedButtonBOrigin();
	afx_msg void OnBnClickedButtonCOrigin();
	afx_msg void OnBnClickedButtonInterpolation();
	afx_msg void OnBnClickedButtonDispensing();
	afx_msg void OnBnClickedButtonFlyingdispensing();
	afx_msg void OnBnClickedButtonMovingdispensing();
	afx_msg void OnBnClickedCheckOutput();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonUpdate();
	CStatic m_cCurPos_A;
	CStatic m_cCurPos_B;
	CStatic m_cCurPos_C;
	CStatic m_cCurVel_A;
	CStatic m_cCurVel_B;
	CStatic m_cCurVel_C;
	CStatic m_cCurAcc_A;
	CStatic m_cCurAcc_B;
	CStatic m_cCurAcc_C;
	CEdit m_cMaxVel_A;
	CEdit m_cMaxVel_B;
	CEdit m_cMaxVel_C;
	CEdit m_cMaxAcc_A;
	CEdit m_cMaxVel_;
	CEdit m_cMaxAcc_B;
	CEdit m_cMaxAcc_C;
	CEdit m_cMaxJerk_A;
	CEdit m_cMaxJerk_B;
	CEdit m_cMaxJerk_C;
	CEdit m_cPulseRatio_A;
	CEdit m_cPulseRatio_B;
	CEdit m_cPulseRatio_C;
	CButton m_cMove_A;
	CButton m_cMove_B;
	CButton m_cMove_C;
	CButton m_cOrigin_A;
	CButton m_cOrigin_B;
	CButton m_cOrigin_C;
	double n;
	afx_msg void OnTimer(UINT_PTR nIDEvent);

//Motion Service 
private: 
	bool isInit = false;
public:
	bool isMotionDriverInit(); 
	bool move_AA_X();
	bool move_AA_Y();
	bool move_AA_Z(double stepSize, double targerPos);

	double get_Z_Pos();
	double get_X_Pos();
	double get_Y_Pos();
	double get_A_Pos();
	double get_B_Pos();
	double get_C_Pos();
	afx_msg void OnBnClickedButtonZRunning();
};
