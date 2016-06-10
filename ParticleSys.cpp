#include "Generator.h"
#include <vector>

#define WINDOW_WIDTH    1080
#define WINDOW_HEIGHT   1080
#define STARTX 0.5
#define STARTY 1.0
#define STARTZ 0.5
#define RADIUS 0.03
#define FLOORHEIGHT 0
#define BOXLEFT 0
#define BOXRIGHT 1
#define BOXHEIGHT 1
#define BOXFRONT 1
#define BOXBACK 0
#define EPS 0.08

static double WinWidth = WINDOW_WIDTH;
static double WinHeight = WINDOW_HEIGHT;
static Vector3d origin(STARTX, STARTY, STARTZ);
static Vector3d V0(0.1,0.1,0.1);
static double CoeffofRestitution = 0.8;
const double timestep = 0.05;
const static int noOfParticles = 10;
const double mass = 0.01;
const double kv = 0.001;
const double ka = 0.001;
const double kc = 0.03;
static double cameraX=0.5, cameraY=1.3, cameraZ=3.1;
static GLfloat sphere_diffuseX = 0.411, sphere_diffuseY = 0.411, sphere_diffuseZ = 0.411, sphere_diffuseA = 1.0;
Vector3d Acceleration(0.0001,0.0001,0.0001);
static std::vector<Particle> particles;
static std::vector<Particle> oldparticles;

static Generator generator(origin, noOfParticles, timestep);

void do_material()
{
    
    float mat_ambient[] = {0.0,0.0,0.0,1.0};
    float mat_diffuse[] = {0.8, 1.0, 1.0,1.0};
    float mat_specular[] = {0.03,0.03,0.03,1.0};
    float mat_shininess[] = {1.0};
    
    glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
    glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
    glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
    
}

void do_lights()
{
    
    float light0_ambient[] = {0.0, 0.0, 0.0, 0.0};
    float light0_diffuse[] = {2.0, 2.0, 2.0, 0.0};
    float light0_specular[] = {2.25, 2.25, 2.25, 0.0};
    float light0_position[] = {1.5, 2.0, 2.0, 1.0};
    float light0_direction[] = { -1.5, -2.0, -2.0, -2.0, 1.0};
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light0_ambient);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
    
    glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0_direction);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
}

