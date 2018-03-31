//Sparrow_AppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Sparrow_App.h"
#include "Sparrow_AppDlg.h"
#include "afxdialogex.h"
#include <Windows.h>
#include <fstream>
#include "GraphUtils.h"

using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSparrowAppDlg dialog



CSparrowAppDlg::CSparrowAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_Sparrow_APP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSparrowAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VIDEO1, m_wndVideoA);
	DDX_Control(pDX, IDC_GRAPH, m_wndGraph);
	//DDX_Control(pDX, IDC_VIDEO2, m_wndVideoB);
	DDX_Control(pDX, IDC_Z, m_zText);
	DDX_Control(pDX, IDC_MFCBUTTON2, m_testButton);
	DDX_Control(pDX, IDC_LIST1, m_logList);
}

BEGIN_MESSAGE_MAP(CSparrowAppDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPENCAMERA, &CSparrowAppDlg::OnBnClickedOpencamera)
	ON_BN_CLICKED(IDC_CLOSE_CAMERA, &CSparrowAppDlg::OnBnClickedCloseCamera)
	ON_BN_CLICKED(IDC_BUTTON4, &CSparrowAppDlg::OnBnClickedSaveImage)
	ON_BN_CLICKED(IDC_BUTTON5, &CSparrowAppDlg::OnBnClickedTakeAAZPoint)
	ON_BN_CLICKED(IDC_BUTTON7, &CSparrowAppDlg::OnBnClickedClearAAZPoint)
	ON_BN_CLICKED(IDC_BUTTON8, &CSparrowAppDlg::OnBnClickedMTFPlot)
	ON_BN_CLICKED(IDC_BUTTON9, &CSparrowAppDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CSparrowAppDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CSparrowAppDlg::OnBnClickedMTFLive)
	ON_BN_CLICKED(IDC_MFCBUTTON2, &CSparrowAppDlg::OnBnClickedOpenCloseCamera)
	ON_BN_CLICKED(IDC_BUTTON6, &CSparrowAppDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON2, &CSparrowAppDlg::OnBnClickedMotionDlg)
	ON_BN_CLICKED(IDC_COMMAND2, &CSparrowAppDlg::OnBnClickedCommand2)
END_MESSAGE_MAP()


// CSparrowAppDlg message handlers

