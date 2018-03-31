#pragma once
#include "Dothinkey.h"
#include "Sparrow_AppDlg.h"

void Camera_Thread(void *params, int iDevID);
void Camera_ThreadA(void *params);
void Camera_ThreadB(void *params);

void NewAA_Thread(void *params);
void MTF_Live_Thread(void *params);