

#include "Particle.h"
#include <vector>

class Generator
{
    
public:
    
    int noOfParticles;
    double timeStep;
    Vector3d Origin;
    std::vector<Particle> particles;
    
    Generator(const Vector3d &origin, const int noOfParticles, const double timestep);
    ~Generator();
    
    void setGeneratorp(const Vector3d &p);
    void setGeneratorSize(const int size);
    void setGeneratorts(const double timestep);

    
};
