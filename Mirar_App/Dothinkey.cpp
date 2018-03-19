#include "stdafx.h"
#include "Dothinkey.h"
#include "Utils.h"
#include "threads.h"
#include <process.h>

Dothinkey::Dothinkey()
	: m_iDevIDA(-1)
	, m_iDevIDB(-1)
	, m_fMclkA(12.0f)
	, m_fMclkB(12.0f)
	, m_fAvddA(2.8f)
	, m_fDvddA(1.5f)
	, m_fDovddA(1.8f)
	, m_fAfvccA(2.8f)
	, m_fAvddB(2.8f)
	, m_fDvddB(1.5f)
	, m_fDovddB(1.8f)
	, m_fAfvccB(2.8f)
	, m_uFocusPosA(0)
	, m_uFocusPosB(0)
	, m_vppA(0.0f)
	, m_vppB(0.0f)
	, m_CameraAWorkStatus(CAMERA_STOP)
	, m_CameraBWorkStatus(CAMERA_STOP)
	, m_GrabImageWorkStatusA(GRAB_STOP)
	, m_GrabImageWorkStatusB(GRAB_STOP)
{
}

Dothinkey::~Dothinkey()
{
	if (&current_sensor_a && current_sensor_a.ParaList)
	{
		delete[] current_sensor_a.ParaList;
	}
	if (&current_sensor_b && current_sensor_b.ParaList)
	{
		delete[] current_sensor_b.ParaList;
	}
	if (m_iDevIDA > 0) {
		CloseDevice(m_iDevIDA);
	}
	if (m_iDevIDB > 0) {
		CloseDevice(m_iDevIDB);
	}
}

SensorTab Dothinkey::GetSensorA()
{
	return current_sensor_a;
}

SensorTab Dothinkey::GetSensorB()
{
	return current_sensor_b;
}

void Dothinkey::DothinkeyEnum()
{
	char *DeviceName[4] = { 0 };
	int DeviceNum;
	int i=0;

	m_charCurrentDeviceA = "";
	m_charCurrentDeviceB = "";
	EnumerateDevice(DeviceName, 4, &DeviceNum);
	if (DeviceNum == 0)
	{
		Log::GetInstance()->Write("[DothinkeyEnum] Cannot find any device!");
		return;
	}
	for (i = 0; i < DeviceNum; i++)
	{
		if (DeviceName[i] != nullptr)
		{
			CString temp(DeviceName[i]);
			temp.Append(_T("[DothinkeyEnum]----Device Found!"));
			Log::GetInstance()->WriteString(temp);
			if (i == 0) m_charCurrentDeviceA = DeviceName[i];
			if (i == 1) m_charCurrentDeviceB = DeviceName[i];
		}
	}
}

void Dothinkey::DothinkeyOpen()
{
	CString logString;
	Log::GetInstance()->WriteString(logString);
	if (m_iDevIDA >= 0) CloseDevice(m_iDevIDA);
	if (m_iDevIDB >= 0) CloseDevice(m_iDevIDB);

	int iDevIDA = -1, iDevIDB = -1;
	if (strlen(m_charCurrentDeviceA) && OpenDevice(m_charCurrentDeviceA, &iDevIDA, 0) != DT_ERROR_OK)
	{
		m_charCurrentDeviceA = "....";
		logString.Format(_T("[DothinkeyOpen] OpenDevice error! %s"), m_charCurrentDeviceA);
		DBOUT("OpenDevice error." << m_charCurrentDeviceA << "\n");
		return;
	}
	else
	{
		if (iDevIDA < 0)
		{
			Log::GetInstance()->Write("[DothinkeyOpen] Opendevice Fail.");
		}
		BYTE pSN[32];
		int iBufferSize = 32;
		int pRetLen = 0;
		if (GetDeviceSN(pSN, iBufferSize, &pRetLen, iDevIDA) == DT_ERROR_OK)
		{
			std::string s(reinterpret_cast<const char *>(pSN), 32);
			if (s.length() > 0) {
				DBOUT("Open device succeed! " << m_charCurrentDeviceA << "\n");
				m_iDevIDA = iDevIDA;
			}
		}
	}

	if (strlen(m_charCurrentDeviceB) > 0 && OpenDevice(m_charCurrentDeviceB, &iDevIDB, 0) != DT_ERROR_OK)
	{
		DBOUT("OpenDevice error. " << m_charCurrentDeviceA << "\n");
		return;
	}
	else
	{
		BYTE pSN[32];
		int iBufferSize = 32;
		int pRetLen = 0;
		if (GetDeviceSN(pSN, iBufferSize, &pRetLen, iDevIDB) == DT_ERROR_OK)
		{
			std::string s(reinterpret_cast<const char *>(pSN), 32);
			if (s.length() > 0) {
				DBOUT("Open succeed. " << m_charCurrentDeviceB << "\n");
				m_iDevIDB = iDevIDB;
			}
		}
	}

}

