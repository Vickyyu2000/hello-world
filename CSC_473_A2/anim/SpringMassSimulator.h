#pragma once

#include "BaseSystem.h"
#include "BaseSimulator.h"
#include <array>
#include <vector>
#include <unordered_set>

class SpringMassSimulator : public BaseSimulator
{
  struct Spring
  {
    size_t i, j;
    double ks;
    double kd;
    double restLength;
  };

  enum IntegrationMethod
  {
    EULER, SYMPLECTIC, VERLET
  };

public:
  SpringMassSimulator(const std::string &);
  ~SpringMassSimulator() override;

  int init(double) override;
  int step(double) override;

  void reset(double) override;
  int command(int, myCONST_SPEC char **) override;
  void display(GLenum mode = GL_RENDER) override;

  void eulerMethod(double);
  void symplecticMethod(double);
  void verletMethod(double);

  void calculateSpringForces();
  void calculateParticleAcc(size_t, Vector);

private:
  // size_t m_numSprings;
  BaseSystem *m_particleSystem;
  size_t m_numParticles;
  double *m_prevState;
  double *m_currState;
  double *m_nextState;
  std::vector<std::vector<size_t>> m_particleSprings;
  std::vector<Spring> m_springs;
  std::vector<std::array<double, 3>> m_forces;
  std::unordered_set<size_t> m_fixedParticles;
  IntegrationMethod m_integrationMethod;
  double m_timeStep;
  double m_simulationTime;
  double m_groundStiffness;
  double m_groundDamping;
  double m_gravity;
  double m_drag;
};