void setup_the_viewvolume()
{
    
    Vector3d eye, view, up;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(37.0, 1.0, 0.1, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    eye.x = cameraX;
    eye.y = cameraY;
    eye.z = cameraZ;
    view.x = 0.5;
    view.y = 0.2;
    view.z = 0.0;
    up.x = 0.0;
    up.y = 1.0;
    up.z = 0.0;
    
    gluLookAt(eye.x, eye.y, eye.z, view.x, view.y, view.z, up.x, up.y, up.z);
    
}

void doReshape(int w, int h)
{
    
    int vpw, vph;
    float aspect = float(WINDOW_WIDTH) / float(WINDOW_HEIGHT);
    
    if(float(w) / float(h) > aspect)
    {
        vph = h;
        vpw = int(aspect * h + 0.5);
    }
    else
    {
        vpw = w;
        vph = int(w / aspect + 0.5);
    }
    
    glViewport(0, 0, vpw, vph);
    WinWidth = w;
    WinHeight = h;
    setup_the_viewvolume();
    
}

void initParticleSystem()
{
    particles.reserve(noOfParticles);
    oldparticles.reserve(noOfParticles);
}

double RandomDouble(double a, double b)
{
    double random = ((double) rand()) / (double) RAND_MAX;
    double diff = b - a;
    double r = random * diff;
    return a + r;
}

void addParticle()
{
    double vx = RandomDouble(0.0,0.0);
    double vy = RandomDouble(0.0,0);
    double vz = RandomDouble(0.0,0.0);
    
    double x = RandomDouble(0.05,0.9);
    double y = RandomDouble(0.05,0.9);
    double z = RandomDouble(0.05,0.9);
    
    
    Vector3d V2(vx,vy,vz);
    Vector3d P2(x,y,z);

    oldparticles.push_back(Particle (P2, V2, Acceleration, mass));
    particles.push_back(Particle (P2, V2, Acceleration, mass));
}

void addParticles()
{
    for (int i= 0; i< noOfParticles; i++)
    {
        addParticle();
        
    }
}

void particleInteraction(const int i, const int j)
{
    double r = 0.0;
    r = (particles[j].Position-particles[i].Position).norm();
    
    Vector3d u;
    Vector3d aa;
    Vector3d av;
    Vector3d ac;
    
    
    u = (particles[j].Position-particles[i].Position).normalize();
    
    aa = (ka*u)/r;
    av = kv*(particles[j].Velocity- particles[i].Velocity);
    ac = kc * (particles[j].Position-particles[i].Position);
    
    
    particles[i].Acceleration = aa+av+ac;

}

void updateParticle(const double timestep, const int i)
{
    
    for (int j=0; j< noOfParticles; j++)
    {   if(i!=j)
        particleInteraction(i,j);
    }
    
    particles[i].Velocity = oldparticles[i].Velocity + timestep*oldparticles[i].Acceleration;
    particles[i].Position.x = oldparticles[i].Position.x + (oldparticles[i].Velocity.x * timestep);
    particles[i].Position.y = oldparticles[i].Position.y + (oldparticles[i].Velocity.y * timestep);
    particles[i].Position.z = oldparticles[i].Position.z + (oldparticles[i].Velocity.z * timestep);
    
    //detect collision with floor

    if(particles[i].Position.y - RADIUS <= FLOORHEIGHT && oldparticles[i].Position.y - RADIUS > FLOORHEIGHT)
    {
        std::cout<<"floor"<<std::endl;
        double f = (FLOORHEIGHT - (oldparticles[i].Position.y - RADIUS)) / (particles[i].Position.y - oldparticles[i].Position.y);
        particles[i].Velocity = oldparticles[i].Velocity + f * timestep * oldparticles[i].Acceleration;
        particles[i].Position = oldparticles[i].Position + f * timestep * oldparticles[i].Velocity;
        oldparticles[i].Velocity = particles[i].Velocity;
        oldparticles[i].Velocity.y *= -CoeffofRestitution;
        
        oldparticles[i].Position = particles[i].Position;
        
        particles[i].Velocity = oldparticles[i].Velocity + (1 - f) * timestep * oldparticles[i].Acceleration;
        particles[i].Position = particles[i].Position + (1 - f) * timestep * oldparticles[i].Velocity;
    }
    
    // left face collision
    if (particles[i].Position.x - RADIUS < BOXLEFT && oldparticles[i].Position.x - RADIUS > BOXLEFT)
    {
        std::cout<<"left"<<std::endl;
        double f = (BOXLEFT - (oldparticles[i].Position.x - RADIUS)) / (particles[i].Position.y - oldparticles[i].Position.y);
        particles[i].Velocity = oldparticles[i].Velocity + f * timestep * oldparticles[i].Acceleration;
        particles[i].Position = oldparticles[i].Position + f * timestep * oldparticles[i].Velocity;
        oldparticles[i].Velocity = particles[i].Velocity;
        oldparticles[i].Velocity.x *= -CoeffofRestitution;
        
        oldparticles[i].Position = particles[i].Position;
        
        particles[i].Velocity = oldparticles[i].Velocity + (1 - f) * timestep * oldparticles[i].Acceleration;
        particles[i].Position = particles[i].Position + (1 - f) * timestep * oldparticles[i].Velocity;
        
    }
    
    // right face collision
    if (particles[i].Position.x - RADIUS <= BOXRIGHT && oldparticles[i].Position.x + RADIUS > BOXRIGHT)
    {
        std::cout<<"right"<<std::endl;
        double f = (BOXRIGHT - (oldparticles[i].Position.x + RADIUS)) / (particles[i].Position.x - oldparticles[i].Position.x);
        particles[i].Velocity = oldparticles[i].Velocity + f * timestep * oldparticles[i].Acceleration;
        particles[i].Position = oldparticles[i].Position + f * timestep * oldparticles[i].Velocity;
        oldparticles[i].Velocity = particles[i].Velocity;
        oldparticles[i].Velocity.x *= -CoeffofRestitution;
        
        oldparticles[i].Position = particles[i].Position;
        
        
        particles[i].Velocity = oldparticles[i].Velocity + (1 - f) * timestep * oldparticles[i].Acceleration;
        particles[i].Position = particles[i].Position + (1 - f) * timestep * oldparticles[i].Velocity;
        
    }
    
    // top face collision
    if ((particles[i].Position.y + RADIUS) >= BOXHEIGHT && oldparticles[i].Position.y + RADIUS < BOXHEIGHT)
    {
        std::cout<<"top"<<std::endl;
        double f = (oldparticles[i].Position.y - RADIUS) / (oldparticles[i].Position.y - particles[i].Position.y);
        particles[i].Velocity = oldparticles[i].Velocity + f * timestep * oldparticles[i].Acceleration;
        particles[i].Position = oldparticles[i].Position + f * timestep * oldparticles[i].Velocity;
        oldparticles[i].Velocity = particles[i].Velocity;
        oldparticles[i].Velocity.y *= -CoeffofRestitution;
        
        oldparticles[i].Position = particles[i].Position;
        
        
        particles[i].Velocity = oldparticles[i].Velocity + (1 - f) * timestep * oldparticles[i].Acceleration;
        particles[i].Position = particles[i].Position + (1 - f) * timestep * oldparticles[i].Velocity;
        
    }
    
    // front face collision
    if (particles[i].Position.z + RADIUS >= BOXFRONT && oldparticles[i].Position.z + RADIUS < BOXFRONT)
    {
        std::cout<<"front"<<std::endl;
        double f = (BOXFRONT - (oldparticles[i].Position.z + RADIUS)) / (particles[i].Position.z - oldparticles[i].Position.z);
        particles[i].Velocity = oldparticles[i].Velocity + f * timestep * oldparticles[i].Acceleration;
        particles[i].Position = oldparticles[i].Position + f * timestep * oldparticles[i].Velocity;
        oldparticles[i].Velocity = particles[i].Velocity;
        oldparticles[i].Velocity.z *= -CoeffofRestitution;
        
        oldparticles[i].Position = particles[i].Position;
        
        
        particles[i].Velocity = oldparticles[i].Velocity + (1 - f) * timestep * oldparticles[i].Acceleration;
        particles[i].Position = particles[i].Position + (1 - f) * timestep * oldparticles[i].Velocity;
        
    }
    
    // back face collision
    if (particles[i].Position.z - RADIUS < BOXBACK && oldparticles[i].Position.z - RADIUS > BOXBACK)
    {
        std::cout<<"back"<<std::endl;
        double f = (BOXLEFT - (oldparticles[i].Position.z - RADIUS)) / (particles[i].Position.z - oldparticles[i].Position.z);
        particles[i].Velocity = oldparticles[i].Velocity + f * timestep * oldparticles[i].Acceleration;
        particles[i].Position = oldparticles[i].Position + f * timestep * oldparticles[i].Velocity;
        oldparticles[i].Velocity = particles[i].Velocity;
        oldparticles[i].Velocity.z *= -CoeffofRestitution;
        
        oldparticles[i].Position = particles[i].Position;
        
        
        particles[i].Velocity = oldparticles[i].Velocity + (1 - f) * timestep * oldparticles[i].Acceleration;
        particles[i].Position = particles[i].Position + (1 - f) * timestep * oldparticles[i].Velocity;
        
    }


    oldparticles = particles;
}

void do_particle_lights()
{
    
    GLfloat sphere_diffuse[] = { sphere_diffuseX, sphere_diffuseY, sphere_diffuseZ, sphere_diffuseA};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, sphere_diffuse);
}

