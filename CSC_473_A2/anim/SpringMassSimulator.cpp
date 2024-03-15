#include "ParticleSystem.h"
#include "SpringMassSimulator.h"
#include "GlobalResourceManager.h"

SpringMassSimulator::SpringMassSimulator(const std::string &name)
    : BaseSimulator(name), m_particleSystem(nullptr), m_numParticles(0), //
      m_prevState(nullptr), m_currState(nullptr), m_nextState(nullptr),  //
      m_particleSprings(), m_springs(), m_forces(), m_fixedParticles(), m_integrationMethod(IntegrationMethod::EULER),
      m_timeStep(0.0001), m_simulationTime(0), m_gravity(0.0), m_drag(0.0), m_groundStiffness(0.0),
      m_groundDamping(0.0)
{
}

SpringMassSimulator::~SpringMassSimulator()
{
  if (m_prevState != nullptr)
  {
    delete[] m_prevState;
  }
  if (m_currState != nullptr)
  {
    delete[] m_currState;
  }
  if (m_nextState != nullptr)
  {
    delete[] m_nextState;
  }
}

int SpringMassSimulator::init(double time)
{
  m_particleSystem->getState(m_currState);
  m_particleSystem->getState(m_nextState);
  return 0;
}

int SpringMassSimulator::step(double time)
{
  double elapsedTime = time - m_simulationTime;
  int n = elapsedTime / m_timeStep;
  double remainedTime = elapsedTime - n * m_timeStep;
  // substep
  while (n-- > 0)
  {
    switch (m_integrationMethod)
    {
    case SpringMassSimulator::EULER:
      eulerMethod(m_timeStep);
      break;
    case SpringMassSimulator::SYMPLECTIC:
      symplecticMethod(m_timeStep);
      break;
    case SpringMassSimulator::VERLET:
      verletMethod(m_timeStep);
      break;
    }
  }
  if (remainedTime > 1e-8)
  {
    switch (m_integrationMethod)
    {
    case SpringMassSimulator::EULER:
      eulerMethod(remainedTime);
      break;
    case SpringMassSimulator::SYMPLECTIC:
      symplecticMethod(remainedTime);
      break;
    case SpringMassSimulator::VERLET:
      verletMethod(remainedTime);
      break;
    }
  }
  m_simulationTime = time;
  return 0;
}

void SpringMassSimulator::reset(double time)
{
}