BOOL Dothinkey::DothinkeyLoadIniFile(int channel)
{
	SensorTab *pCurrentSensor;
	channel == 0 ? (pCurrentSensor = &this->current_sensor_a) : (pCurrentSensor = &this->current_sensor_b);
	IniParser *iniParser = new IniParser();
	//std::string str_filename = "C:\\Mirar\\IMX214_4L_3120_063_34.ini";
	std::string str_filename = "C:\\Mirar\\IMX362_4L_3024_063_34.ini";
	iniParser->SetIniFilename(str_filename);

	if (channel == 0)
	{
		m_fMclkA = (float)iniParser->ReadIniData("Sensor", "mclk", 0x01) / 1000;
		m_fAvddA = (float)iniParser->ReadIniData("Sensor", "avdd", 0x00) / 1000;
		m_fDovddA = (float)iniParser->ReadIniData("Sensor", "dovdd", 0x00) / 1000;
		m_fDvddA = (float)iniParser->ReadIniData("Sensor", "dvdd", 0x00) / 1000;
	}
	else if (channel == 1)
	{
		m_fMclkB = (float)iniParser->ReadIniData("Sensor", "mclk", 0x01) / 1000;
		m_fAvddB = (float)iniParser->ReadIniData("Sensor", "avdd", 0x00) / 1000;
		m_fDovddB = (float)iniParser->ReadIniData("Sensor", "dovdd", 0x00) / 1000;
		m_fDvddB = (float)iniParser->ReadIniData("Sensor", "dvdd", 0x00) / 1000;
	}
	pCurrentSensor->width = iniParser->ReadIniData("Sensor", "width", 0);
	pCurrentSensor->height = iniParser->ReadIniData("Sensor", "height", 0);
	pCurrentSensor->type = iniParser->ReadIniData("Sensor", "type", 2);
	pCurrentSensor->port = iniParser->ReadIniData("Sensor", "port", 0);
	pCurrentSensor->pin = iniParser->ReadIniData("Sensor", "pin", 0);
	pCurrentSensor->SlaveID = iniParser->ReadIniData("Sensor", "SlaveID", 0);
	pCurrentSensor->mode = iniParser->ReadIniData("Sensor", "mode", 0);
	pCurrentSensor->FlagReg = iniParser->ReadIniData("Sensor", "FlagReg", 0);
	pCurrentSensor->FlagMask = iniParser->ReadIniData("Sensor", "FlagMask", 0xff);
	pCurrentSensor->FlagData = iniParser->ReadIniData("Sensor", "FlagData", 0);
	pCurrentSensor->FlagReg1 = iniParser->ReadIniData("Sensor", "FlagReg1", 0);
	pCurrentSensor->FlagMask1 = iniParser->ReadIniData("Sensor", "FlagMask1", 0x0);
	pCurrentSensor->FlagData1 = iniParser->ReadIniData("Sensor", "FlagData1", 0);
	pCurrentSensor->outformat = iniParser->ReadIniData("Sensor", "outformat", 0x00);
	pCurrentSensor->mclk = iniParser->ReadIniData("Sensor", "mclk", 0x01);
	pCurrentSensor->avdd = iniParser->ReadIniData("Sensor", "avdd", 0x00);
	pCurrentSensor->dovdd = iniParser->ReadIniData("Sensor", "dovdd", 0x00);
	pCurrentSensor->dvdd = iniParser->ReadIniData("Sensor", "dvdd", 0x00);
	pCurrentSensor->ParaList = new USHORT[8192 * 4];
	pCurrentSensor->ParaListSize = 0;
	pCurrentSensor->SleepParaList = NULL;
	pCurrentSensor->SleepParaListSize = NULL;

	iniParser->GetI2CData(pCurrentSensor);

	delete iniParser;
	return true;
}

