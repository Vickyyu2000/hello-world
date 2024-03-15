#ifndef MY_MODEL_H
#define MY_MODEL_H

#include "BaseSystem.h"
#include <shared/defs.h>
#include <util/util.h>
#include "animTcl.h"
#include <GLmodel/GLmodel.h>

#include "shared/opengl.h"

typedef struct state {
	Vector pos;
	Vector tangent;
}state_s;

//
class MyModel : public BaseSystem
{ 

public:
	MyModel( const std::string& name );
	virtual void getState( double *p );
	virtual void setState( double *p );
	void reset( double time );

	void display( GLenum mode = GL_RENDER );

	void readModel(char *fname) { m_model.ReadOBJ(fname); }
	void flipNormals(void) { glmReverseWinding(&m_model); }
	int command(int argc, myCONST_SPEC char **argv) ;

protected:

	float m_sx;
	float m_sy;
	float m_sz;

	state_s m_state;

	GLMmodel m_model;

} ;
#endif
