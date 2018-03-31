#include "stdafx.h"
#include "Utils.h"
#include <fstream>
#include "Log.h"

long long milliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}


void split(const std::string& str, std::vector<std::string>& v) {
	std::stringstream ss(str);
	ss >> std::noskipws;
	std::string field;
	char ws_delim;
	while (1) {
		if (ss >> field)
			v.push_back(field);
		else if (ss.eof())
			break;
		else
			v.push_back(std::string());
		ss.clear();
		ss >> ws_delim;
	}
}

void split(const std::string& str, std::vector<double>& v) {
	std::stringstream ss(str);
	ss >> std::noskipws;
	std::string field;
	char ws_delim;
	while (1) {
		if (ss >> field)
			v.push_back(std::stod(field));
		else if (ss.eof())
			break;
		else
			v.push_back(0);
		ss.clear();
		ss >> ws_delim;
	}
}

void sfrCurveFitting(vector<Sfr_entry> v, double* a, double& r2, double& x_min, double& x_max, double &ex, double &ey,  double &ez) {
	int i, j, k;
	const int N = (int)v.size();
	double minX = 999999;
	double maxX = -999999;
	ex = 0, ey = 0; 
	for (int i = 0; i < N; i++) {
		if (v.at(i).pz < minX) minX = v.at(i).pz;
		if (v.at(i).pz > maxX) maxX = v.at(i).pz;
		ex += v.at(i).px;
		ey += v.at(i).py;
	}
	x_min = minX;
	x_max = maxX;

	ex /= N;
	ey /= N;
	const int n = 4;                          // n is the degree of Polynomial    
	double X[2 * n + 1];                      //Array that will store the values of sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
	for (i = 0; i<2 * n + 1; i++)
	{
		X[i] = 0;
		for (j = 0; j<N; j++)
			X[i] = X[i] + pow(v.at(j).pz - minX, i);        //consecutive positions of the array will store N,sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
	}
	double B[n + 1][n + 2];            //B is the Normal matrix(augmented) that will store the equations, 'a' is for value of the final coefficients
	for (i = 0; i <= n; i++)
		for (j = 0; j <= n; j++)
			B[i][j] = X[i + j];            //Build the Normal matrix by storing the corresponding coefficients at the right positions except the last column of the matrix
	double Y[n + 1];                    //Array to store the values of sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
	for (i = 0; i<n + 1; i++)
	{
		Y[i] = 0;
		for (j = 0; j<N; j++)
			Y[i] = Y[i] + pow(v.at(j).pz - minX, i)*v.at(j).sfr_025;        //consecutive positions will store sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
	}
	for (i = 0; i <= n; i++)
		B[i][n + 1] = Y[i];                //load the values of Y as the last column of B(Normal Matrix but augmented)
	const int m = n + 1;                //n is made n+1 because the Gaussian Elimination part below was for n equations, but here n is the degree of polynomial and for n degree we get n+1 equations
	for (i = 0; i<m; i++)                    //From now Gaussian Elimination starts(can be ignored) to solve the set of linear equations (Pivotisation)
		for (k = i + 1; k<m; k++)
			if (B[i][i]<B[k][i])
				for (j = 0; j <= m; j++)
				{
					double temp = B[i][j];
					B[i][j] = B[k][j];
					B[k][j] = temp;
				}

	for (i = 0; i<m - 1; i++)            //loop to perform the gauss elimination
		for (k = i + 1; k<m; k++)
		{
			double t = B[k][i] / B[i][i];
			for (j = 0; j <= m; j++)
				B[k][j] = B[k][j] - t * B[i][j];    //make the elements below the pivot elements equal to zero or elimnate the variables
		}
	for (i = m - 1; i >= 0; i--)                //back-substitution
	{                        //x is an array whose values correspond to the values of x,y,z..
		a[i] = B[i][m];                //make the variable to be calculated equal to the rhs of the last equation
		for (j = 0; j<m; j++)
			if (j != i)            //then subtract all the lhs values except the coefficient of the variable whose value                                   is being calculated
				a[i] = a[i] - B[i][j] * a[j];
		a[i] = a[i] / B[i][i];            //now finally divide the rhs by the coefficient of the variable to be calculated
	}
	double ymean = 0;
	double sse = 0;
	double ssto = 0;
	for (i = 0; i < N; i++)
		ymean += v.at(i).sfr_025;
	ymean /= N;
	for (i = 0; i < N; i++)
	{
		double ey = 0;
		for (j = 0; j < m; j++)
			ey += a[j] * pow(v.at(i).pz-minX, j);
		sse += pow(ey - v.at(i).sfr_025, 2);
		ssto += pow(v.at(j).sfr_025 - ymean, 2);
	}
	r2 = 1 - sse / ssto;

	double delta = (maxX - minX) / 300;
	double localMaxima = -999999;
	double localMaximaZ = minX; 
	for (double z = minX; z < maxX; z += delta) {
		double ey = 0; 
		for (int c = 0; c < m; c++) {
			ey += a[c] * pow(z-minX, c);
		}
		if (ey > localMaxima) {
			localMaxima = ey;
			localMaximaZ = z; 
		}
	}
	//DBOUT(localMaximaZ << "\n");
	
	ez = localMaximaZ;
}


