#include <math.h>

#define MAX(a, b)      ((a) > (b) ? (a) : (b))
#define MIN(a, b)      ((a) < (b) ? (a) : (b))
#define CLAMP(x, a, b) (MIN((b), MAX((x), (a))))

#define DEG2RAD(x) ((x) * M_PI / 180.0f)
#define RAD2DEG(x) ((x) * 180.0f / M_PI)

unsigned int hash(const char *data);
