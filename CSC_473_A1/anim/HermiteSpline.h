#pragma once
#include <vector>
#include "BaseSystem.h"

//structures for Adaptive Gaussian Integration
typedef struct {
	double u, length;
} u_length_entry_S;

typedef struct {
	double u1, u2;
	double length;
} interval_length_entry_S;

//2D cubic curve, 
typedef struct {
	double ax, bx, cx, dx;  // for x(u) 
	double ay, by, cy, dy;  // for y(u) 

	std::vector<u_length_entry_S*> u_length_table;  
	double endPointU; //u value of the piece end point in the global curve¡£
} CubicCurve_S;

//  
typedef struct {
	double *coeff;
	int degree; // length of the array.
} Polynomial_S;
//
void AdaptiveGaussianIntegration(CubicCurve_S* curve, double u1, double u2,
	double tolerance);
double Subdivide(interval_length_entry_S* full_interval, Polynomial_S* func,
	double total_length, double tolerance, std::vector<u_length_entry_S*>* entries);
void AddTableEntry(double u, double length, std::vector<u_length_entry_S*> *entries);
double IntegrateFuncOnInterval(Polynomial_S* func, interval_length_entry_S* interval);
double EvaluatePoly(Polynomial_S* poly, double u);

// Get the length for [0, u] in a local segment. 
double GetSengmentLength(std::vector<u_length_entry_S*>* entries, double u);
 

class HermiteSpline :
    public BaseSystem
{
public:
	HermiteSpline(const std::string& name);
	~HermiteSpline();

	virtual void getState(double* p);
	virtual void setState(double* p);
	void reset(double time);

	void display(GLenum mode = GL_RENDER);

	int command(int argc, myCONST_SPEC char** argv);

	void GetPositionTangent(double u, double* pos, double* tangent);

private:
	void LoadFile(char *fileName);
	int ExportFile(char* fileName);
	void InitHermite(); //

	//Evaluate x(u) or y(u) at u.
	double EvaluatePos(double u, double *C);
	//Evaluate derivative : x'(u) or y'(u) for x(u) or y(u) at u.
	double EvaluateTangent(double u, double* C);

	double GetGlobalArcLength(double u);

	void UpdateTotalLength();

	void CatmullRomInitialization();
	
	// for display
	void DoSamplePoints();

	// Clean things except the points and tangents loaded from file.
	void Clean();

	void ClearAll();
	
	//standard init process by definition.
	CubicCurve_S* CreateSegmentByTangent(double *point0, double* point1, double *tangent0, double *tangent1);
protected:

	double M[16];		// Hermite Matrix
	double M_CR[16];	// Catmull-Rom Matrix 

#define MAX_POINTS 40

	std::vector< double*> points;	//n
	std::vector< double*> tangents;	//n
	std::vector< CubicCurve_S*> segments;  //n-1
	
	std::vector< double*> samplePoints;	// for display.

	double totalLength; // changed by updateTotalLength() 
};