BOOL CSparrowAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.
	SetWindowPos(&this->wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
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

	//Directory Setup
	CreateDirectory(_T("Sparrow_Log"), NULL);
	CreateDirectory(_T("Sparrow_Graph"), NULL);

	pLog = Log::GetInstance(); 
	pLog->Write("[OnInitDialog] InitDialog started");

	//Dothinkey Related
	if (dk == nullptr) {
		dk = new Dothinkey();
	}
	//Set Timer 
	//SetTimer(0, 10, 0);
	liveViewThreadFlag = false;
	
	//Init PositionLearningDlg, the one and only one 
	positionLearningDlg = CPositionLearningDlg::GetInstance();
	positionLearningDlg->setMachineDefinition(&machineDefinition);
	positionLearningDlg->loadConfigFile();

	//Init MotionDlg . the one and only one motionDlg
	motionDlg = MotionDlg::GetInstance();

	//UI Related
	m_logList.InsertColumn(0, _T("Log"), LVCFMT_LEFT, 1800);

	SFont.CreateFont(70, 28, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, _T("Times New Roman")); //Create Font
	
	m_testButton.EnableFullTextTooltip(true);
	m_testButton.EnableMenuFont();
	m_testButton.EnableWindowsTheming(true);
	m_testButton.SetAutorepeatMode();
	//m_testButton.SetFaceColor(RGB(255, 0, 0), true);
	m_testButton.SetTextColor(RGB(0, 0, 255));
	m_testButton.SetFont(&SFont, true);
	UpdateData(true);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSparrowAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSparrowAppDlg::OnPaint()
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
HCURSOR CSparrowAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSparrowAppDlg::OnTimer(UINT_PTR nIDEvent)
{
	AfxMessageBox(_T("Hello"));
	CDialog::OnTimer(nIDEvent);
}

BOOL CSparrowAppDlg::DestroyWindow()
{
	pLog->Write("[DestroyWindow] DestroyWindow is called!");
	if (dk != nullptr)
	{
		delete dk;
	}
	return CDialog::DestroyWindow();
}

void CSparrowAppDlg::logToListBox(CString str)
{
	CString t = CTime::GetCurrentTime().Format("%y-%m:%d %H:%M:%S : ");
	m_logList.InsertItem(0, t + str);
}

void CSparrowAppDlg::OnBnClickedOpencamera()
{
	// TODO: Add your control notification handler code here
	dk->DothinkeyEnum();
	dk->DothinkeyOpen();
	dk->DothinkeyLoadIniFile(0);
	dk->DothinkeyStartCamera(0, &m_wndVideoA);
	//dk->DothinkeyGrabImage(0);
	dk->DothinkeyStartCameraCapture(0);
	//for (int i = 0; i < 2; i++) {
	//	dk->DothinkeyLoadIniFile(i);
	//	i == 0 ? dk->DothinkeyStartCamera(i, &m_wndVideoA) : dk->DothinkeyStartCamera(i, &m_wndVideoB);
	//	dk->DothinkeyStartCameraCapture(i);
		//dk->DothinkeyGrabImage(i);
	//}
}


void CSparrowAppDlg::OnBnClickedCloseCamera()
{
	dk->DothinkeyStopCameraCapture(0);
	dk->DothinkeyStopCameraCapture(1);
	// TODO: Add your control notification handler code here
}

void CSparrowAppDlg::OnBnClickedSaveImage()
{
	// TODO: Add your control notification handler code here
	bool ret = dk->DothinkeySaveImage(0);
	if (!ret) AfxMessageBox(_T("Camera not open!"));
}

void test_sfr(int idx, std::promise<vector<Sfr_entry>> &&p, double currZ) {
	vector<Sfr_entry> v = Sfr::calculateSfr(idx, currZ, "output\\1.bmp");
	p.set_value(v);
}

vector<Sfr_entry> CSparrowAppDlg::calculateSfrAtCurrZ(bool isDisplayResult, double currZ) {
	//Update Image Here

	//Sfr Calculation 
	std::promise<vector<Sfr_entry>> p;
	auto f = p.get_future();
	std::thread mThread(test_sfr, 0, std::move(p), currZ);  //Use output/1.bmp as a input 
	mThread.join();
	vector<Sfr_entry> sfr_list = f.get();

	//Show Result on the current image 

	if (isDisplayResult) {
		IplImage *img = cvLoadImage("output/1.bmp");
		int Radius = 2;
		CvScalar Color = CV_RGB(255, 0, 0);
		CvFont font;
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 1, 3);
		for (int j = 0; j < sfr_list.size(); j ++) {
				int dx = 0, dy = 0;
				String sz_temp = std::to_string(sfr_list[j].sfr_025);
				sz_temp = sz_temp.substr(0, 5);
				cvCircle(img, cvPoint((int)(sfr_list[j].px), (int)(sfr_list[j].py)), Radius, CV_RGB(0, 255, 0), 3, CV_AA);
				cvPutText(img, sz_temp.c_str(), cvPoint((int)(sfr_list[j].px+dx), (int)(sfr_list[j].py+dy)), &font, CV_RGB(255, 0, 200));
		}

		cvShowImage("sfr", img);
		cvMoveWindow("sfr", 0, 0);
		cvReleaseImage(&img);
	}

	return sfr_list;
}

void test_opencv(void* params) {
	CSparrowAppDlg *dlg = (CSparrowAppDlg *) params;
	dlg->calculateSfrAtCurrZ(true, 0);
	cvWaitKey(10000);
	cvDestroyWindow("sfr");
}

//This is Z Scan
void CSparrowAppDlg::OnBnClickedTakeAAZPoint()
{
	bool ret = dk->DothinkeySaveImage(0);  // Take photo first
	CString pz;
	m_zText.GetWindowTextW(pz);
	double z = _wtof(pz);
	sfr_sampling.push_back(calculateSfrAtCurrZ(true, z));
}


void CSparrowAppDlg::OnBnClickedClearAAZPoint()
{
	sfr_sampling.clear();
}