BOOL Dothinkey::DothinkeyStartCamera(int channel, CStatic* wnd)
{
	SensorTab *pSensor = nullptr;
	ULONG *grabSize = nullptr;
	int iDevID = -1;
	if (channel == 0)
	{
		pSensor = &current_sensor_a;
		iDevID = m_iDevIDA;
		grabSize = &m_GrabSizeA;
	}
	else if (channel == 1)
	{
		pSensor = &current_sensor_b;
		iDevID = m_iDevIDB;
		grabSize = &m_GrabSizeB;
	}
	channel == 0 ? pSensor = &current_sensor_a : &current_sensor_b;
	channel == 0 ? iDevID = m_iDevIDA : m_iDevIDB;
	//ToDo: KillDataBuffer
	SetSoftPinPullUp(IO_NOPULL, 0);
	if (SetSensorClock(false, (USHORT)(0 * 10), iDevID) != DT_ERROR_OK)
	{
		CloseDevice(iDevID);
		DBOUT("Set Clock Fail! \n");
		return false;
	}
	Sleep(1);
	
	if (SetVoltageMclk(*pSensor, iDevID, m_fMclkA, m_fAvddA, m_fDvddA, m_fDovddA, m_fAfvccA, m_vppA) != DT_ERROR_OK)
	{
		CloseDevice(iDevID);
		DBOUT("Set Voltage and Mclk Failed! \n");
		return false;
	}
	DBOUT("Start Camera Success! Camera channel:" << channel << "\n");

	//I2C init
	SetSensorI2cRate(I2C_400K, iDevID);
	SetSensorI2cRapid(TRUE, iDevID);

	//check sensor is on line or not ,if on line,init sensor to work....
	SensorEnable(pSensor->pin ^ 0x02, 1, iDevID); //reset
	Sleep(20);
	SensorEnable(pSensor->pin, 1, iDevID);
	Sleep(50);

	//check sensoris on line...
	if (SensorIsMe(pSensor, CHANNEL_A, 0, iDevID) != DT_ERROR_OK)
	{
		DBOUT("Sensor is not ok!\r\n");
		return DT_ERROR_FAILED;
	}
	//init sensor....
	if (InitSensor(pSensor->SlaveID,
		pSensor->ParaList,
		pSensor->ParaListSize,
		pSensor->mode, iDevID) != DT_ERROR_OK)
	{
		DBOUT("Init Sensor Failed! \r\n");
		return DT_ERROR_FAILED;
	}

	if (pSensor->type == D_YUV || pSensor->type == D_YUV_SPI || pSensor->type == D_YUV_MTK_S)
		SetYUV422Format(pSensor->outformat, iDevID);
	else
		SetRawFormat(pSensor->outformat, iDevID);
	InitRoi(0, 0, pSensor->width, pSensor->height, 0, 0, 1, 1, pSensor->type, TRUE, iDevID);
	SetSensorPort(pSensor->port, pSensor->width, pSensor->height, iDevID);
	CalculateGrabSize(grabSize, iDevID);
	//open video....
	OpenVideo(*grabSize, iDevID);
	SetMonoMode(true, iDevID);
	InitDisplay(wnd->GetSafeHwnd(), pSensor->width, pSensor->height, pSensor->type, CHANNEL_A, NULL, iDevID);
	InitIsp(pSensor->width, pSensor->height, pSensor->type, CHANNEL_A, iDevID);
	return true;
}

