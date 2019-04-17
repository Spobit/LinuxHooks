#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <syslog.h>

#include <string>
#include <vector>
#include <map>

using namespace std;

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

typedef void* (*Dlsym)(void * __restrict handle, const char * __restrict symbol);

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
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

__attribute__ ((visibility("hidden")))
int MapsInfo(vector<MapsItem2>& infos)
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

__attribute__ ((visibility("hidden")))
int ExportInof(vector<ExportInof1>& infos, const string lib)
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

__attribute__ ((visibility("hidden")))
int IdentifyLibdl(vector<ExportInof1>& infos)
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

__attribute__ ((visibility("hidden")))
int IdentifyMapItem(vector<MapsItem2>& infos, int& index)
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

#ifdef __cplusplus
extern "C"
#endif
void* dlsym(void * __restrict handle, const char * __restrict symbol)
{
    EIT_SYSLOG(LOG_DEBUG, "%s", "Enter dlsym!");

    static Dlsym kOrgDlsym = NULL;

    if (!kOrgDlsym)
    {
        pthread_mutex_lock(&mutex);
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
        }
        pthread_mutex_unlock(&mutex);
    }

    EIT_SYSLOG(LOG_DEBUG, "%s", "before invoke kOrgDlsym()!");
    void* addr = kOrgDlsym(handle, symbol);
    EIT_SYSLOG(LOG_DEBUG, "%s", "after invoke kOrgDlsym()!");
    EIT_SYSLOG(LOG_DEBUG, "%s : %p", symbol, addr);
    EIT_SYSLOG(LOG_DEBUG, "%s", "Leave dlsym!");
    return addr;
}
