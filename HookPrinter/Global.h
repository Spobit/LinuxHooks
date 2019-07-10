#ifndef GLOBAL_H
#define GLOBAL_H

#include "Type.h"


// log
#ifdef EIT_GIT_DEBUG
# define EIT_SYSLOG(pri, fmt, args...) do{ \
    std::string nakedfilename = __FILE__; \
    if (std::string::npos != nakedfilename.rfind("/")) \
    nakedfilename = nakedfilename.substr(nakedfilename.rfind("/") + 1); \
    syslog(pri,"HookDlsym " fmt " (%s:%u)",\
    ##args, nakedfilename.c_str(), __LINE__); \
    }while (0)
#else
# define EIT_SYSLOG(pri, fmt, args...) do{}while (0)
#endif


// module variables
extern __attribute__((visibility("hidden"))) IppRead kOrgIppRead;
extern __attribute__((visibility("hidden"))) IppReadFile kOrgIppReadFile;
extern __attribute__((visibility("hidden"))) IppReadIO kOrgIppReadIO;
extern __attribute__((visibility("hidden"))) CupsDoRequest kCupsDoRequest;
extern __attribute__((visibility("hidden"))) CupsGetDests2 kOrgCupsGetDests2;

extern __attribute__((visibility("hidden"))) bool kIsHook;

#endif // GLOBAL_H
