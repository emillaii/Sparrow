#pragma once
#include <sstream>
#include <vector>
#include <opencv2\opencv.hpp>
#include <Eigen/EigenValues>
#include <Eigen/Dense>
#include "Sfr_entry.h"

using namespace cv;

#define DBOUT( s )            \
{                             \
   CString t = CTime::GetCurrentTime().Format("%y-%m:%d %H:%M:%S : "); \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugStringW( t + os_.str().c_str() );  \
}
 
struct mtfPoint {
	int group;
	double x;
	double y;
	double mtf50;
};

struct threeDPoint {
	threeDPoint() : x(0), y(0), z(0)
	{

	};
	threeDPoint(double xx, double yy, double zz)
		: x(xx), y(yy), z(zz)
	{
	};
	double x;
	double y;
	double z;
};

void split(const std::string& str, std::vector<std::string>& v);
void split(const std::string& str, std::vector<double>& v);
void sfrCurveFitting(vector<Sfr_entry> v, double* a, double& r2, double& x_min, double& x_max, double &ex, double &ey, double &ez);
void curveFitting(double x[], double y[], const int sampleSize, double* a, double& r2); 
void planeFitting(std::vector<threeDPoint> points);
vector<vector<Sfr_entry>> sfr_curve_analysis(vector<vector<Sfr_entry>> const sfr_sampling, bool isShowGraph);
long long milliseconds_now();
void saveSFR(vector<vector<Sfr_entry>> sfr_list);
vector<vector<Sfr_entry>> loadSFR();
void gnuplot(vector<CString> equations, double lb, double rb);