#pragma once

#include "BaseSystem.h"

class ParticleSystem : public BaseSystem
{
public:
  ParticleSystem(const std::string &);
  ~ParticleSystem() override;

  void getState(double *) override;
  void setState(double *) override;

  int command(int, myCONST_SPEC char **) override;
  void display(GLenum mode = GL_RENDER) override;

  size_t getNumParticles() const
  {
    return m_dim;
  }

private:
  size_t m_dim;
  double *m_state;
};