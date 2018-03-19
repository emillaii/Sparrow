#include "stdafx.h"
#include "PositionLearningDlg.h"
#include "Utils.h"
// CPositionLearningDlg dialog

#define CONFIG_FILE "positionLearning.dat"

using namespace std;
using json = nlohmann::json;

CPositionLearningDlg::CPositionLearningDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_POSITION_LEARNING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPositionLearningDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//SUT Related
	DDX_Control(pDX, IDC_SUT_PICK_X, m_sut_pick_x_edit_text);
	DDX_Control(pDX, IDC_SUT_PICK_Y, m_sut_pick_y_edit_text);
	DDX_Control(pDX, IDC_SUT_PICK_Z, m_sut_pick_z_edit_text);

	DDX_Control(pDX, IDC_POS_DLG_SUT_X_POS, m_sut_x_curr_pos_text);
	DDX_Control(pDX, IDC_POS_DLG_SUT_Y_POS, m_sut_y_curr_pos_text);
	DDX_Control(pDX, IDC_POS_DLG_SUT_Z_POS, m_sut_z_curr_pos_text);

	DDX_Control(pDX, IDC_POS_DLG_SUT_X_STEP, m_sut_x_step_size);
	DDX_Control(pDX, IDC_POS_DLG_SUT_Y_STEP, m_sut_y_step_size);
	DDX_Control(pDX, IDC_POS_DLG_SUT_Z_STEP, m_sut_z_step_size);

}

BEGIN_MESSAGE_MAP(CPositionLearningDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CPositionLearningDlg::OnBnClickedLearnSUTX)
	ON_BN_CLICKED(IDC_BUTTON2, &CPositionLearningDlg::OnBnClickedSaveConfiguration)
	ON_BN_CLICKED(IDC_BUTTON3, &CPositionLearningDlg::OnBnClickedSUTXMoveF)
	ON_BN_CLICKED(IDC_BUTTON4, &CPositionLearningDlg::OnBnClickedSUTMoveB)
	ON_BN_CLICKED(IDC_BUTTON5, &CPositionLearningDlg::OnBnClickedSUTYMoveF)
	ON_BN_CLICKED(IDC_BUTTON12, &CPositionLearningDlg::OnBnClickedSUTYMoveB)
	ON_BN_CLICKED(IDC_BUTTON13, &CPositionLearningDlg::OnBnClickedSUTZMoveF)
	ON_BN_CLICKED(IDC_BUTTON14, &CPositionLearningDlg::OnBnClickedSUTZMoveB)
END_MESSAGE_MAP()


// CMFCApplication1Dlg message handlers

BOOL CPositionLearningDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if (machine == nullptr) {
		AfxMessageBox(_T("Error ! Missing in machine definition!"));
		return TRUE;
	}
	//Read the config file from Config file
	//Then parse the json into object
	//ToDo: Move to a function, this part will be getting bigger later...
	ifstream fin(CONFIG_FILE);
	string line;
	if (fin.is_open())
	{
		if (getline(fin, line)) {
			nlohmann::json o = nlohmann::json::parse(line.c_str());
			for (json::iterator it = o.begin(); it != o.end(); ++it) {
				if (strcmp(SUT_POSITION_PICK_X, it.key().c_str()) == 0) {
					double value = (double)it.value();
					this->machine->setSensorUTPickXPos(value);
				}
				else if (strcmp(SUT_POSITION_PICK_Y, it.key().c_str()) == 0) {
					double value = (double)it.value();
					this->machine->setSensorUTPickYPos(value);
				}
				else if (strcmp(SUT_POSITION_PICK_Z, it.key().c_str()) == 0) {
					double value = (double)it.value();
					this->machine->setSensorUTPickZPos(value);
				}
				DBOUT(it.key().c_str());
			}
		}
		fin.close();
	}
	else {
		AfxMessageBox(_T("Warning! Missing the config file"));
	}

	//Update the UI here
	CString temp;
	temp.Format(_T("%.3f"), this->machine->getSensorUTPos().sutPickX);
	this->m_sut_pick_x_edit_text.SetWindowTextW(temp);
	temp.Format(_T("%.3f"), this->machine->getSensorUTPos().sutPickY);
	this->m_sut_pick_y_edit_text.SetWindowTextW(temp);
	temp.Format(_T("%.3f"), this->machine->getSensorUTPos().sutPickZ);
	this->m_sut_pick_z_edit_text.SetWindowTextW(temp);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPositionLearningDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPositionLearningDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPositionLearningDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPositionLearningDlg::setMachineDefinition(MachineDefinition* machineDefinition)
{
	this->machine = machineDefinition;
}

