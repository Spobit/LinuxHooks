#include "DlsymHook.h"

#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <dlfcn.h>

#include <string>
#include <vector>
#include <map>

#include "Global.h"
#include "WhiteList.h"

using namespace std;

struct MapsItem1{
    string addrrange;
    string memattr;
    string offset;
    string deviceno;
    string inode;
    string path;

    MapsItem1( const char* s1, const char* s2, const char* s3,
               const char* s4, const char* s5, const char* s6)
    {
        addrrange = s1;
        memattr = s2;
        offset = s3;
        deviceno = s4;
        inode = s5;
        path = s6;
    }
};

struct MapsItem2{
    unsigned long int startaddr;
    unsigned long int endaddr;
    string path;

    MapsItem2(unsigned long int s1, unsigned long int s2, const char* s3)
    {
        startaddr = s1;
        endaddr = s2;
        path = s3;
    }
};

struct ExportInof1 {
    unsigned long int offset;
    char type;
    string name;

    ExportInof1(unsigned long int off, char t, const char* n)
    {
        offset = off;
        type = t;
        name = n;
    }
};

static unsigned long int kOffset = 0;
#ifdef _HAVE_MULTI_THREAD_SAFETY_FLAG
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static int MapsInfo(vector<MapsItem2>& infos)
{

#define EIT_ERR_EXIT(errstring) do{ \
    if (fp) { fclose(fp); fp = NULL; } \
    snprintf(tmpchars, sizeof(tmpchars), "%s" " (%s:%u:%d)", \
    errstring, __FILE__, __LINE__, getpid()); \
    return -1; \
}while (0)

    ///> vars
    char tmpchars[PATH_MAX] = "";
    string mapsfullpath = "";
    vector<string> linestrings;
    vector<MapsItem1> mapsitem1s;
    FILE* fp = NULL;
    int tmpint = 0;

    ///> init
    snprintf(tmpchars, sizeof(tmpchars), "/proc/%d/maps", getpid());
    mapsfullpath = tmpchars;

    ///> get line string vector
    // file can access
    if (access(mapsfullpath.c_str(), F_OK | R_OK))
        EIT_ERR_EXIT(strerror(errno));

    //
    fp = fopen(mapsfullpath.c_str(), "r");
    if (!fp)
        EIT_ERR_EXIT(strerror(errno));

    while (fgets(tmpchars, sizeof(tmpchars), fp))
        linestrings.push_back(tmpchars);
    tmpint = errno;
    if (!feof(fp))
        EIT_ERR_EXIT(strerror(tmpint));

    fclose(fp);
    fp = NULL;

    ///> split
    {
        char tmp[6][PATH_MAX] = {};
        for (size_t i = 0; i < linestrings.size(); i++)
        {
            memset(tmp[5], 0, sizeof(tmp[5]));
            sscanf( linestrings[i].c_str(), "%s %s %s %s %s %s",
                    tmp[0], tmp[1], tmp[2],
                    tmp[3], tmp[4], tmp[5]);

            if (!(strlen(tmp[0]) && strlen(tmp[1]) && strlen(tmp[5])))
                continue;

            mapsitem1s.push_back(MapsItem1(tmp[0], tmp[1], tmp[2],
                    tmp[3], tmp[4], tmp[5]));
        }
    }

    ///> parse
    for (size_t i = 0; i < mapsitem1s.size(); i++)
    {
        string startaddr = "";
        string endaddr = "";
        string memattr = "";
        string path = "";
        unsigned long int start;
        unsigned long int end;

        size_t hyphenpos = mapsitem1s[i].addrrange.find("-");
        if (string::npos == hyphenpos) continue;
        startaddr = mapsitem1s[i].addrrange.substr(0, hyphenpos);
        endaddr = mapsitem1s[i].addrrange.substr(hyphenpos);
        memattr = mapsitem1s[i].memattr;
        path = mapsitem1s[i].path;

        start = strtoull(startaddr.c_str(), 0, 16);
        end = strtoull(endaddr.c_str() + 1, 0, 16);

        if ( (ULONG_MAX == start) || (ULONG_MAX == end) )
            continue;
        if (!(start && end))
            continue;
        if (start >= end)
            continue;
        if ("r-xp" != memattr)
            continue;
        if (!( string::npos != path.find("libdl") &&
               string::npos != path.find(".so")))
            continue;

        infos.push_back(MapsItem2(start, end, path.c_str()));
    }

    ///>
    return 0;
}

static int ExportInof(vector<ExportInof1>& infos, const string lib)
{
    int success = 0;

    string cmd = "nm -D ";
    cmd += lib;
    cmd += " | grep T";

    FILE* fp = popen(cmd.c_str(), "r");
    if (!fp)
        return -1;

    char line[PATH_MAX] = "";
    char item[3][PATH_MAX] = {};
    unsigned long int offset = 0;
    while (fgets(line, sizeof(line), fp))
    {
        memset(item, 0, sizeof(item));
        sscanf(line, "%s %s %s", item[0], item[1], item[2]);

        if (!(strlen(item[0]) && strlen(item[1]) && strlen(item[2])))
            continue;
        if (0 != strcmp(item[1], "T"))
            continue;

        offset = strtoull(item[0], 0, 16);
        if (ULONG_MAX == offset)
            continue;

        infos.push_back(ExportInof1(offset, 'T', item[2]));
        success = 1;
    }
    pclose(fp);

    if (success)
        return 0;

    return 1;
}

