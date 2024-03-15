#include "PathSimulator.h"
#include "HermiteSpline.h"
#include "ResourceManager.h"

PathSimulator::PathSimulator(const std::string& name, BaseSystem* target, BaseSystem* path) :
	BaseSimulator(name),
	m_object(target),
	m_path(path),
	m_acceleration(0),
	m_velocity(0),
	m_distance(0),
	last_time(0),
	next_log_time(0)
{

}

PathSimulator::~PathSimulator()
{
}


int PathSimulator::step(double time)
{
	//static double last_time = 0;
	
	if (m_distance > 1)
	{
#if 1
		static boolean last_log = FALSE;
		
		if (!last_log)
		{
			animTcl::OutputMessage("Simulation time: %lf, Tank speed: %lf", time, m_velocity);
			last_log = TRUE;
		}
#endif
		return 0;
	}

	if (time > next_log_time)
	{
		animTcl::OutputMessage("Simulation time: %lf, Tank speed: %lf", time, m_velocity);
		next_log_time++;
	}

	double state[6];
	
	double delta_time = time - last_time;
	if (delta_time < 0) // check pause.
	{		
		//animTcl::OutputMessage(" PathSimulator::step -- delta_time < 0. time: %lf, last_time: %lf", time, last_time);
		delta_time = SIMULATION_TIME_STEP;
	}


	if(m_distance <= 1)
	{
		m_distance += m_velocity * delta_time;
		m_velocity += m_acceleration * delta_time;
	}
 

	//update m_acceleration
	if (m_distance >= 1)
	{
		m_acceleration = 0;
	}
	else if (m_distance > 0.9)
	{
		m_acceleration = -ACCELERATION;
	}
	else if (m_distance > 0.1)
	{
		m_acceleration = 0;
	}
	else
	{				
		m_acceleration = ACCELERATION;
	}
 
	((HermiteSpline*)m_path)->GetPositionTangent(m_distance, state, state + 3);

	m_object->setState(state);

	last_time = time;

	return 0;
}

void PathSimulator::reset(double time)
{
	m_acceleration = 0,
	m_velocity = 0,
	m_distance = 0;
	last_time = 0;
	next_log_time = 0;
}