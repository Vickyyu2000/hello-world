#include "HermiteSpline.h"

#include "GlobalResourceManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;


HermiteSpline::HermiteSpline(const std::string& name):BaseSystem(name), totalLength(0.0)
{
	// Hermite Matrix
	M[0] = 2;
	M[1] = -2;
	M[2] = 1;
	M[3] = 1;

	M[4] = -3;
	M[5] = 3;
	M[6] = -2;
	M[7] = -1;

	M[8] = 0;
	M[9] = 0;
	M[10] = 1;
	M[11] = 0;

	M[12] = 1;
	M[13] = 0;
	M[14] = 0;
	M[15] = 0;

	// Catmull-Rom Matrix 
	M_CR[0] = -1;
	M_CR[1] = 3;
	M_CR[2] = -3;
	M_CR[3] = 1;

	M_CR[4] = 2;
	M_CR[5] = -5;
	M_CR[6] = 4;
	M_CR[7] = -1;

	M_CR[8] = -1;
	M_CR[9] = 0;
	M_CR[10] = 1;
	M_CR[11] = 0;

	M_CR[12] = 0;
	M_CR[13] = 2;
	M_CR[14] = 0;
	M_CR[15] = 0;

	// M_CR = 0.5 * M_CR
	multNumArray(0.5, M_CR, 4, 4);
}
 
HermiteSpline::~HermiteSpline()
{
	ClearAll();
}

void HermiteSpline::Clean()
{
	// points and tangents not deleted. 
	
	//segments;   
	for (int i = 0; i < segments.size(); i ++)
	{
		CubicCurve_S *cc = segments.at(i);
		for (int j = 0; j < cc->u_length_table.size(); j++)
		{
			u_length_entry_S* te = cc->u_length_table.at(j);
			cc->u_length_table.at(j) = NULL;
			delete te;
		}
		cc->u_length_table.clear();

		segments.at(i) = NULL;
		delete cc;
	}
	segments.clear();

	//samplePoints;	
	for (int i = 0; i < samplePoints.size(); i++)
	{
		double* p = samplePoints.at(i);
		samplePoints.at(i) = NULL;
		delete[3] p;
	}
	samplePoints.clear();

}

void HermiteSpline::ClearAll()
{
	for (int i = 0; i < points.size(); i++)
	{
		double* p = points.at(i);
		points.at(i) = NULL;
		delete[3] p;

		double *pt = tangents.at(i);
		tangents.at(i) = NULL;
		delete[3] pt;
	}

	points.clear();
	tangents.clear();

	Clean();
}
void HermiteSpline::getState(double* p)
{
}

void HermiteSpline::setState(double* p)
{
}

void HermiteSpline::reset(double time)
{
	// nothing to do.
}

// u is global value.
void HermiteSpline::GetPositionTangent(double u, double* pos, double* tangent)
{ 
	// fine the object segment.
	int i = 0;
	int n = segments.size();

	if(u >= 1)
	{
		i = n - 1;
		double local_u = 1;
		double C[4];
		C[0] = segments[i]->ax;
		C[1] = segments[i]->bx;
		C[2] = segments[i]->cx;
		C[3] = segments[i]->dx;
		
		pos[0] = EvaluatePos(local_u, C);
		tangent[0] = EvaluateTangent(local_u, C);

		C[0] = segments[i]->ay;
		C[1] = segments[i]->by;
		C[2] = segments[i]->cy;
		C[3] = segments[i]->dy;
		pos[1] = EvaluatePos(local_u, C);
		tangent[1] = EvaluateTangent(local_u, C);

		//
		pos[2] = 0;
		tangent[2] = 0;

		return;
	}

	while (i < n && u > segments.at(i)->endPointU)
	{
		i++;
	}

	double start_u = 0;

	if (i > 0)
	{
		start_u = segments.at(i - 1)->endPointU;
	}

	// do the work.
	if ( i < segments.size())
	{
		double local_u = (u - start_u)/(segments.at(i)->endPointU - start_u);  
	 
		double C[4];
		C[0] = segments[i]->ax;
		C[1] = segments[i]->bx;
		C[2] = segments[i]->cx;
		C[3] = segments[i]->dx;
 
		pos[0] = EvaluatePos(local_u, C);
		tangent[0] = EvaluateTangent(local_u, C);

		C[0] = segments[i]->ay;
		C[1] = segments[i]->by;
		C[2] = segments[i]->cy;
		C[3] = segments[i]->dy;
		pos[1] = EvaluatePos(local_u, C);
		tangent[1] = EvaluateTangent(local_u, C);

		//
		pos[2] = 0;
		tangent[2] = 0;
	}

	return;
}