void DrawParticle (float x, float y, float z)
{
    /*
     do_particle_lights();
     glBegin(GL_POINTS);
     glVertex3f(x,y,z);
     glEnd();
    
    */
    
    float drawx = x;
    float drawy = y;
    float drawz = z;
    float drawrad = 0.03;
    
    do_particle_lights();
    
    glPushMatrix();
    glTranslatef (drawx,drawy,drawz);
    glutSolidSphere(drawrad,40,40);
    glPopMatrix ();
    
    glFlush();
    
}



void simulateParticle(const double timestep, const int i)
{
    updateParticle(timestep,i);
    DrawParticle(oldparticles[i].Position.x, oldparticles[i].Position.y, oldparticles[i].Position.z);
}



void runGenerator(Generator &generator)
{
    
    for (int i= 0; i< noOfParticles; i++)
    {
        simulateParticle(timestep, i);
        
    }
    
}

void DrawBox()
{
    
    do_lights();
    do_material();
    
    int i;
    
    Vector3d front[4] = { Vector3d(0.0,0.0,1.0), Vector3d(0.0,1.0,1.0), Vector3d(1.0,1.0,1.0), Vector3d(1.0,0.0,1.0)};
    Vector3d back[4] = {Vector3d(1.0,0.0,0.0),Vector3d(1.0,1.0,0.0),Vector3d(0.0,1.0,0.0),Vector3d(0.0,0.0,0.0)};
    Vector3d left[4] = {Vector3d(0.0,0.0,0.0),Vector3d(0.0,1.0,0.0),Vector3d(0.0,1.0,1.0),Vector3d(0.0,0.0,1.0)};
    Vector3d right[4] = {Vector3d(1.0,0.0,1.0),Vector3d(1.0,1.0,1.0),Vector3d(1.0,1.0,0.0),Vector3d(1.0,0.0,0.0)};
    Vector3d top[4] = {Vector3d(0.0,1.0,1.0),Vector3d(0.0,1.0,0.0),Vector3d(1.0,1.0,0.0),Vector3d(1.0,1.0,1.0)};
    Vector3d bottom[4] = {Vector3d(0.0,0.0,0.0),Vector3d(0.0,0.0,1.0),Vector3d(1.0,0.0,1.0),Vector3d(1.0,0.0,0.0)};
    
    glBegin(GL_QUADS);
    /*
     Do not draw the front surface for now
     
     glNormal3f(0.0, 0.0, -1.0);
     for(i=0;i<4;i++) glVertex3f(front[i].x, front[i].y, front[i].z);
     */
    glNormal3f(0.0, 0.0, 1.0);
    for(i=0;i<4;i++) glVertex3f(back[i].x, back[i].y, back[i].z);
    glNormal3f(1.0, 0.0, 0.0);
    for(i=0;i<4;i++) glVertex3f(left[i].x, left[i].y, left[i].z);
    glNormal3f(1.0, 0.0, 0.0);
    for(i=0;i<4;i++) glVertex3f(right[i].x, right[i].y, right[i].z);
    glNormal3f(0.0, 1.0, 0.0);
    for(i=0;i<4;i++) glVertex3f(top[i].x, top[i].y, top[i].z);
    glNormal3f(0.0, 1.0, 0.0);
    for(i=0;i<4;i++) glVertex3f(bottom[i].x, bottom[i].y, bottom[i].z);
    glEnd();
    
    glFlush();
    
}


