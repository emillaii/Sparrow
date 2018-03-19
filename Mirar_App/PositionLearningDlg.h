#pragma once

#include "resource.h"		// main symbols
#include <iostream>
#include <fstream>
#include "MachineDefinition.h"
#include "json.hpp"
class CPositionLearningDlg : public CDialog
{
	// Construction
public:
	CPositionLearningDlg(CWnd* pParent = NULL);	// standard constructor

												// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private: 
	int x = 0; 
	MachineDefinition *machine; 
	//UI
	//SUT Related
	CStatic m_sut_pick_x_edit_text;
	CStatic m_sut_pick_y_edit_text;
	CStatic m_sut_pick_z_edit_text;
	CStatic m_sut_x_curr_pos_text;
	CStatic m_sut_y_curr_pos_text;
	CStatic m_sut_z_curr_pos_text;
	CStatic m_sut_x_step_size;
	CStatic m_sut_y_step_size;
	CStatic m_sut_z_step_size;

public: 
	void setMachineDefinition(MachineDefinition* machine);
	int getSUTX(); 
	void updateSUTPosition();
public:
	afx_msg void OnBnClickedLearnSUTX();
	afx_msg void OnBnClickedSaveConfiguration();
	afx_msg void OnBnClickedSUTXMoveF();
	afx_msg void OnBnClickedSUTMoveB();
	afx_msg void OnBnClickedSUTYMoveF();
	afx_msg void OnBnClickedSUTYMoveB();
	afx_msg void OnBnClickedSUTZMoveF();
	afx_msg void OnBnClickedSUTZMoveB();

private: 
	bool incrementMotorInStep(CStatic &m, double step, bool direction);
};