void HermiteSpline::display(GLenum mode)
{

	glEnable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);

	for (int i = 0; i < points.size(); i++)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		double* p = points.at(i);
		glTranslated(p[0], p[1], p[2]);
		
		glutSolidSphere(0.1, 20, 20);

		glPopMatrix();
		glPopAttrib();
	}

	glPushMatrix();
	
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < samplePoints.size(); i++)
	{
		glColor3f(1.0, 0, 0);
		glVertex3f(samplePoints[i][0], samplePoints[i][1], samplePoints[i][2]);
	}
	glEnd();

	glPopMatrix();


}

// 
int HermiteSpline::command(int argc, myCONST_SPEC char** argv)
{
	if (argc < 1)
	{
		animTcl::OutputMessage("system %s: wrong number of params.", m_name.c_str());
		return TCL_ERROR;
	}
	else if (strcmp(argv[0], "load") == 0)
	{
		if (argc == 2)
		{
			ClearAll();
			LoadFile(argv[1]);
			//p480
			InitHermite();			
			glutPostRedisplay();

			animTcl::OutputMessage("system %s: load %s. %d points", m_name.c_str(), argv[1], points.size());

			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage: load <file_name>");
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "cr") == 0)
	{		
		CatmullRomInitialization();
		glutPostRedisplay();
		animTcl::OutputMessage("Catmull-Rom Initialization done!");

		return TCL_OK;

	}
	else if (strcmp(argv[0], "export") == 0)
	{
		if (argc == 2)
		{
			return ExportFile(argv[1]);			 
		}
		else
		{
			animTcl::OutputMessage("Usage: export <file_name>");
			return TCL_ERROR;
		}

	}
	else if (strcmp(argv[0], "getArcLength") == 0)
	{
		if (argc == 2)
		{
			double t = atof(argv[1]);
			double length = GetGlobalArcLength(t);
			//
			animTcl::OutputMessage("system %s: arc length for[0, %s] is %lf.", m_name.c_str(), argv[1], length);
			return TCL_OK;
		}
		else
		{
			animTcl::OutputMessage("Usage: getArcLength <t>");
			return TCL_ERROR;
		}

	}
	else if (strcmp(argv[0], "set") == 0)
	{
		if (argc == 6)
		{
			if (strcmp(argv[1], "tangent") == 0)
			{
				int index = atoi(argv[2]); //
				double x = atof(argv[3]);
				double y = atof(argv[4]);
				double z = atof(argv[5]);

				double * tangent = tangents.at(index);
				tangent[0] = x;
				tangent[1] = y;
				tangent[2] = z;

				// re init
				Clean();
				InitHermite();
				glutPostRedisplay();

				return TCL_OK;
			}
			else if (strcmp(argv[1], "point") == 0)
			{
				int index = atoi(argv[2]);
				double x = atof(argv[3]);
				double y = atof(argv[4]);
				double z = atof(argv[5]);

				double* point = points.at(index);
				point[0] = x;
				point[1] = y;
				point[2] = z;

				// re init
				Clean();
				InitHermite();	
				glutPostRedisplay();

				return TCL_OK;
			}
			else
			{
				animTcl::OutputMessage("Usage: set point <index> <x y z> or set tangent <index> <x y z>");
				return TCL_ERROR;
			}
		
		}
		else
		{
			animTcl::OutputMessage("Usage: set point <index> <x y z> or set tangent <index> <x y z>");
			return TCL_ERROR;
		}
	}
	else if (strcmp(argv[0], "add") == 0)
	{
		if (argc == 8)
		{
			if (strcmp(argv[1], "point") == 0)
			{
				double x = atof(argv[2]);
				double y = atof(argv[3]);
				double z = atof(argv[4]);

				double sx = atof(argv[5]);
				double sy = atof(argv[6]);
				double sz = atof(argv[7]);

				double *point = new double[3];
				double *tangent = new double[3];

				point[0] = x;
				point[1] = y;
				point[2] = z;

				tangent[0] = sx;
				tangent[1] = sy;
				tangent[2] = sz;

				points.push_back(point);
				tangents.push_back(tangent);

				// re init
				Clean();
				InitHermite();
				glutPostRedisplay();

				return TCL_OK;
			}
			else
			{
				animTcl::OutputMessage("Usage: add point <x y z sx sy sz>");
				return TCL_ERROR;
			}
		}
		else
		{
			animTcl::OutputMessage("Usage: add point <x y z sx sy sz>");
			return TCL_ERROR;
		}
	}

	return TCL_OK;
}

