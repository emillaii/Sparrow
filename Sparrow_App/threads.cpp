#include "stdafx.h"
#include "threads.h"
#include "Utils.h"
#include "Log.h"
#include <chrono>

using namespace std::chrono;

void NewAA_Thread(void *params)
{
	Log::GetInstance()->Write("[NewAA_Thread] Calculation Thread");
	CSparrowAppDlg *dlg = (CSparrowAppDlg *)params;
	_endthread();
}

void SFR_Thread(void *params)
{
	Log::GetInstance()->Write("[SFR_Thread] SFR Calculation Thread");
	CSparrowAppDlg *dlg = (CSparrowAppDlg *)params;
	_endthread();
}

void MTF_Live_Thread(void *params)
{
	Log::GetInstance()->Write("[MTF_Live_Thread] is called");
	CSparrowAppDlg *dlg = (CSparrowAppDlg *)params;
	while (dlg->mtfLiveViewThread) {
		Log::GetInstance()->Write("MTF Live is capturing....");
		dlg->dk->DothinkeySaveImage(0);   //Save the image from Dothinkey
		dlg->calculateSfrAtCurrZ(true, 0);
		cvWaitKey(100);
	}
	cvDestroyWindow("sfr");
	Log::GetInstance()->Write("[MTF_Live_Thread] is finished"); 
	_endthread();
}

void Camera_Thread(void *params, int iDevID)
{
	Dothinkey *dk = (Dothinkey *)params;
	SensorTab pSensor;
	int grabSize;
	HANDLE handle = nullptr;
	int *cameraWorkStatus = nullptr;
	int *grabWorkStatus = nullptr;
	grabSize = dk->m_GrabSizeA;

	if (iDevID == 0) 
	{
		pSensor = dk->GetSensorA();
		grabSize = dk->m_GrabSizeA;
		handle = dk->m_hCameraAThreadEvent;
		cameraWorkStatus = &dk->m_CameraAWorkStatus;
		grabWorkStatus = &dk->m_GrabImageWorkStatusA;
	}
	else if (iDevID == 1) 
	{
		pSensor = dk->GetSensorB();
		grabSize = dk->m_GrabSizeB;
		handle = dk->m_hCameraBThreadEvent;
		cameraWorkStatus = &dk->m_CameraBWorkStatus;
		grabWorkStatus = &dk->m_GrabImageWorkStatusB;
	}

	UINT timeStart = 0;
	UINT timeStop = 0;
	UINT FrameNum = 0;
	ULONG RetSize = 0;

	UINT DeviceErrCnt = 0;
	CString FpsInfo;
	int x = 0;

	int bRet = 0;
	WORD ImgH = 0, ImgV = 0;

	LPBYTE bmpBuffer = NULL;
	ULONG retSize = 0;
	UINT crcCount = 0;

	USHORT width = pSensor.width;
	USHORT height = pSensor.height;
	BYTE type = pSensor.type;
	FrameInfo frameInfo;
	bmpBuffer = (LPBYTE)malloc(width * height * 4);
	//allocate the bmp buffer.
	UINT nSize = width * height * 3 + 1024 * 1024;
	LPBYTE cameraBuffer = NULL;
	cameraBuffer = (LPBYTE)malloc(nSize);

	UINT CrcCount;
	if ((CalibrateSensorPort(8000, 0) == DT_ERROR_TIME_OUT))
	{
		DBOUT("Calibrate Sensor Port Error \n");
	}
	GetMipiCrcErrorCount(&CrcCount, CHANNEL_A, 0);
	USHORT OldCrcCount = CrcCount;
	timeStart = GetTickCount();
	while (*cameraWorkStatus == CAMERA_START)
	{
		int ret = GrabFrame(cameraBuffer, grabSize, &retSize, &frameInfo, iDevID);
		if (ret == DT_ERROR_OK)
		{
			GetMipiCrcErrorCount(&crcCount, CHANNEL_A, iDevID);
		}
		BOOL bRaw10 = FALSE;
		ImageProcess(cameraBuffer, bmpBuffer, width, height, &frameInfo, iDevID);
		if (*grabWorkStatus == GRAB_START)
		{
			DBOUT("Save image to local drive started\n");
			auto start = std::chrono::system_clock::now();
			auto t_start = std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch()).count(); //convert now() to timestamp in ms
			//dk->SaveBmpFile("output/" + std::to_string(iDevID) + std::to_string(t_start) + ".bmp", bmpBuffer, width, height);
			dk->SaveBmpFile("output/camera" + std::to_string(iDevID) + ".bmp", bmpBuffer, width, height);
			*grabWorkStatus = GRAB_STOP;
			DBOUT("Save image to local drive finished\n");
		}
		DisplayRGB24(bmpBuffer, &frameInfo, iDevID);
		Sleep(1);
	}
	if (*cameraWorkStatus == CAMERA_STOP)
	{
		DBOUT("Camera Stop \n");
	}
	delete(bmpBuffer);
	delete(cameraBuffer);
	bmpBuffer = NULL;
	cameraBuffer = NULL;
	SetEvent(handle);
	_endthread();
}

void Camera_ThreadA(void *params)
{
	Camera_Thread(params, 0);
}

void Camera_ThreadB(void *params)
{
	Camera_Thread(params, 1);
}