void curveFitting(double x[], double y[], const int sampleSize, double* a, double& r2) {
	int i, j, k;
	const int N = sampleSize;
	const int n = 6;                          // n is the degree of Polynomial    
	double X[2 * n + 1];                      //Array that will store the values of sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
	for (i = 0; i<2 * n + 1; i++)
	{
		X[i] = 0;
		for (j = 0; j<N; j++)
			X[i] = X[i] + pow(x[j], i);        //consecutive positions of the array will store N,sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
	}
	double B[n + 1][n + 2];            //B is the Normal matrix(augmented) that will store the equations, 'a' is for value of the final coefficients
	for (i = 0; i <= n; i++)
		for (j = 0; j <= n; j++)
			B[i][j] = X[i + j];            //Build the Normal matrix by storing the corresponding coefficients at the right positions except the last column of the matrix
	double Y[n + 1];                    //Array to store the values of sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
	for (i = 0; i<n + 1; i++)
	{
		Y[i] = 0;
		for (j = 0; j<N; j++)
			Y[i] = Y[i] + pow(x[j], i)*y[j];        //consecutive positions will store sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
	}
	for (i = 0; i <= n; i++)
		B[i][n + 1] = Y[i];                //load the values of Y as the last column of B(Normal Matrix but augmented)
	const int m = n + 1;                //n is made n+1 because the Gaussian Elimination part below was for n equations, but here n is the degree of polynomial and for n degree we get n+1 equations
	for (i = 0; i<m; i++)                    //From now Gaussian Elimination starts(can be ignored) to solve the set of linear equations (Pivotisation)
		for (k = i + 1; k<m; k++)
			if (B[i][i]<B[k][i])
				for (j = 0; j <= m; j++)
				{
					double temp = B[i][j];
					B[i][j] = B[k][j];
					B[k][j] = temp;
				}

	for (i = 0; i<m - 1; i++)            //loop to perform the gauss elimination
		for (k = i + 1; k<m; k++)
		{
			double t = B[k][i] / B[i][i];
			for (j = 0; j <= m; j++)
				B[k][j] = B[k][j] - t * B[i][j];    //make the elements below the pivot elements equal to zero or elimnate the variables
		}
	for (i = m - 1; i >= 0; i--)                //back-substitution
	{                        //x is an array whose values correspond to the values of x,y,z..
		a[i] = B[i][m];                //make the variable to be calculated equal to the rhs of the last equation
		for (j = 0; j<m; j++)
			if (j != i)            //then subtract all the lhs values except the coefficient of the variable whose value                                   is being calculated
				a[i] = a[i] - B[i][j] * a[j];
		a[i] = a[i] / B[i][i];            //now finally divide the rhs by the coefficient of the variable to be calculated
	}
	double ymean = 0;
	double sse = 0;
	double ssto = 0;
	for (i = 0; i < N; i++)
		ymean += y[i];
	ymean /= N;
	for (i = 0; i < N; i++)
	{
		double ey = 0;
		for (j = 0; j < m; j++)
			ey += a[j] * pow(x[i], j);
		sse += pow(ey - y[i], 2);
		ssto += pow(y[i] - ymean, 2);
	}
	r2 = 1 - sse / ssto;
}

