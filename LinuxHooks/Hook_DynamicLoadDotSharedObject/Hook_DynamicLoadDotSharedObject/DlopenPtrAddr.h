#ifndef DLOPENPTRADDR
#define DLOPENPTRADDR

#include "TypeDecl.h"

#ifdef __cplusplus
extern "C"
#endif
__attribute__ ((visibility("hidden"))) DLOPEN orgdlopen();

//extern "C" void fun();

#endif // DLOPENPTRADDR