void HermiteSpline::LoadFile(char* fileName)
{
	//read file
	ifstream inFile(fileName);

	string temp;

	getline(inFile, temp, ' ');   //ignore name

	getline(inFile, temp, '\n');   //count
	int n = atoi(temp.c_str());
	if (n > MAX_POINTS + 1)
	{
		animTcl::OutputMessage("system %s: MaxPoints: %d, %d provided.", m_name.c_str(), MAX_POINTS, n);
		n = MAX_POINTS;
	}

	for (int i = 0; i < n; i++)
	{
		double* pPoint = new double[3];  // 

		getline(inFile, temp, ' ');   //P.x
		pPoint[0] = atof(temp.c_str());

		getline(inFile, temp, ' ');   //P.y
		pPoint[1] = atof(temp.c_str());

		getline(inFile, temp, ' ');   //P.z
		pPoint[2] = atof(temp.c_str());

		points.push_back(pPoint);

		double* pTangent = new double[3];  //

		getline(inFile, temp, ' ');   //T.x
		pTangent[0] = atof(temp.c_str());

		getline(inFile, temp, ' ');   //T.y
		pTangent[1] = atof(temp.c_str());

		getline(inFile, temp, '\n');   //T.z
		pTangent[2] = atof(temp.c_str());

		tangents.push_back(pTangent);

		animTcl::OutputMessage("Added point to hermite spline '%s' (%d/%d)\r\n", retrieveName().c_str(), (i + 1), MAX_POINTS);
		animTcl::OutputMessage("\tlocation: [%lf, %lf, %lf]\r\n", pPoint[0], pPoint[1], pPoint[2]);
		animTcl::OutputMessage("\ttangent:  [%lf, %lf, %lf]\r\n", pTangent[0], pTangent[1], pTangent[2]);
	}
}
 
int HermiteSpline::ExportFile(char* fileName)
{
	//write file
	ofstream outFile(fileName);

	string temp;

	if (!outFile.is_open()) {
		animTcl::OutputMessage("can't open %s.", fileName);
		return TCL_ERROR;
	}

	outFile << this->retrieveName() << " " << this->points.size() << endl;

	for (int i = 0; i < this->points.size(); i++)
	{
		double* p = points.at(i);
		double* t = tangents.at(i);
		outFile << p[0] << " " << p[1] << " " << p[2] << " "
			<< t[0] << " " << t[1] << " " << t[2] << endl;
	}

	//
	animTcl::OutputMessage("system %s: export %s.", m_name.c_str(), fileName);
	return TCL_OK;
}

