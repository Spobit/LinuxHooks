#ifndef MODULE_H
#define MODULE_H

#include <syslog.h>
#include <string.h>

#include "TypeDecl.h"

// log
#ifdef EIT_GIT_DEBUG
# define EIT_SYSLOG(pri, fmt, args...) do{ \
    std::string nakedfilename = __FILE__; \
    if (std::string::npos != nakedfilename.rfind("/")) \
    nakedfilename = nakedfilename.substr(nakedfilename.rfind("/") + 1); \
    syslog(pri, fmt " (%s:%u)",\
    ##args, nakedfilename.c_str(), __LINE__); \
    }while (0)
#else
# define EIT_SYSLOG(pri, fmt, args...) do{}while (0)
#endif // MODULE_H

#endif // MODULE_H
