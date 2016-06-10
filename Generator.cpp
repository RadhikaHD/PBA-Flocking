#include "Generator.h"

#include <vector>

Generator::Generator(const Vector3d &origin, const int noOfParticles, const double timestep)
{
    setGeneratorp(origin);
    setGeneratorSize(noOfParticles);
    setGeneratorts(timestep);
    
}

Generator::~Generator()
{
    
}

void Generator::setGeneratorp(const Vector3d &p)
{
    Generator::Origin = p;
   
}

void Generator::setGeneratorSize(const int size)
{
    Generator::noOfParticles = size;
    
}

void Generator::setGeneratorts(const double timestep)
{
    timeStep = timestep;
    
}