// P(u) = UT MB
// UT =[u3 u2 u 1]
// M =[[ 2 -2 1 1]
//		[ -3 3 -2 -1]
//		[ 0 0 1 0]
//		[ 1 0 0 0]]
// B = [ Pi Pi+1 P'i P'i+1]T
void HermiteSpline::InitHermite()
{
	Clean();

	int n = points.size();
	for (int i = 0; i <  n - 1; i++)
	{
		CubicCurve_S* cc = CreateSegmentByTangent(points.at(i), points.at(i + 1), tangents.at(i), tangents.at(i + 1));
		AdaptiveGaussianIntegration(cc, 0, 1, 0.01);  //build (u, length) table
		 
		segments.push_back(cc);
	}
	
	UpdateTotalLength();
	DoSamplePoints();

}

void HermiteSpline::CatmullRomInitialization()
{
	Clean();

	//init the very beginning
	//P'(0) = 0.5(2P1 -P2 - P0)
	//P'(1) = 0.5(P(2) - P(0))

	double tangent0[3];
	double tangent1[3];

	tangent0[0] = 0.5 * (2 * points.at(1)[0] - points.at(2)[0] - points.at(0)[0]);
	tangent0[1] = 0.5 * (2 * points.at(1)[1] - points.at(2)[1] - points.at(0)[1]);
	tangent0[2] = 0;

	tangent1[0] = 0.5 * (points.at(2)[0] - points.at(0)[0]);
	tangent1[1] = 0.5 * (points.at(2)[1] - points.at(0)[1]);
	tangent1[2] = 0;

	CubicCurve_S* cc = CreateSegmentByTangent(points.at(0), points.at(1), tangent0, tangent1);

	AdaptiveGaussianIntegration(cc, 0, 1, 0.01);  // build (u, length) table
	segments.push_back(cc);

	//init the interior segments
	for (int i = 1; i < points.size()-2; i++)
	{
		CubicCurve_S* cc = new CubicCurve_S;

		double coeff[4];
		//memset
		double B[4];
		// x = x(u)
		B[0] = points.at(i - 1)[0];
		B[1] = points.at(i)[0];
		B[2] = points.at(i + 1)[0];
		B[3] = points.at(i + 2)[0];
		multArray(coeff, M_CR, B, 4, 4, 1);

		cc->ax = coeff[0];
		cc->bx = coeff[1];
		cc->cx = coeff[2];
		cc->dx = coeff[3];
		// y = y(u)
		B[0] = points.at(i - 1)[1];
		B[1] = points.at(i)[1];
		B[2] = points.at(i + 1)[1];
		B[3] = points.at(i + 2)[1];
		multArray(coeff, M_CR, B, 4, 4, 1);

		cc->ay = coeff[0];
		cc->by = coeff[1];
		cc->cy = coeff[2];
		cc->dy = coeff[3];

		AdaptiveGaussianIntegration(cc, 0, 1, 0.01);  //build (u, length) table
		segments.push_back(cc);
	}
	
	//init the very end.
	//P'(n - 2) = 0.5(P(n-1) - P(n-3))
	//P'(n - 1) = 0.5(P(n-1) + P(n-3) - 2P(n-2))

	int n = points.size();
	
	tangent0[0] = 0.5 * (points.at(n - 1)[0] - points.at(n - 3)[0]);
	tangent0[1] = 0.5 * (points.at(n - 1)[1] - points.at(n - 3)[1]);
	tangent0[2] = 0;

	tangent1[0] = 0.5 * (points.at(n - 1)[0] + points.at(n - 3)[0] - 2 * points.at(n - 2)[0]);
	tangent1[1] = 0.5 * (points.at(n - 1)[1] + points.at(n - 3)[1] - 2 * points.at(n - 2)[1]);
	tangent1[2] = 0;

	//
	cc = CreateSegmentByTangent(points.at(n - 2), points.at(n - 1), tangent0, tangent1);

	AdaptiveGaussianIntegration(cc, 0, 1, 0.01);  //build (u, length) table
	segments.push_back(cc);

	// compute total length
	UpdateTotalLength();
	DoSamplePoints();
}