//Plane Fitting 

void get_plane(const threeDPoint & centroid, const threeDPoint & normal, double & a, double & b, double & c, double & d) {
	d = -(centroid.x * normal.x + centroid.y * normal.y + centroid.z * normal.z);
	a = normal.x;
	b = normal.y;
	c = normal.z;
	//std::cout << "The plane is " << weighted_dir.x << "*x + " << weighted_dir.y << "*y + " << weighted_dir.z << "*z + " << d << "=0" << std::endl;
}

double dot_product(threeDPoint a, threeDPoint b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

void normalize(threeDPoint & vec) {
	double len = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
	len = sqrt(len);
	if (len <= 0)
		return;
	vec.x /= len;
	vec.y /= len;
	vec.z /= len;
}

double cal_error(const std::vector<threeDPoint> & points, double a, double b, double c, double d) {
	double error = 0;
	double denum = a * a + b * b + c * c;
	if (denum <  0.0001)
		return error;
	for (auto it = points.begin(); it != points.end(); ++it) {
		double numerator = std::fabs(a * it->x + b * it->y + c * it->z + d);
		error += numerator / denum;
	}
	return error;
}

void planeFitting(std::vector<threeDPoint> points) {
	double xx_sum = 0, yy_sum = 0, zz_sum = 0;
	int total = (int)points.size();
	DBOUT("There are " << total << " points in total" << std::endl);
	for (int i = 0; i < total; ++i) {
		xx_sum += points[i].x;
		yy_sum += points[i].y;
		zz_sum += points[i].z;
	}
	double cx = xx_sum / total;
	double cy = yy_sum / total;
	double cz = zz_sum / total;
	threeDPoint centroid(cx, cy, cz);
	DBOUT("The centroid of the cloud is (" << cx << ", " << cy << ", " << cz << ")." << std::endl);
	double xx = 0, xy = 0, xz = 0;
	double yy = 0, yz = 0, zz = 0;
	for (int i = 0; i < total; ++i) {
		double dev_x = points[i].x - cx;
		double dev_y = points[i].y - cy;
		double dev_z = points[i].z - cz;
		xx += dev_x * dev_x;
		xy += dev_x * dev_y;
		xz += dev_x * dev_z;
		yy += dev_y * dev_y;
		yz += dev_y * dev_z;
		zz += dev_z * dev_z;
	}
	xx /= total;
	xy /= total;
	xz /= total;
	yy /= total;
	yz /= total;
	zz /= total;
	// Method 1
	threeDPoint weighted_dir = threeDPoint(0, 0, 0);
	{
		double det_x = yy * zz - yz * yz;
		double axis_x = det_x;
		double axis_y = xz * yz - xy * zz;
		double axis_z = xy * yz - xz * yy;
		threeDPoint axis_dir(axis_x, axis_y, axis_z);
		//std::cout << "axis_dir: (" << axis_dir.x << ", " << axis_dir.y << ", " << axis_dir.z << ")" << std::endl;
		double weight = det_x * det_x;
		if (dot_product(weighted_dir, axis_dir) < 0.0) {
			weight = -weight;
		}
		weighted_dir.x += axis_dir.x * weight;
		weighted_dir.y += axis_dir.y * weight;
		weighted_dir.z += axis_dir.z * weight;
	}
	{
		double det_y = xx * zz - xz * xz;
		double axis_x = xz * yz - xy * zz;
		double axis_y = det_y;
		double axis_z = xy * xz - yz * xx;
		threeDPoint axis_dir(axis_x, axis_y, axis_z);
		//std::cout << "axis_dir: (" << axis_dir.x << ", " << axis_dir.y << ", " << axis_dir.z << ")" << std::endl;
		double weight = det_y * det_y;
		if (dot_product(weighted_dir, axis_dir) < 0.0) {
			weight = -weight;
		}
		weighted_dir.x += axis_dir.x * weight;
		weighted_dir.y += axis_dir.y * weight;
		weighted_dir.z += axis_dir.z * weight;
	}
	{
		double det_z = xx * yy - xy * xy;
		double axis_x = xy * yz - xz * yy;
		double axis_y = xy * xz - yz * xx;
		double axis_z = det_z;
		threeDPoint axis_dir(axis_x, axis_y, axis_z);
		//std::cout << "axis_dir: (" << axis_dir.x << ", " << axis_dir.y << ", " << axis_dir.z << ")" << std::endl;
		double weight = det_z * det_z;
		if (dot_product(weighted_dir, axis_dir) < 0.0) {
			weight = -weight;
		}
		weighted_dir.x += axis_dir.x * weight;
		weighted_dir.y += axis_dir.y * weight;
		weighted_dir.z += axis_dir.z * weight;
		normalize(weighted_dir);
		DBOUT("weighted normal vector: (" << weighted_dir.x << ", " << weighted_dir.y << ", " << weighted_dir.z << ")" << std::endl);
		double a, b, c, d;
		get_plane(centroid, weighted_dir, a, b, c, d);
		DBOUT("Method 1: " << std::endl);
		DBOUT("The plane is " << a << "*x + " << b << "*y + " << c << "*z + " << d << "=0" << std::endl);
		DBOUT("Normal vector: (" << weighted_dir.x << ", " << weighted_dir.y << ", " << weighted_dir.z << ")" << std::endl);
		double error = cal_error(points, a, b, c, d);
		DBOUT( "Total error: " << error << std::endl );
	}
}

vector<vector<Sfr_entry>> sfr_curve_analysis(vector<vector<Sfr_entry>> const sfr_sampling, bool isShowGraph)
{
	vector<vector<Sfr_entry>> input = sfr_sampling;
	vector<vector<Sfr_entry>> clustered;
	vector<vector<Sfr_entry>> averaged;
	for (int i = 0; i < input.size(); i++) {
		vector<Sfr_entry> v1;
		for (int ii = 0; ii < input.at(i).size(); ii++) {
			Sfr_entry s = input.at(i).at(ii);
			Sfr_entry avg;
			avg.sfr_025 += s.sfr_025;
			avg.group = s.group; avg.px += s.px; avg.py += s.py;
			avg.pz = s.pz;
			int count = 1;
			for (int iii = ii + 1; iii < input.at(i).size(); iii++)
			{
				if (input.at(i).at(iii).group == s.group) {
					avg.sfr_025 += input.at(i).at(iii).sfr_025;
					avg.px += input.at(i).at(iii).px; avg.py += input.at(i).at(iii).py;
					count++;
					ii++;
				}
			}
			avg.sfr_025 /= count;
			avg.px /= count;
			avg.py /= count;
			v1.push_back(avg);
		}
		averaged.push_back(v1);
	}

	for (int i = 0; i < averaged.size(); i++) {
		vector<Sfr_entry> v1;
		for (int ii = 0; ii < averaged.at(i).size(); ii++) {
			Sfr_entry s = averaged.at(i).at(ii);
			v1.push_back(s);
			if (i != averaged.size() - 1) {
				for (int j = i + 1; j < averaged.size(); j++) {
					int index = -1; // Find closest index 
					double min_dist = 99999;
					for (int k = 0; k < averaged.at(j).size(); k++) {
						Sfr_entry ss = averaged.at(j).at(k);
						double dist = sqrt(pow((s.px - ss.px), 2) + pow((s.py - ss.py), 2));
						if (dist < 100 && dist < min_dist) {
							index = k;
							min_dist = dist;
							s = ss;   // Update the sampling point 
						}
					}
					if (index != -1) {
						v1.push_back(averaged.at(j).at(index));
						averaged.at(j).erase(averaged.at(j).begin() + index);
					}
				}
			}
			if (v1.size() > 0) {
				clustered.push_back(v1);
				v1.clear();
			}
		}
	}
	//this->sfr_sampling = clustered;
	//Curve Fitting 
	vector<threeDPoint> points;
	vector<CString> v;
	bool is_set_y_value = false;
	double g_x_min = 99999;
	double g_x_max = -99999;
	for (int i = 0; i < clustered.size(); i++) {
		double r2;
		double c[7] = { 0 };
		threeDPoint point;
		if (clustered.at(i).size() > 5) {
			double x_min;    //x_min and x_max are the plotting range
			double x_max;
			sfrCurveFitting(clustered.at(i), c, r2, x_min, x_max, point.x, point.y, point.z); // need to transfrom the x y z into um 
			if (g_x_min > x_min) g_x_min = x_min;
			if (g_x_max < x_max) g_x_max = x_max;
			points.push_back(point);
			CString temp;
			temp.Format(_T("[%f:%f] %f + %f*(x-%f) + %f*(x-%f)**2 + %f*(x-%f)**3 + %f*(x-%f)**4 title \"\""), x_min, x_max, c[0], c[1], x_min, c[2], x_min, c[3], x_min, c[4], x_min);
			v.push_back(temp);
		}
	}
	if (isShowGraph) gnuplot(v, g_x_min, g_x_max);
	//Plane Fitting ...
	planeFitting(points);
	return clustered;
}

void saveSFR(vector<vector<Sfr_entry>> sfr_sampling) {
	CFileDialog fileDlg(false, CString(".csv"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CString(".csv"));
	if (fileDlg.DoModal() == IDOK) {
		CString filename = fileDlg.GetFileName();
		CString pathname = fileDlg.GetPathName();
		const int sampleSize = (int)sfr_sampling.size();
		if (sampleSize <= 0) {
			DBOUT(__FUNCTION__ << " Sample size is zero, reject the request \n");
		}
		std::ofstream fout;
		fout.open(filename, std::ios::out | std::ios::app);
		for (int j = 0; j < sfr_sampling.size(); j++) {
			fout << "group, px, py, pz, sfr, \n";
			for (int i = 0; i < sfr_sampling.at(j).size(); i++) {
				fout << sfr_sampling.at(j).at(i).group << ",";
				fout << sfr_sampling.at(j).at(i).px << ",";
				fout << sfr_sampling.at(j).at(i).py << ",";
				fout << sfr_sampling.at(j).at(i).pz << ",";
				fout << sfr_sampling.at(j).at(i).sfr_025 << ",";
				fout << "\n";
			}
			fout << "***************************************************" << "\n";
		}
		fout.close();
		DBOUT(__FUNCTION__ << " Save Success \n");
		DBOUT(fileDlg.GetFileName() << " \n");
	}
}

vector<vector<Sfr_entry>> loadSFR() {
	Log::GetInstance()->Write("[loadSFR] is called");
	vector<vector<Sfr_entry>> output_v; 
	vector<Sfr_entry> v;
	std::string line;
	CFileDialog fileDlg(true, NULL, NULL, OFN_HIDEREADONLY, CString("*.csv|"));
	if (fileDlg.DoModal() == IDOK) {
		CString filename = fileDlg.GetFileName();
		CString pathname = fileDlg.GetPathName();
		std::ifstream infile;
		infile.open(pathname, std::ios::in);
		while ( std::getline(infile, line) ) {
			std::stringstream ss(line);

			vector<double> vect;
			double i;
			while (ss >> i)
			{
				vect.push_back(i);
				if (ss.peek() == ',')
					ss.ignore();
			}
			if (vect.size() == 5) {
				Sfr_entry e;
				e.group = (int)vect[0];
				e.px = vect[1];
				e.py = vect[2];
				e.pz = vect[3];
				e.sfr_025 = vect[4];
				v.push_back(e);
			}
			else if (vect.size() > 0) {
				Log::GetInstance()->Write("[loadSFR] Input data file corrupted!");
			}
			if (line[0] == '*') {
				output_v.push_back(v);
				v.clear();
			}
			//DBOUT(line.c_str() << "\n");
		}
		infile.close();
	}
	return output_v;
}

void gnuplot(vector<CString> equations, double lb, double rb)
{
	Log::GetInstance()->Write("[gnuplot] is called!");
	FILE *fp = _popen("gnuplot", "w");
	if (!fp) { perror("popen gnuplot"); exit(EXIT_FAILURE); };
	fprintf(fp, "set term png size 1024, 768\n");
	CString str;
	str.Format(_T("plot [%f:%f] [0:100] 0 title \"\", "), lb, rb);
	for (auto e : equations) {
		str.AppendFormat(_T("%s,"), e);
	}
	CT2A command(str);
	fprintf(fp, "set output \"graphPlot.png\"\n");
	fprintf(fp, command.m_psz);
	fflush(fp);
	_pclose(fp);
	Log::GetInstance()->Write("[gnuplot] is finished!");
}