static int IdentifyLibdl(vector<ExportInof1>& infos)
{
#define NUM_DEF_SYMBOLS 3

    const char defsymbols[NUM_DEF_SYMBOLS][10] = {"dlopen", "dlsym", "dlclose"};
    int found = 0;
    for (size_t i = 0; i < infos.size(); i++)
    {
        for (size_t j = 0; j < NUM_DEF_SYMBOLS; j++)
        {
            if (infos[i].name == defsymbols[j])
                found++;
            if (infos[i].name == "dlsym")
                kOffset = infos.at(i).offset;
        }
    }
    if (NUM_DEF_SYMBOLS == found)
        return 0;

    return -1;

#undef NUM_DEF_SYMBOLS
}

static int IdentifyMapItem(vector<MapsItem2>& infos, int& index)
{
    for (size_t i = 0; i < infos.size(); i++)
    {
        vector<ExportInof1> expinfo;
        if (ExportInof(expinfo, infos.at(i).path.c_str()))
            continue;
        if (IdentifyLibdl(expinfo))
            continue;

        index = i;
        return 0;
    }

    return -1;
}

void* dlsym(void * __restrict handle, const char * __restrict symbol) throw()
{
    EIT_SYSLOG(LOG_DEBUG, "Enter %s!", __FUNCTION__);

    static Dlsym kOrgDlsym = NULL;

    if (!kOrgDlsym)
    {
#ifdef _HAVE_MULTI_THREAD_SAFETY_FLAG
        pthread_mutex_lock(&mutex);
#endif
        if (!kOrgDlsym)
        {
            vector<MapsItem2> mapsinfos;
            if (MapsInfo(mapsinfos))
            {
                EIT_SYSLOG(LOG_EMERG, "%s", "Failed to invoke MapsInfo()!");
                abort();
            }

            int index = -1;
            if (IdentifyMapItem(mapsinfos, index))
            {
                EIT_SYSLOG(LOG_EMERG, "%s", "Failed to invoke IdentifyMapItem()!");
                abort();
            }

            kOrgDlsym = (Dlsym)(mapsinfos.at(index).startaddr + kOffset);
            EIT_SYSLOG(LOG_DEBUG, "kOrgDlsym = 0x%p", kOrgDlsym);

            void* handle = dlopen("/usr/lib/libcups.so.2", RTLD_NOW);
            if (!handle)
            {
                EIT_SYSLOG(LOG_DEBUG, "Please link libcups.s0.2 to "
                                      "/usr/lib/libcups.so.2");
                EIT_SYSLOG(LOG_DEBUG, "Leave %s!", __FUNCTION__);
                return NULL;
            }

            kOrgIppRead = (IppRead)kOrgDlsym(handle, "ippRead");
            EIT_SYSLOG(LOG_DEBUG, "kOrgIppRead = 0x%p", kOrgIppRead);

            kOrgIppReadFile = (IppReadFile)kOrgDlsym(handle, "ippReadFile");
            EIT_SYSLOG(LOG_DEBUG, "kOrgIppReadFile = 0x%p", kOrgIppReadFile);

            kOrgIppReadIO = (IppReadIO)kOrgDlsym(handle, "ippReadIO");
            EIT_SYSLOG(LOG_DEBUG, "kOrgIppReadIO = 0x%p", kOrgIppReadIO);

            kCupsDoRequest = (CupsDoRequest)kOrgDlsym(handle, "cupsDoRequest");
            EIT_SYSLOG(LOG_DEBUG, "kCupsDoRequest = 0x%p", kCupsDoRequest);

            kOrgCupsGetDests2 = (CupsGetDests2)kOrgDlsym(handle, "cupsGetDests2");
            EIT_SYSLOG(LOG_DEBUG, "kOrgCupsGetDests2 = 0x%p", kOrgCupsGetDests2);

            // dlclose(handle);

            kIsHook = IsHook();
            EIT_SYSLOG(LOG_DEBUG, "kIsHook = 0x%d", kIsHook);
        }
#ifdef _HAVE_MULTI_THREAD_SAFETY_FLAG
        pthread_mutex_unlock(&mutex);
#endif
    }

    void* addr = NULL;
    if (0 == strcmp("ippRead", symbol))
    {
        addr = (void*)ippRead;
    }
    else if (0 == strcmp("cupsGetDests", symbol))
    {
        addr = (void*)cupsGetDests;
    }
    else if (0 == strcmp("cupsGetDests2", symbol))
    {
        addr = (void*)cupsGetDests2;
    }
    else
    {
        addr = kOrgDlsym(handle, symbol);
        EIT_SYSLOG(LOG_DEBUG, "%s : 0x%p", symbol, addr);
    }

    EIT_SYSLOG(LOG_DEBUG, "Leave %s!", __FUNCTION__);
    return addr;
}