CubicCurve_S* HermiteSpline::CreateSegmentByTangent(double* point0, double* point1, double* tangent0, double* tangent1)
{
	CubicCurve_S* cc = new CubicCurve_S;

	double coeff[4];
	//memset
	double B[4];

	// x = x(u)
	B[0] = point0[0];//points.at(i)[0];
	B[1] = point1[0];
	B[2] = tangent0[0];
	B[3] = tangent1[0];
	multArray(coeff, M, B, 4, 4, 1);

	cc->ax = coeff[0];
	cc->bx = coeff[1];
	cc->cx = coeff[2];
	cc->dx = coeff[3];

	// y = y(u)
	B[0] = point0[1];
	B[1] = point1[1];
	B[2] = tangent0[1];
	B[3] = tangent1[1];
	multArray(coeff, M, B, 4, 4, 1);

	cc->ay = coeff[0];
	cc->by = coeff[1];
	cc->cy = coeff[2];
	cc->dy = coeff[3];

	return cc;
}

double HermiteSpline::EvaluatePos(double u, double *C)
{
	double U[4];
	U[3] = 1;
	U[2] = u;
	U[1] = U[2] * u;
	U[0] = U[1] * u;

	double result = U[0] * C[0] + U[1] * C[1] + U[2] * C[2] + U[3] * C[3];
	return result;
}

double HermiteSpline::EvaluateTangent(double u, double* C)
{
	double U[4];
	U[3] = 0;
	U[2] = 1;
	U[1] = 2 * u;
	U[0] = 3 * u * u;

	double result = U[0] * C[0] + U[1] * C[1] + U[2] * C[2] + U[3] * C[3];
	return result;
}

double HermiteSpline::GetGlobalArcLength(double u)
{
	double length = 0;

	// find which segment is u in. 
	for (int i = 0; i < segments.size(); i++)
	{
		if (segments.at(i)->endPointU < u)
		{
			length += (*(segments.at(i)->u_length_table.end() - 1))->length;
		}
		else
		{
			// the current local segment¡£
			double lastU = 0;
			if (i > 0)
			{
				lastU = segments.at(i - 1)->endPointU;
			}

			double virtualU = (u - lastU) / (segments.at(i)->endPointU - lastU);  // the normalized local virtual u.


			length += GetSengmentLength(&segments.at(i)->u_length_table,virtualU);

			break;
		}
	}

	return length;
}

void HermiteSpline::UpdateTotalLength()
{
	double totalLength = 0;
	double globalU = 0;
	for (int i = 0; i < segments.size(); i++)
	{
		totalLength += (*(segments.at(i)->u_length_table.end() - 1))->length;
	}
	this->totalLength = totalLength;
	for (int i = 0; i < segments.size(); i++)
	{
		globalU += (*(segments.at(i)->u_length_table.end() - 1))->length / this->totalLength;
		segments.at(i)->endPointU = globalU;
	}
}

void HermiteSpline::DoSamplePoints()
{
	samplePoints.clear();  //ReleaseSamplePoints;

	for (int i = 0; i < segments.size(); i++)
	{
		for (double u = 0; u <= 1; u += 0.05)
		{
			double* p = new double[3]; //
			double C[4];
			C[0] = segments[i]->ax;
			C[1] = segments[i]->bx;
			C[2] = segments[i]->cx;
			C[3] = segments[i]->dx;
			
			p[0] = EvaluatePos(u, C);

			C[0] = segments[i]->ay;
			C[1] = segments[i]->by;
			C[2] = segments[i]->cy;
			C[3] = segments[i]->dy;
			p[1] = EvaluatePos(u, C);

			p[2] = 0;
 
			samplePoints.push_back(p);
		}

	}
	//work around, add the last one.
	double* p = new double[3]; 
	VecCopy(p, *(points.end() - 1));
	samplePoints.push_back(p);
}


double GetSengmentLength(std::vector<u_length_entry_S*>* entries, double u)
{
	double lastU = 0;
	double lastLen = 0;
	double totalLen = 0;
	for (int i = 0; i < entries->size(); i++)
	{
		if (entries->at(i)->u < u)
		{
			lastU = entries->at(i)->u;
			lastLen = entries->at(i)->length;
		}
		else
		{
			totalLen = lastLen + (u - lastU) / (entries->at(i)->u - lastU) * (entries->at(i)->length - lastLen);
			break;
		}
	}

	return totalLen;

}