int SpringMassSimulator::command(int argc, myCONST_SPEC char **argv)
{
  if (argc < 1)
  {
    animTcl::OutputMessage("simulator %s: wrong number of params.", m_name.c_str());
    return TCL_ERROR;
  }
  else if (strcmp(argv[0], "link") == 0)
  {
    if (argc == 3)
    {
      // TODO: how to get the particle nums without dynamic_cast?
      m_particleSystem = GlobalResourceManager::use()->getSystem(argv[1]);
      if (m_particleSystem == nullptr)
      {
        animTcl::OutputMessage("simulator %s: system %s does not exist.", m_name.c_str(), argv[1]);
        return TCL_ERROR;
      }
      auto partSys = dynamic_cast<ParticleSystem *>(m_particleSystem);
      if (partSys == nullptr)
      {
        animTcl::OutputMessage("system %s id not a valid spring mass system.", argv[1]);
        return TCL_ERROR;
      }
      m_numParticles = partSys->getNumParticles();
      m_particleSprings.resize(m_numParticles);
      m_currState = new double[7 * m_numParticles]();
      m_nextState = new double[7 * m_numParticles]();
      int n = atoi(argv[2]);
      if (n < 0)
      {
        animTcl::OutputMessage("Number of springs must be non-negative.");
        return TCL_ERROR;
      }
      // m_numSprings = n;
      m_springs.reserve(n);
      m_forces.reserve(n);
    }
    else
    {
      animTcl::OutputMessage("Usage: link <sys name> <Number of Springs>");
      return TCL_ERROR;
    }
  }
  else if (strcmp(argv[0], "spring") == 0)
  {
    if (argc == 6)
    {
      size_t i = atoi(argv[1]);
      size_t j = atoi(argv[2]);
      double ks = atof(argv[3]);
      double kd = atof(argv[4]);
      double restlength = atof(argv[5]);
      size_t idx = m_springs.size();
      m_particleSprings[i].push_back(idx);
      m_particleSprings[j].push_back(idx);
      m_springs.push_back(Spring{i, j, ks, kd, restlength});
      m_forces.emplace_back(std::array<double, 3>{0.0, 0.0, 0.0});
    }
    else
    {
      animTcl::OutputMessage("Usage: spring <index1> <index2> <ks> <kd> <restlength>");
      return TCL_ERROR;
    }
  }
  else if (strcmp(argv[0], "fix") == 0)
  {
    if (argc == 2)
    {
      size_t i = atoi(argv[1]);
      m_fixedParticles.insert(i);
    }
    else
    {
      animTcl::OutputMessage("Usage: fix <index>");
      return TCL_ERROR;
    }
  }
  else if (strcmp(argv[0], "integration") == 0)
  {
    if (argc == 3)
    {
      if (strcmp(argv[1], "euler") == 0)
      {
        m_integrationMethod = IntegrationMethod::EULER;
      }
      else if (strcmp(argv[1], "symplectic") == 0)
      {
        m_integrationMethod = IntegrationMethod::SYMPLECTIC;
      }
      else if (strcmp(argv[1], "verlet") == 0)
      {
        m_integrationMethod = IntegrationMethod::VERLET;
      }
      else
      {
        animTcl::OutputMessage("Usage: integration <euler|symplectic|verlet> <time step>");
        return TCL_ERROR;
      }
      m_timeStep = atof(argv[2]);
    }
    else
    {
      animTcl::OutputMessage("Usage: integration <euler|symplectic|verlet> <time step>");
      return TCL_ERROR;
    }
  }
  else if (strcmp(argv[0], "ground") == 0)
  {
    if (argc == 3)
    {
      m_groundStiffness = atof(argv[1]);
      m_groundDamping = atof(argv[2]);
    }
    else
    {
      animTcl::OutputMessage("Usage: ground <ks> <kd>");
      return TCL_ERROR;
    }
  }
  else if (strcmp(argv[0], "gravity") == 0)
  {
    if (argc == 2)
    {
      m_gravity = atof(argv[1]);
    }
    else
    {
      animTcl::OutputMessage("Usage: gravity <g>");
      return TCL_ERROR;
    }
  }
  else if (strcmp(argv[0], "drag") == 0)
  {
    if (argc == 2)
    {
      m_drag = atof(argv[1]);
    }
    else
    {
      animTcl::OutputMessage("Usage: drag <kdrag>");
      return TCL_ERROR;
    }
  }
  glutPostRedisplay();
  return TCL_OK;
}

void SpringMassSimulator::display(GLenum mode)
{
  glColor4f(0.3, 0.3, 0.3, 1.0);
  glLineWidth(1.0);

  glBegin(GL_LINES);
  for (auto &spring : m_springs)
  {
    size_t i = 7 * spring.i;
    size_t j = 7 * spring.j;
    glVertex3d(m_currState[i + 1], m_currState[i + 2], m_currState[i + 3]);
    glVertex3d(m_currState[j + 1], m_currState[j + 2], m_currState[j + 3]);
  }
  glEnd();
}

void SpringMassSimulator::eulerMethod(double dt)
{
  calculateSpringForces();
  if (m_prevState == nullptr)
  {
    m_prevState = new double[7 * m_numParticles]();
    // m_particleSystem->getState(m_prevState);
    // deep copy mass
    for (int i{0}; i < 7 * m_numParticles; i += 7)
    {
      m_prevState[i] = m_currState[i];
    }
  }
  for (size_t i = 0; i < m_numParticles; ++i)
  {
    size_t idx = 7 * i;
    if (m_fixedParticles.count(i))
    {
      VecCopy(&m_nextState[idx + 1], &m_currState[idx + 1]);
      zeroVector(&m_nextState[idx + 4]);
      //m_currState[idx + 4] = 0.0;
      //m_currState[idx + 5] = 0.0;
      //m_currState[idx + 6] = 0.0;
    }
    else
    {
      Vector v{};
      VecCopy(v, &m_currState[idx + 4]);
      Vector a{0.0, 0.0, 0.0};
      calculateParticleAcc(i, a);
      Vector dv{0.0, 0.0, 0.0};
      VecNumMul(dv, a, dt);
      VecAdd(&m_nextState[idx + 4], v, dv);

      Vector x{};
      VecCopy(x, &m_currState[idx + 1]);
      Vector dx{0.0, 0.0, 0.0};
      VecNumMul(dx, v, dt);
      VecAdd(&m_nextState[idx + 1], x, dx);
      //m_currState[idx + 1] = x[0] + v[0] * dt;
      //m_currState[idx + 2] = x[1] + v[1] * dt;
      //m_currState[idx + 3] = x[2] + v[2] * dt;
      //m_currState[idx + 4] = v[0] + a[0] * dt;
      //m_currState[idx + 5] = v[1] + a[1] * dt;
      //m_currState[idx + 6] = v[2] + a[2] * dt;
    }
  }
  m_particleSystem->setState(m_nextState);
  // shallow swap rather than deep copy
  double* state = m_prevState;
  m_prevState = m_currState;
  m_currState = m_nextState;
  m_nextState = state;
}