void CSparrowAppDlg::OnBnClickedMTFPlot()
{
	const int sampleSize = (int)this->sfr_sampling.size();
	if (sampleSize <= 0) return;
	double minZ = 99999; 
	double maxZ = -99999;
	double x[255] = { 0 }, y[255] = { 0 };  // order 3 curve fitting
	for (int i = 0; i < sampleSize; i++) {
		vector<Sfr_entry> v = sfr_sampling.at(i);
		x[i] = v.at(0).sfr_025;
		y[i] = v.at(0).pz;
		if (v.at(0).pz > maxZ) maxZ = v.at(0).pz;
		if (v.at(0).pz < minZ) minZ = v.at(0).pz;
	}
	double c[4] = { 0.0 };
	double r2 = 0;
	curveFitting(x, y, sampleSize, c, r2);
	int yy = 0;
	Mat data_x(1, 1000, CV_64F);
	Mat data_y(1, 1000, CV_64F);
    
	for (int i = 0; i < data_x.cols; i++)
	{
		double x = (double)i / 10000;
		double y = 0;
		for (int j = 0; j < 4; j++) {
			y += c[j] * pow(x, j);
		}
		data_x.at<double>(0, i) = x;
		data_y.at<double>(0, i) = y;
	}
}

//Save the sfr sampling points into a .dat file
void CSparrowAppDlg::OnBnClickedButton9()
{
	saveSFR(sfr_sampling);
	AfxMessageBox(_T("Save SFR Success."));
}

//Load the Sfr sampling points from the .dat file
void CSparrowAppDlg::OnBnClickedButton10()
{
	this->sfr_sampling = loadSFR();
	long long start = milliseconds_now();
	auto e = sfr_curve_analysis(sfr_sampling, true);
	long long elapsed = milliseconds_now() - start;
	CString logString;
	logString.Format(_T("Sfr Curve Time elapsed: %lld ms"), elapsed);
	pLog->WriteString(logString);
	CImage image;
	image.Load(_T("graphPlot.png"));
	if (!image.IsNull())
	{
		HBITMAP hBmp = image.Detach();
		m_wndGraph.SetBitmap(hBmp);
	}
}

// This button is used to display the MTF in Live View 
void CSparrowAppDlg::OnBnClickedMTFLive()
{
	if (!mtfLiveViewThread) {
		mtfLiveViewThread = true;
		std::thread t(MTF_Live_Thread, this);
		t.detach();
	}
	else {
		mtfLiveViewThread = false;
	}
}