BOOL Dothinkey::SetVoltageMclk(SensorTab CurrentSensor, int iDevID, float Mclk, float Avdd, float Dvdd, float Dovdd, float Afvcc, float vpp)
{
	SENSOR_POWER Power[10] = { POWER_AVDD, POWER_DOVDD, POWER_DVDD, POWER_AFVCC, POWER_VPP };
	int Volt[10] = { 0 };
	int Current[10] = { 300, 300, 300, 300, 100 };//300mA
	BOOL OnOff[10] = { TRUE,TRUE,TRUE,TRUE,TRUE };
	CURRENT_RANGE range[5] = { CURRENT_RANGE_MA, CURRENT_RANGE_MA, CURRENT_RANGE_MA, CURRENT_RANGE_MA, CURRENT_RANGE_MA };
	//set power to 0V
	if (PmuSetVoltage(Power, Volt, 5, iDevID) != DT_ERROR_OK)
	{
		CloseDevice(iDevID);
		DBOUT("Set Voltage Failed! \r\n");
		return FALSE;
	}
	//wait for the power is all to zero....
	Sleep(50);
	if (PmuSetOnOff(Power, OnOff, 5, iDevID) != DT_ERROR_OK)
	{
		CloseDevice(iDevID);
		DBOUT("Open PowerOnOff Failed! \r\n");
		return FALSE;
	}
	Sleep(50);
	// 1. set power the avdd
	Volt[POWER_DOVDD] = (int)(Dovdd * 1000); // 2.8V
	if (PmuSetVoltage(Power, Volt, 5, iDevID) != DT_ERROR_OK)
	{
		DBOUT("Open PowerOn Failed!\r\n");
		return DT_ERROR_FAILED;
	}
	Sleep(2);

	// 2, set power the dvdd 
	Volt[POWER_DVDD] = (int)(Dvdd * 1000); // 2.8V
	if (PmuSetVoltage(Power, Volt, 5, iDevID) != DT_ERROR_OK)
	{
		DBOUT("Open PowerOn Failed!\r\n");
		return DT_ERROR_FAILED;
	}
	Sleep(2);

	// 3, set power the dovdd
	Volt[POWER_AVDD] = (int)(Avdd * 1000); // 2.8V
	if (PmuSetVoltage(Power, Volt, 5, iDevID) != DT_ERROR_OK)
	{
		DBOUT("Open PowerOn Failed!\r\n");
		return DT_ERROR_FAILED;
	}
	Sleep(2);
	//4. set power the afvcc and vpp
	Volt[POWER_AFVCC] = (int)(Afvcc * 1000); // 2.8V
	Volt[POWER_VPP] = (int)(vpp * 1000); // 2.8V 
	if (PmuSetVoltage(Power, Volt, 5, iDevID) != DT_ERROR_OK)
	{
		DBOUT("Open PowerOn Failed!\r\n");
		return DT_ERROR_FAILED;
	}
	//should wait for 50ms to be ready...
	Sleep(50);
	//first set pin definition...
	{
		BYTE  pinDef[40] = { 0 };
		if (CurrentSensor.port == PORT_MIPI || CurrentSensor.port == PORT_HISPI)
		{
			pinDef[0] = 20;
			pinDef[1] = 0;
			pinDef[2] = 2;
			pinDef[3] = 1;
			pinDef[4] = 3;
			pinDef[5] = 4;
			pinDef[6] = 5;
			pinDef[7] = 6;
			pinDef[8] = 7;
			pinDef[9] = 8;
			pinDef[10] = 9;
			pinDef[11] = 20;
			pinDef[12] = 10;
			pinDef[13] = 11;
			pinDef[14] = 12;
			pinDef[15] = 20;
			pinDef[16] = 20;
			pinDef[17] = 13;
			pinDef[18] = 15;
			pinDef[19] = 14;
			pinDef[20] = 19;
			pinDef[21] = 18;
			pinDef[22] = 20;
			pinDef[23] = 16;
			pinDef[24] = 20;
			pinDef[25] = 20;
		}
		else  //standard parallel..
		{

			pinDef[0] = 16;
			pinDef[1] = 0;
			pinDef[2] = 2;
			pinDef[3] = 1;
			pinDef[4] = 3;
			pinDef[5] = 4;
			pinDef[6] = 5;
			pinDef[7] = 6;
			pinDef[8] = 7;
			pinDef[9] = 8;
			pinDef[10] = 9;
			pinDef[11] = 20;
			pinDef[12] = 10;
			pinDef[13] = 11;
			pinDef[14] = 12;
			pinDef[15] = 20;
			pinDef[16] = 20;
			pinDef[17] = 20;
			pinDef[18] = 20;
			pinDef[19] = 20;
			pinDef[20] = 13;
			pinDef[21] = 20;
			pinDef[22] = 14;
			pinDef[23] = 15;
			pinDef[24] = 18;
			pinDef[25] = 19;
		}
		SetSoftPin(pinDef, iDevID);
	}
	EnableSoftPin(TRUE, iDevID);
	EnableGpio(TRUE, iDevID);

	int SampleSpeed[5] = { 100,100,100,100,100 };
	PmuSetSampleSpeed(Power, SampleSpeed, 5, iDevID);
	Sleep(10);
	{
		PmuSetCurrentRange(Power, range, 5, iDevID);
		PmuSetOcpCurrentLimit(Power, Current, 5, iDevID);
	}
	if (SetSensorClock(TRUE, (USHORT)(Mclk * 10), iDevID) != DT_ERROR_OK)
	{
		DBOUT("Set Mclk Failed!\n");
		return DT_ERROR_FAILED;
	}
	SetSoftPinPullUp(IO_PULLUP, iDevID);
	return TRUE;
}