// 
void AdaptiveGaussianIntegration(CubicCurve_S* curve, double u1, double u2,double tolerance)
{
	Polynomial_S poly_func;
	interval_length_entry_S interval;

	poly_func.degree = 4;
	poly_func.coeff = new double[5];
	poly_func.coeff[0] = 9 * (curve->ax * curve->ax + curve->ay * curve->ay);	//A
	poly_func.coeff[1] = 12 * (curve->ax * curve->bx + curve->ay * curve->by);	//B
	poly_func.coeff[2] = 6 * (curve->ax * curve->cx + curve->ay * curve->cy) + 
		4 * (curve->bx * curve->bx + curve->by * curve->by);					//C
	poly_func.coeff[3] = 4 * (curve->bx * curve->cx + curve->by * curve->cy);	//D
	poly_func.coeff[4] = curve->cx * curve->cx + curve->cy * curve->cy;			//E

	interval.u1 = u1;
	interval.u2 = u2;

	AddTableEntry(0, 0, &(curve->u_length_table));  // 
	
	interval.length = IntegrateFuncOnInterval(&poly_func, &interval);

	// discard return value;
	Subdivide(&interval, &poly_func, 0.0, tolerance, &(curve->u_length_table));

	return;
}

double Subdivide(interval_length_entry_S* fullInterval, Polynomial_S* func,
	double totalLength, double tolerance, std::vector<u_length_entry_S*>* entries)
{
	
	double mid_u = (fullInterval->u1 + fullInterval->u2) / 2;

	interval_length_entry_S leftInterval;
	leftInterval.u1 = fullInterval->u1; 
	leftInterval.u2 = mid_u;

	interval_length_entry_S right_interval;
	right_interval.u1 = mid_u; 
	right_interval.u2 = fullInterval->u2;

	double left_length = IntegrateFuncOnInterval(func, &leftInterval);
	double right_length = IntegrateFuncOnInterval(func, &right_interval);

	double delta = fabs(fullInterval->length - (left_length + right_length));
	if (delta > tolerance) {
		leftInterval.length = left_length;
		right_interval.length = right_length;
		totalLength = Subdivide(&leftInterval, func, totalLength, tolerance, entries);
		totalLength = Subdivide(&right_interval, func, totalLength, tolerance, entries);
		return(totalLength);
	}
	else {
		totalLength = totalLength + left_length;
		AddTableEntry(mid_u, totalLength, entries);
		totalLength = totalLength + right_length;
		AddTableEntry(fullInterval->u2, totalLength, entries);
		return(totalLength);
	}
}

void AddTableEntry(double u, double length, std::vector<u_length_entry_S*> *entries)
{
	/* add entry of (u, length) */
	u_length_entry_S* p = new u_length_entry_S;
	p->u = u;
	p->length = length;
	entries->push_back(p);

}

// 
double IntegrateFuncOnInterval(Polynomial_S* func, interval_length_entry_S* interval)
{
	double x[5] = { .1488743389,.4333953941,.6794095682,.8650633666, .9739065285 };
	double w[5] = { .2966242247,.2692667193,.2190863625,.1494513491,.0666713443 };
	 
	double u1 = interval->u1;
	double u2 = interval->u2;
	double mid_u = (u1 + u2) / 2.0;
	double diff = (u2 - u1) / 2.0;
	double length = 0.0;

	for (int i = 0; i < 5; i++) {
		double dx = diff * x[i];
		length += w[i] * (sqrt(EvaluatePoly(func, mid_u + dx)) + sqrt(EvaluatePoly(func, mid_u - dx)));
	}
	length *= diff;
	return (length);
}

double EvaluatePoly(Polynomial_S* poly, double u)
{
	double item = 1.0;	
	double result = 0.0; 

	for (int i = poly->degree; i >= 0; i--) {
		result += poly->coeff[i] * item;
		item *= u;
	}

	return result;
}
