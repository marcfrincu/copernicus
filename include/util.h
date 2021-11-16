#define DEBUG

#define DISTANCE(x,y,z) sqrt((x)*(x)+(y)*(y)+(z)*(z))
#define DEG2RAD(x) ((x)*radian_correction)
#define RAD2DEG(x) ((x)*1/radian_correction)

static double radian_correction = 3.14159265358979323846f / 180.0f;

extern char* pwd;

double rev ( double x );
void polar_to_cartesian ( float azimuth, float length, float altitude, float *x, float *y, float *z );
int between ( float a, float b, float x );
char* getWorkingDir();
double distance (double x1, double y1, double z1, double x2, double y2, double z2 );
void cartesian_to_polar ( float x, float y, float z, float *azimuth, float *altitude, float *length );

