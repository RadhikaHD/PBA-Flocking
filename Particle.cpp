

#include "Particle.h"

Particle::Particle()
{
    
}

Particle::Particle( const Vector3d &p, const Vector3d &v, const Vector3d &a,  const double mass)
{
    setParticlev(v);
    setParticlep(p);
    setParticlea(a);
    setParticlemass(mass);
}

Particle::~Particle()
{
    
}

void Particle::setParticlev(const Vector3d &v)
{
    Velocity = v;
}

void Particle::setParticlep(const Vector3d &p)
{
    Position = p;
}

void Particle::setParticlea(const Vector3d &a)
{
    Acceleration = a;
}

void Particle::setParticlemass(const double mass)
{
    Particle::mass  = mass;
}