void SpringMassSimulator::symplecticMethod(double dt)
{
  calculateSpringForces();
  if (m_prevState == nullptr)
  {
    m_prevState = new double[7 * m_numParticles]();
    // m_particleSystem->getState(m_prevState);
    // deep copy mass
    for (int i{0}; i < 7 * m_numParticles; i += 7)
    {
      m_prevState[i] = m_currState[i];
    }
  }
  for (size_t i = 0; i < m_numParticles; ++i)
  {
    size_t idx = 7 * i;
    if (m_fixedParticles.count(i))
    {
      VecCopy(&m_nextState[idx + 1], &m_currState[idx + 1]);
      zeroVector(&m_nextState[idx + 4]);
    }
    else
    {
      Vector v{};
      VecCopy(v, &m_currState[idx + 4]);
      Vector a{0.0, 0.0, 0.0};
      calculateParticleAcc(i, a);
      Vector dv{0.0, 0.0, 0.0};
      VecNumMul(dv, a, dt);
      VecAdd(&m_nextState[idx + 4], v, dv);
      //m_currState[idx + 4] = v[0] + dv[0];
      //m_currState[idx + 5] = v[1] + dv[1];
      //m_currState[idx + 6] = v[2] + dv[2];

      Vector x{};
      VecCopy(x, &m_currState[idx + 1]);
      VecAdd(v, v, dv);
      Vector dx{0.0, 0.0, 0.0};
      VecNumMul(dx, v, dt);
      VecAdd(&m_nextState[idx + 1], x, dx);
      //m_currState[idx + 1] = x[0] + v_new[0] * dt;
      //m_currState[idx + 2] = x[1] + v_new[1] * dt;
      //m_currState[idx + 3] = x[2] + v_new[2] * dt;
    }
  }
  m_particleSystem->setState(m_nextState);
  // shallow swap rather than deep copy
  double *state = m_prevState;
  m_prevState = m_currState;
  m_currState = m_nextState;
  m_nextState = state;
}

void SpringMassSimulator::verletMethod(double dt)
{
  calculateSpringForces();
  if (m_prevState == nullptr)
  {
    m_prevState = new double[7 * m_numParticles]();
    for (int i{0}; i < 7 * m_numParticles; i += 7)
    {
      m_prevState[i] = m_currState[i];
      Vector dx{};
      VecNumMul(dx, &m_currState[i + 4], dt);
      VecSubtract(&m_prevState[i + 1], &m_currState[i + 1], dx);
      VecCopy(&m_prevState[i + 4], &m_currState[i + 4]);
    }
  }

  for (size_t i = 0; i < m_numParticles; ++i)
  {
    size_t idx = 7 * i;
    if (m_fixedParticles.count(i))
    {
      VecCopy(&m_nextState[idx + 1], &m_currState[idx + 1]);
      zeroVector(&m_nextState[idx + 4]);
    }
    else
    {
      Vector x{};
      VecCopy(x, &m_currState[idx + 1]);
      VecScale(x, 2.0);
      VecSubtract(x, x, &m_prevState[idx + 1]);
      Vector a{0.0, 0.0, 0.0};
      calculateParticleAcc(i, a);
      Vector dx{};
      VecNumMul(dx, a, dt * dt);
      VecAdd(&m_nextState[idx + 1], x, dx);

      Vector v{};
      VecCopy(v, &m_currState[idx + 4]);
      Vector dv{0.0, 0.0, 0.0};
      VecNumMul(dv, a, dt);
      VecAdd(&m_nextState[idx + 4], v, dv);
    }
  }
  
  m_particleSystem->setState(m_nextState);
  // shallow swap rather than deep copy
  double *state = m_prevState;
  m_prevState = m_currState;
  m_currState = m_nextState;
  m_nextState = state;
}

