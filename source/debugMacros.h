#ifndef __DEBUG_MACROS_H
#define __DEBUG_MACROS_H

#include <iostream.h>

//#define DEBUG 1

#ifdef DEBUG

#define DPRINTVAR(x) cout << #x " = " << x << "\n";
#define DPRINT(x) cout << x << "\n";

#else
#define DPRINTVAR(x)
#define DPRINT(x) 

#endif //DEBUG

#endif //__DEBUG_MACROS_H