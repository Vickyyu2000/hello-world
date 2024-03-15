#pragma once
#include "BaseSimulator.h"
#include "BaseSystem.h"
 

class PathSimulator :
    public BaseSimulator
{
#define ACCELERATION 0.01

public:

	PathSimulator(const std::string& name, BaseSystem* target, BaseSystem* path);
	~PathSimulator();

	int step(double time);
	int init(double time)
	{		 
		//m_object->getState(m_pos0);
		//m_acceleration  = 0;
		//m_velocity = 0;
		//m_distance = 0;
		last_time = 0;
		next_log_time = 0;
		return 0;
	};

	int command(int argc, myCONST_SPEC char** argv) { return TCL_OK; }
	void reset(double time);
protected:

	//Vector m_pos0; // initial position

	//Vector m_pos;

	BaseSystem* m_object;
	BaseSystem* m_path;

	double m_acceleration;
	double m_velocity;
	double m_distance;

	double last_time;

	int next_log_time;
};

