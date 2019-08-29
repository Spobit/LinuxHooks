#ifndef DLSYMHOOK_H
#define DLSYMHOOK_H

#include "Type.h"

/**
 * @brief dlsym
 * @return the fuction hooks dlsym of system.
 */
EXTERNC void* dlsym(void * __restrict handle, const char * __restrict symbol) throw();

#endif // DLSYMHOOK_H
