#include "DlopenPtrAddr.h"

#include <stddef.h>
#include <dlfcn.h>

__attribute ((visibility("hidden"))) DLOPEN g_orgdlopen = NULL;


DLOPEN orgdlopen()
{
    dlopen("111", RTLD_NOW);
    if (!g_orgdlopen)
        g_orgdlopen = dlopen;
    return g_orgdlopen;
}