BOOL Dothinkey::DothinkeyGrabImage(int channel)
{
	LPBYTE bmpBuffer = NULL;
	SensorTab *pSensor = nullptr;
	ULONG retSize = 0;
	int iDevID = -1;
	UINT crcCount = 0;
	int grabSize;
	if (channel == 0)
	{
		pSensor = &current_sensor_a;
		iDevID = this->m_iDevIDA;
		grabSize = this->m_GrabSizeA;
	}
	else if (channel == 1)
	{
		pSensor = &current_sensor_b;
		iDevID = this->m_iDevIDB;
		grabSize = this->m_GrabSizeB;
	}

	USHORT width = pSensor->width;
	USHORT height = pSensor->height;
	BYTE type = pSensor->type;
	FrameInfo frameInfo;
	bmpBuffer = (LPBYTE)malloc(width * height * 4);

	if (bmpBuffer == NULL)
	{
		DBOUT("Malloc BMP buffer fail. \r\n");
	}

	//allocate the bmp buffer.
	UINT nSize = width * height * 3 + 1024 * 1024;
	LPBYTE CameraBuffer = NULL;
	CameraBuffer = (LPBYTE)malloc(nSize);
	if ((CameraBuffer == NULL))
	{
		return false;
	}
	memset(CameraBuffer, 0, nSize);
	for (int i = 0; i < 10; i++)
	{
		DBOUT("Grabbing image: " << i << "\n");
		int ret = GrabFrame(CameraBuffer, grabSize, &retSize, &frameInfo, iDevID);
		if (ret == DT_ERROR_OK)
		{
			GetMipiCrcErrorCount(&crcCount, CHANNEL_A, iDevID);
		}
		//std::time_t result = std::time(nullptr);
		//std::string filename = "abc.jpg";
		BOOL bRaw10 = FALSE;
		ImageProcess(CameraBuffer, bmpBuffer, width, height, &frameInfo, iDevID);
		DisplayRGB24(bmpBuffer, &frameInfo, iDevID);
		SaveBmpFile(std::to_string(iDevID) + "_" + std::to_string(i) + ".bmp", bmpBuffer, width, height);
		Sleep(1000);
	}
	delete(bmpBuffer);
	delete(CameraBuffer);
	bmpBuffer = NULL;
	CameraBuffer = NULL;
	return TRUE;
}


BOOL Dothinkey::DothinkeyStartCameraCapture(int channel)
{
	SensorTab *pSensor = nullptr;
	if (channel == 0)
	{
		pSensor = &current_sensor_a;
		if (m_CameraAWorkStatus == CAMERA_START)
			return true;
		m_CameraAWorkStatus = CAMERA_START;
		m_hCameraAThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		_beginthread(Camera_ThreadA, 0, (void*)this);
	}
	else if (channel == 1)
	{
		pSensor = &current_sensor_b;
		if (m_CameraBWorkStatus == CAMERA_START)
			return true;
		m_CameraBWorkStatus = CAMERA_START;
		m_hCameraBThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		_beginthread(Camera_ThreadB, 0, (void*)this);
	}
	return true;
}