int CPositionLearningDlg::getSUTX()
{
	return x;
}

void CPositionLearningDlg::updateSUTPosition()
{
	CString temp;
	SensorUT sut;
	m_sut_pick_x_edit_text.GetWindowTextW(temp);
	double value = _wtof(temp);
	sut.sutPickX = value;
	m_sut_pick_y_edit_text.GetWindowTextW(temp);
	value = _wtof(temp);
	sut.sutPickY = value;
	m_sut_pick_z_edit_text.GetWindowTextW(temp);
	value = _wtof(temp);
	sut.sutPickZ = value;
	machine->setSensorUTPos(sut);
}

void CPositionLearningDlg::OnBnClickedLearnSUTX()
{
	// TODO: Add your control notification handler code here
	x++;
	CString temp;
	SensorUT sut;
	m_sut_pick_x_edit_text.GetWindowTextW(temp);
	double value = _wtof(temp);
	sut.sutPickX = value; 
	m_sut_pick_y_edit_text.GetWindowTextW(temp);
    value = _wtof(temp);
	sut.sutPickY = value;
	m_sut_pick_z_edit_text.GetWindowTextW(temp);
    value = _wtof(temp);
	sut.sutPickZ = value;
	machine->setSensorUTPos(sut);
}


void CPositionLearningDlg::OnBnClickedSaveConfiguration()
{
	updateSUTPosition(); 
	json j; 
	j.emplace(SUT_POSITION_PICK_X, this->machine->getSensorUTPos().sutPickX);
	j.emplace(SUT_POSITION_PICK_Y, this->machine->getSensorUTPos().sutPickY);
	j.emplace(SUT_POSITION_PICK_Z, this->machine->getSensorUTPos().sutPickZ);
	ofstream fout;
	fout.open(CONFIG_FILE, std::ios::out);
	CString dumpString(j.dump().c_str());
	fout << dumpString + "";
	fout.close();
}

//Generic Call
bool CPositionLearningDlg::incrementMotorInStep(CStatic &m, double step, bool direction)
{
	if (step > 10 || step < -10) {
		AfxMessageBox(_T("Wrong Input Step Size!"));
		return false;
	}
	CString temp;
	m.GetWindowTextW(temp);
	double value = _wtof(temp);
	if (direction) value += step; 
	else value -= step;
	temp.Format(_T("%.6f"), value);
	m.SetWindowTextW(temp);
	return true;
}

//SUT X Move Forward
void CPositionLearningDlg::OnBnClickedSUTXMoveF()
{
	CString temp; m_sut_x_step_size.GetWindowTextW(temp);
	double step = _wtof(temp);
	incrementMotorInStep(m_sut_x_curr_pos_text, step, true);
}

//SUT X Move Backward
void CPositionLearningDlg::OnBnClickedSUTMoveB()
{
	CString temp; m_sut_x_step_size.GetWindowTextW(temp);
	double step = _wtof(temp);
	incrementMotorInStep(m_sut_x_curr_pos_text, step, false);
}


void CPositionLearningDlg::OnBnClickedSUTYMoveF()
{
	CString temp; m_sut_y_step_size.GetWindowTextW(temp);
	double step = _wtof(temp);
	incrementMotorInStep(m_sut_y_curr_pos_text, step, true);
}


void CPositionLearningDlg::OnBnClickedSUTYMoveB()
{
	CString temp; m_sut_y_step_size.GetWindowTextW(temp);
	double step = _wtof(temp);
	incrementMotorInStep(m_sut_y_curr_pos_text, step, false);
}


void CPositionLearningDlg::OnBnClickedSUTZMoveF()
{
	CString temp; m_sut_z_step_size.GetWindowTextW(temp);
	double step = _wtof(temp);
	incrementMotorInStep(m_sut_z_curr_pos_text, step, true);
}


void CPositionLearningDlg::OnBnClickedSUTZMoveB()
{
	CString temp; m_sut_z_step_size.GetWindowTextW(temp);
	double step = _wtof(temp);
	incrementMotorInStep(m_sut_z_curr_pos_text, step, false);
}
