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

// init function, see README
extern __attribute__((visibility("hidden"))) void HookPrinterInit(void);

// module variables
extern __attribute__((visibility("hidden"))) Dlsym                  kOrgDlsym;

extern __attribute__((visibility("hidden"))) IppRead                kOrgIppRead;
extern __attribute__((visibility("hidden"))) IppReadFile            kOrgIppReadFile;
extern __attribute__((visibility("hidden"))) IppReadIO              kOrgIppReadIO;
extern __attribute__((visibility("hidden"))) CupsDoRequest          kCupsDoRequest;
extern __attribute__((visibility("hidden"))) CupsGetDests2          kOrgCupsGetDests2;

extern __attribute__((visibility("hidden"))) bool                   kIsHook;


extern __attribute__((visibility("hidden"))) CupsCreateJob           kCupsCreateJob            ;
extern __attribute__((visibility("hidden"))) CupsCreateDestJob       kCupsCreateDestJob        ;
extern __attribute__((visibility("hidden"))) CupsPrintFiles          kCupsPrintFiles           ;
extern __attribute__((visibility("hidden"))) CupsPrintFiles2         kCupsPrintFiles2          ;
extern __attribute__((visibility("hidden"))) CupsPrintFile           kCupsPrintFile            ;
extern __attribute__((visibility("hidden"))) CupsPrintFile2          kCupsPrintFile2           ;
extern __attribute__((visibility("hidden"))) CupsStartDestDocument   kCupsStartDestDocument    ;
extern __attribute__((visibility("hidden"))) CupsStartDocument       kCupsStartDocument        ;
extern __attribute__((visibility("hidden"))) IppAddString            kIppAddString             ;

#endif // GLOBAL_H