void CSparrowAppDlg::OnBnClickedOpenCloseCamera()
{
	long long start = milliseconds_now();
	std::promise<vector<Sfr_entry>> p;
	std::promise<vector<Sfr_entry>> p1;
	auto f = p.get_future();
	auto f1 = p1.get_future();
	std::thread mThread(test_sfr, 0, std::move(p), 0);
	std::thread mThread1(test_sfr, 1, std::move(p1), 0);
	mThread.join();
	mThread1.join();
	vector<Sfr_entry> sfr_list = f.get();
	vector<Sfr_entry> sfr_list1 = f1.get();
	long long elapsed = milliseconds_now() - start;
	DBOUT("MTF Time elapsed: " << elapsed << "\n");
	IplImage *img = cvLoadImage("output/1.bmp");
	int Radius = 2;
	CvScalar Color = CV_RGB(255, 0, 0);
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 1, 3);
	for (int j = 0; j < sfr_list.size(); j += 4) {
		int estimated_rect_x = (int)abs(sfr_list[j + 1].px - sfr_list[j + 3].px);
		int estimated_rect_y = (int)abs(sfr_list[j].py - sfr_list[j + 2].py);
		for (int k = 0; k < 4; k++)
		{
			int dx = 0, dy = 0;
			String sz_temp = std::to_string(sfr_list[j + k].sfr_025);
			sz_temp = sz_temp.substr(0, 5);
			cvCircle(img, cvPoint((int)(sfr_list[j + k].px), (int)(sfr_list[j + k].py)), Radius, CV_RGB(0, 255, 0), 3, CV_AA);
			if ((j + k) % 4 == 2)
			{
				dx = 0;
				dy = estimated_rect_y / 4;
			}
			if ((j + k) % 4 == 3)
			{
				dx = -estimated_rect_x / 2;
				dy = 0;
			}
			cvPutText(img, sz_temp.c_str(), cvPoint((int)(sfr_list[j + k].px + dx), (int)(sfr_list[j + k].py + dy)), &font, CV_RGB(255, 0, 200));
		}
	}

	cvShowImage("sfr", img);
	cvMoveWindow("sfr", 0, 0);
	cvReleaseImage(&img);

	//Curve Fitting Test 

	//double x[9] = { -0.13, -0.14, -0.15, -0.16, -0.17, -0.18, -0.19, -0.2, -0.21 };
	//double y[9] = { 8.989, 13.28, 21.14, 36.46, 46.32, 43.37, 31.45, 13.21, 10.63 };
	//double x[6] = { 0.16, 0.14, 0.12, 0.1, 0.08, 0.06 };   // Need to normal the data, otherwise, overflow and cause wrong calculation
	//double y[6] = { 3.8095, 7.57075, 18.15475, 33.365, 23.843, 9.7285};
	double x[9] = { 0.22,
		0.2,
		0.18,
		0.16,
		0.14,
		0.12,
		0.1,
		0.08,
		0.06
	};
	double y[9] = { 14.235,
		16.436,
		24.267,
		46.189,
		59.243,
		26.01,
		6.608,
		11.094,
		7.006

	};
	double c[7] = { 0 };
	double r2 = 0;
	curveFitting(x, y, 9, c, r2);   // order is 4
									//showCurve(4, c, x, y, 6, "Curve");

	showFloatGraph("Curve Fitting", (float*)y, 9);

	//Plane Fitting Test
	vector<threeDPoint> points;
	//points.push_back(threeDPoint(0, 0, 0));
	//points.push_back(threeDPoint(1, 1, 1));
	//points.push_back(threeDPoint(-1, 1, 1));
	points.push_back(threeDPoint(3085, 360, 20.147));

	points.push_back(threeDPoint(1130, 490.228, 20.2103));

	points.push_back(threeDPoint(1087, 2071, 20.0779));

	points.push_back(threeDPoint(3275, 2234, 20.006));
	planeFitting(points);
}

void CSparrowAppDlg::OnBnClickedButton6()
{
	logToListBox(_T("Testing"));
	nlohmann::json j2 = {
		{ "pi", 3.141 },
		{ "happy", true },
		{ "name", "Niels" },
		{ "nothing", nullptr },
		{ "answer",{
			{ "everything", 42 }
		} },
		{ "list",{ 1, 0, 2 } },
		{ "object",{
			{ "currency", "USD" },
		{ "value", 42.99 }
		} }
	};
	std::string s = j2.dump();
	CString str(s.c_str());
	logToListBox(str);
	nlohmann::json j3 = nlohmann::json::parse(s);
	CString strr(j3.dump().c_str());
	logToListBox(strr);

	//Testing machine data structure
	MachineDefinition md;
	CPositionLearningDlg *dlg = new CPositionLearningDlg();
	dlg->setMachineDefinition(&md);
	dlg->DoModal(); 

}


void CSparrowAppDlg::OnBnClickedMotionDlg()
{
	if (motionDlg->isMotionDriverInit()) {
		AfxMessageBox(_T("Motion Driver Inited"));
	}
	motionDlg->DoModal();
}


//Description: Z Scan 

void CSparrowAppDlg::OnBnClickedCommand2()
{
	int step_count = 8; 
	int step_size = 10;
	this->positionLearningDlg->loadConfigFile();
	double zScanStartPos = machineDefinition.getAAHeadPos().aaZScanZ;
	if (!motionDlg->isMotionDriverInit())
	{
		pLog->WriteString(_T("[ZScan] Motion is not ready"));
	}
	for (int i = 0; i < step_count; i++) {
		motionDlg->move_AA_Z(step_count, zScanStartPos + i*step_size); //Move Z Motor
		bool ret = dk->DothinkeySaveImage(0);  // Take photo first
		if (ret)
		{
			CString pz;
			m_zText.GetWindowTextW(pz);
			double z = _wtof(pz);
			sfr_sampling.push_back(calculateSfrAtCurrZ(true, z));
		}
	}
	//plane fitting ....
	auto e = sfr_curve_analysis(sfr_sampling, true);
}
