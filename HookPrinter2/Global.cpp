#include "Global.h"

IppRead kOrgIppRead = NULL;
IppReadFile kOrgIppReadFile = NULL;
IppReadIO kOrgIppReadIO = NULL;
CupsDoRequest kCupsDoRequest = NULL;
CupsGetDests2 kOrgCupsGetDests2 = NULL;
bool kIsHook = true;


///> cups full name hook
Dlsym                   kOrgDlsym               = NULL;
CupsCreateJob           kCupsCreateJob          = NULL;
CupsCreateDestJob       kCupsCreateDestJob      = NULL;
CupsPrintFiles          kCupsPrintFiles         = NULL;
CupsPrintFiles2         kCupsPrintFiles2        = NULL;
CupsPrintFile           kCupsPrintFile          = NULL;
CupsPrintFile2          kCupsPrintFile2         = NULL;
CupsStartDestDocument   kCupsStartDestDocument  = NULL;
CupsStartDocument       kCupsStartDocument      = NULL;
IppAddString            kIppAddString           = NULL;