BOOL Dothinkey::DothinkeyStopCameraCapture(int channel)
{
	if (channel == 0)
	{
		if (m_CameraAWorkStatus == CAMERA_STOP) return true;
		m_CameraAWorkStatus = CAMERA_STOP;
		if (m_hCameraAThreadEvent)
		{
			DBOUT("Waiting for the camera thread A end \n");
			WaitForSingleObject(m_hCameraAThreadEvent, INFINITE);
			DBOUT("Stop camera A capture success \n");
			CloseHandle(m_hCameraAThreadEvent);
			m_hCameraAThreadEvent = NULL;
		}
	}
	else if (channel == 1)
	{
		if (m_CameraBWorkStatus == CAMERA_STOP) return true;
		m_CameraBWorkStatus = CAMERA_STOP;
		if (m_hCameraBThreadEvent)
		{
			DBOUT("Waiting for the camera thread B end \n");
			WaitForSingleObject(m_hCameraBThreadEvent, INFINITE);
			DBOUT("Stop camera B capture success \n");
			CloseHandle(m_hCameraBThreadEvent);
			m_hCameraBThreadEvent = NULL;
		}
	}
	return true;
}

BOOL Dothinkey::DothinkeySaveImage(int channel)
{
	if (channel == 0)
	{
		if (m_CameraAWorkStatus != CAMERA_START) return false;
		m_GrabImageWorkStatusA = GRAB_START;
		while (m_GrabImageWorkStatusA != GRAB_STOP) {
			Sleep(100);
			DBOUT("Waiting for grabbing image \n");
		}
		DBOUT("Grabbing image finished \n");
	}
	else if (channel == 1)
	{
		if (m_CameraBWorkStatus != CAMERA_START) return false;
		m_GrabImageWorkStatusB = GRAB_STOP;
	}
	return true;
}

BOOL Dothinkey::SaveRawFile(std::string sfilename, BYTE *pBuffer, UINT width, UINT height, UINT type)
{
	HFILE rawFile;
	DWORD dwSizeImage = width * height * type;
	rawFile = _lcreat(sfilename.c_str(), 0);
	if (rawFile < 0)
		return FALSE;
	UINT len;
	len = _lwrite(rawFile, (LPSTR)pBuffer, dwSizeImage);
	_lclose(rawFile);

	return TRUE;
}

BOOL Dothinkey::SaveBmpFile(std::string sfilename, BYTE *pBuffer, UINT width, UINT height)
{
	int				 OffBits;
	HFILE			 bmpFile;
	BITMAPFILEHEADER bmpHeader; // Header for Bitmap file
	BITMAPINFO		 bmpInfo;

	OffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	DWORD dwSizeImage = width * height * 3;//IMAGESIZE_X*IMAGESIZE_Y*3;

	bmpHeader.bfType = ((WORD)('M' << 8) | 'B');
	bmpHeader.bfSize = OffBits + dwSizeImage;
	bmpHeader.bfReserved1 = 0;
	bmpHeader.bfReserved2 = 0;
	bmpHeader.bfOffBits = OffBits;

	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = height;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = 0;
	bmpInfo.bmiHeader.biXPelsPerMeter = 0;
	bmpInfo.bmiHeader.biYPelsPerMeter = 0;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;

	bmpFile = _lcreat(sfilename.c_str(), FALSE);
	if (bmpFile < 0)
	{
		DBOUT("Cannot create the bmp file.\r\n");
		return FALSE;
	}

	UINT len;
	len = _lwrite(bmpFile, (LPSTR)&bmpHeader, sizeof(BITMAPFILEHEADER));
	len = _lwrite(bmpFile, (LPSTR)&bmpInfo, sizeof(BITMAPINFOHEADER));
	len = _lwrite(bmpFile, (LPSTR)pBuffer, bmpHeader.bfSize - sizeof(bmpHeader) - sizeof(bmpInfo) + 4);  //+4 is for exact filesize
	_lclose(bmpFile);

	return TRUE;
}

void Dothinkey::MipiRaw10ToP10(BYTE *pIn, BYTE *pOut, int width, int height)
{
	int index = 0;
	LONG m_GrabSize = width * height * 5 / 4;
	for (int i = 0; i <m_GrabSize; i = i + 5)
	{
		pOut[index++] = (pIn[i + 4] & 0x03) << 6;
		pOut[index++] = pIn[i];

		pIn[i + 4] >>= 2;
		pOut[index++] = (pIn[i + 4] & 0x03) << 6;
		pOut[index++] = pIn[i + 1];

		pIn[i + 4] >>= 2;
		pOut[index++] = (pIn[i + 4] & 0x03) << 6;
		pOut[index++] = pIn[i + 2];

		pIn[i + 4] >>= 2;
		pOut[index++] = (pIn[i + 4] & 0x03) << 6;
		pOut[index++] = pIn[i + 3];

	}
}