void SpringMassSimulator::calculateSpringForces()
{
  for (int k{0}; k < m_springs.size(); ++k)
  {
    const Spring &spring = m_springs[k];
    size_t i = 7 * spring.i;
    double mi{m_currState[i]};
    Vector xi{};
    VecCopy(xi, &m_currState[i + 1]);
    Vector vi{};
    VecCopy(vi, &m_currState[i + 4]);
    size_t j = 7 * spring.j;
    double mj{m_currState[j]};
    Vector xj{};
    VecCopy(xj, &m_currState[j + 1]);
    Vector vj{};
    VecCopy(vj, &m_currState[j + 4]);

    Vector dx{};
    VecSubtract(dx, xi, xj);
    double len = VecLength(dx);
    if (len < 1e-8)
    {
      VecScale(dx, 1e3);
    }
    len = VecLength(dx);
    VecScale(dx, 1.0 / len);

    Vector dv{0.0, 0.0, 0.0};
    VecSubtract(dv, vi, vj);

    Vector stiffnessForce{0.0, 0.0, 0.0};
    VecNumMul(stiffnessForce, dx, spring.ks * (spring.restLength - len));

    Vector damplingForce{0.0, 0.0, 0.0};
    VecNumMul(damplingForce, dx, -spring.kd * VecDotProd(dv, dx));

    VecAdd(m_forces[k].data(), stiffnessForce, damplingForce);
  }
}

void SpringMassSimulator::calculateParticleAcc(size_t i, Vector acc)
{
  Vector force{0.0, 0.0, 0.0};
  // spring force
  for (auto k : m_particleSprings[i])
  {
    const auto &spring = m_springs[k];
    if (spring.i == i)
    {
      VecAdd(force, force, m_forces[k].data());
    }
    else
    {
      VecSubtract(force, force, m_forces[k].data());
    }
  }

  size_t idx = 7 * i;

  // drag force
  Vector v{};
  VecCopy(v, &m_currState[idx + 4]);
  Vector dragForce{};
  VecNumMul(dragForce, v, -m_drag);
  VecAdd(force, force, dragForce);

  // ground penalty force
  Vector x_curr{};
  VecCopy(x_curr, &m_currState[idx + 1]);
  if (x_curr[1] < 0.0)
  {
    bool flag = false;
    Vector normal{0.0, 1.0, 0.0};
    
    if (m_prevState != nullptr)
    {
      // with friction, get the intersect point
      Vector x_prev{};
      VecCopy(x_prev, &m_prevState[idx + 1]);
      double dy = x_curr[1] - x_prev[1];
      double t = -x_prev[1] / dy;

      Vector x_intersect{};
      if (x_prev[1] > 0)
      {
        VecInter(x_intersect, x_prev, x_curr, t);
      }
      else
      {
        VecCopy(x_intersect, x_prev);
        x_intersect[1] = 0;
      }
      

      Vector dx{};
      VecSubtract(dx, x_curr, x_intersect);
      double len = VecLength(dx);

      Vector stiffnessForce{};
      VecNumMul(stiffnessForce, dx, -m_groundStiffness);
      Vector fsp{};
      VecCopy(fsp, stiffnessForce);

      // normalize dx
      if (len < 1e-8)
      {
        VecScale(dx, 1e3);
      }
      len = VecLength(dx);
      VecScale(dx, 1.0 / len);

      Vector dampingForce{0.0, 0.0, 0.0};
      VecNumMul(dampingForce, dx, -m_groundDamping * VecDotProd(v, dx));
      VecAdd(fsp, fsp, dampingForce);
      Vector fn{};
      VecNumMul(fn, normal, VecDotProd(fsp, normal));
      Vector ftan{};
      VecSubtract(ftan, fsp, fn);

      // TODO: Coulomb Friction Force Cone

      if (fn[1] > 0)
        VecAdd(force, force, fn);
    }
    else
    {
      // without friction or initial state
      Vector stiffnessForce{};
      VecNumMul(stiffnessForce, normal, -m_groundStiffness * x_curr[1]);
      Vector fn{};
      VecCopy(fn, stiffnessForce);

      Vector dampingForce{};
      VecNumMul(dampingForce, normal, -m_groundDamping * VecDotProd(v, normal));
      VecAdd(fn, fn, dampingForce);

      if (fn[1] > 0)
        VecAdd(force, force, fn);
    }
  }

  VecNumMul(acc, force, 1.0 / m_currState[7 * i]);
  // gravity force, only when the particle is in the air
  if (x_curr[1] > 0)
    acc[1] += m_gravity;
}