void drawDisplay()
{
   
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0,0.0,0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    //DrawBox();
    
    runGenerator(generator);
    
    glutSwapBuffers();
    
    
}

void Timer(int unused)
{
    glutPostRedisplay();
    glutTimerFunc(20, Timer, 0);
}

void handleKey(unsigned char key, int x, int y)
{
    double delta = 0.5;
    
    switch (key) {
        case 'a' :
            cameraX -= delta;
            break;
        case 'd' :
            cameraX += delta;
            break;
        case 'w' :
            cameraY += delta;
            break;
        case 's' :
            cameraY -= delta;
            break;
        case 'q' :
            cameraZ += delta;
            break;
        case 'e' :
            cameraZ -= delta;
            break;
        case 'r' :
            sphere_diffuseX = 1.0, sphere_diffuseY = 0.2, sphere_diffuseZ = 0.0, sphere_diffuseA = 1.0;
            break;
        case 'g' :
            sphere_diffuseX = 0.2, sphere_diffuseY = 0.8, sphere_diffuseZ = 0.2, sphere_diffuseA = 1.0;
            break;
        case 'b' :
            sphere_diffuseX = 0.188, sphere_diffuseY = 0.188, sphere_diffuseZ = 1.0, sphere_diffuseA = 1.0;
            break;
        case 'o' :
            sphere_diffuseX = 0.411, sphere_diffuseY = 0.411, sphere_diffuseZ = 0.411, sphere_diffuseA = 1.0;
            break;
        case 27:
            exit(0);
            break;
            
        default:
            return;
            break;
       
    }
    setup_the_viewvolume();
    glutPostRedisplay();
    
}


int main(int argc, char* argv[])
{
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGBA | GLUT_DEPTH );
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100,50);
    glutCreateWindow("Flocking and Interacting Particle Systems");
    glutReshapeFunc(doReshape);
    do_lights();
    do_material();
    initParticleSystem();
    addParticles();
    glutDisplayFunc(drawDisplay);
    Timer(0);
    glutKeyboardFunc(handleKey);
    glutMainLoop();
    return 0;
    
}

