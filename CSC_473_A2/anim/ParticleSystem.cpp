#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(const std::string &name) : BaseSystem(name), m_dim(0), m_state(NULL)
{
}

ParticleSystem::~ParticleSystem()
{
  if (m_state)
  {
    delete[] m_state;
  }
}

void ParticleSystem::getState(double *p)
{
  for (int i = 0; i < 7 * m_dim; i++)
  {
    p[i] = m_state[i];
  }
}

void ParticleSystem::setState(double *p)
{
  for (int i = 0; i < 7 * m_dim; i++)
  {
    m_state[i] = p[i];
  }
}

int ParticleSystem::command(int argc, myCONST_SPEC char **argv)
{
  if (argc < 1)
  {
    animTcl::OutputMessage("system %s: wrong number of params.", m_name.c_str());
    return TCL_ERROR;
  }
  else if (strcmp(argv[0], "dim") == 0)
  {
    if (argc == 2)
    {
      int n = atoi(argv[1]);
      if (n < 0)
      {
        animTcl::OutputMessage("Number of particles must be non-negative.");
        return TCL_ERROR;
      }
      m_dim = n;
      m_state = new double[7 * m_dim]();
      return TCL_OK;
    }
    else
    {
      animTcl::OutputMessage("Usage: dim <Number of Particles>");
      return TCL_ERROR;
    }
  }
  else if (strcmp(argv[0], "particle") == 0)
  {
    if (argc == 9)
    {
      int n = atoi(argv[1]);
      if (n < 0 || n >= m_dim)
      {
        animTcl::OutputMessage("Invalid index, the index of particle should less than %d", m_dim);
        return TCL_ERROR;
      }
      size_t idx = 7 * n;
      // mass
      m_state[idx] = atof(argv[2]);
      // x y z
      m_state[idx + 1] = atof(argv[3]);
      m_state[idx + 2] = atof(argv[4]);
      m_state[idx + 3] = atof(argv[5]);
      // vx vy vz
      m_state[idx + 4] = atof(argv[6]);
      m_state[idx + 5] = atof(argv[7]);
      m_state[idx + 6] = atof(argv[8]);
    }
    else
    {
      animTcl::OutputMessage("Usage: particle <index> <mass> <x y z vx vy vz>");
      return TCL_ERROR;
    }
  }
  else if (strcmp(argv[0], "all_velocities") == 0)
  {
    if (argc == 4)
    {
      double vx = atof(argv[1]);
      double vy = atof(argv[2]);
      double vz = atof(argv[3]);
      for (int idx = 0; idx < m_dim; idx += 7)
      {
        m_state[idx + 4] = vx;
        m_state[idx + 5] = vy;
        m_state[idx + 6] = vz;
      }
    }
    else
    {
      animTcl::OutputMessage("Usage: all_velocities <vx vy vz>");
      return TCL_ERROR;
    }
  }

  glutPostRedisplay();
  return TCL_OK;
}

void ParticleSystem::display(GLenum mode)
{
  glColor4f(0.0, 0.5, 1.0, 1.0);
  glPointSize(2.0);
  glBegin(GL_POINTS);
  for (int i = 0; i < m_dim; i++)
  {
    glVertex3d(m_state[7 * i + 1], m_state[7 * i + 2], m_state[7 * i + 3]);
  }
  glEnd();
}
