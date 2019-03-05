#include "Hook_DynamicLoadDotSharedObject.h"

#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <dlfcn.h>

#include "TypeDecl.h"
#include "DlopenPtrAddr.h"
#include "Module.h"


// journalctl -p 6..0 -f | grep wps
// #define	LOG_EMERG	0	/* system is unusable */
// #define	LOG_ALERT	1	/* action must be taken immediately */
// #define	LOG_CRIT	2	/* critical conditions */
// #define	LOG_ERR		3	/* error conditions */
// #define	LOG_WARNING	4	/* warning conditions */
// #define	LOG_NOTICE	5	/* normal but significant condition */
// #define	LOG_INFO	6	/* informational */
// #define	LOG_DEBUG	7	/* debug-level messages */
void* dlsym(void * __restrict handle, const char * __restrict symbol)
{
    EIT_SYSLOG(LOG_DEBUG, "%s", "Enter dlsym!");

    ///>
    static unsigned long long offset = (unsigned long long)0;
    if ((unsigned long long)0 == offset)
    {
        std::map<std::string, int> libs;
        if (get_load_libs_string_in_maps(libs))
        {
            EIT_SYSLOG(LOG_CRIT, "%s",
                       "Failed to invoke get_load_libs_string_in_maps()!");
            abort();
        }

        get_expected_libs_strings(libs);

        offset = get_offset_from_dlopen_to_dlsym_in_libs(libs);
        if ((unsigned long long)0 == offset)
        {
            EIT_SYSLOG(LOG_CRIT, "%s", "Failed to invoke "
                                       "get_offset_from_dlopen_to_dlsym_in_libs()!");
            abort();
        }
    }

    static DLSYM orgdlsym = (DLSYM)((unsigned long long)orgdlopen() + offset);
    if (!orgdlsym)
    {
        EIT_SYSLOG(LOG_CRIT, "%s", "Failed to orgdlsym!");
        abort();
    }

    ///>
    void* addr = orgdlsym(handle, symbol);

    ///>
    char tmpchars[MAX_INPUT];
    snprintf(tmpchars, sizeof(tmpchars), "%s: %p", symbol, addr);
    EIT_SYSLOG(LOG_INFO, "%s", tmpchars);

    ///>
    EIT_SYSLOG(LOG_DEBUG, "%s", "Leave dlsym!");
    return addr;
}

int get_load_libs_string_in_maps(std::map<std::string, int>& libs)
{

#define EIT_ERR_EXIT(errstring) do{ \
    if (fp) { fclose(fp); fp = NULL; } \
    snprintf(tmpchars, sizeof(tmpchars), "%s" " (%s:%u:%d)", \
    errstring, __FILE__, __LINE__, getpid()); \
    libs.clear(); \
    libs[tmpchars]++; \
    return -1; \
}while (0)

    ///> vars
    char tmpchars[PATH_MAX] = "";
    std::string mapsfullpath = "";
    std::vector<std::string> linestrings;

    //
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

    ///> get lib strings map
    char tmpchars1[6][PATH_MAX] = {};
    for (size_t i = 0; i < linestrings.size(); i++)
    {
        memset(tmpchars1[5], 0, sizeof(tmpchars1[5]));
        sscanf(linestrings[i].c_str(), "%s %s %s %s %s %s",
               tmpchars1[0], tmpchars1[1], tmpchars1[2],
                tmpchars1[3], tmpchars1[4], tmpchars1[5]);
        if (strlen(tmpchars1[5]))
            libs[tmpchars1[5]]++;
    }

    if (0 == libs.size())
        EIT_ERR_EXIT("It is no libs loaded?");

    ///>
    return 0;
}

void get_expected_libs_strings(std::map<std::string, int> &libs)
{
    for (std::map<std::string, int>::iterator it = libs.begin();
         it != libs.end(); it++)
    {
        //it.
        if (!(std::string::npos != it->first.find("libdl") &&
              std::string::npos != it->first.find(".so")))
            libs.erase(it);
    }
}

unsigned long long get_offset_from_dlopen_to_dlsym(const std::string& lib)
{
    ///> vars
    const char* exportsmybols[2] = { "dlopen", "dlsym" };
    unsigned long long pos[2] = {};

    //
    FILE* fp = NULL;
    char cmdchars[PATH_MAX];

    ///> init
    if (lib.empty())
        return (unsigned long long)0;

    ///>
    char tmpchars[3][0x20];
    for (size_t i = 0; i < sizeof(exportsmybols) / sizeof(char*); i++)
    {
        snprintf(cmdchars, sizeof(cmdchars), "nm -D %s | grep %s",
                 lib.c_str(), exportsmybols[i]);

        if (fp) pclose(fp);
        fp = popen(cmdchars, "r");
        if (!fp) return (unsigned long long)0;
        if (fgets(cmdchars, sizeof(cmdchars), fp))
        {
            memset(tmpchars[2], 0, sizeof(tmpchars[2]));
            sscanf(cmdchars, "%s %s %s", tmpchars[0], tmpchars[1], tmpchars[2]);
            if (0 == strcmp(tmpchars[2], exportsmybols[i]))
            {
                pos[i] = strtoull(tmpchars[0], 0, 16);
                continue;
            }
        }
        pclose(fp);
        fp = NULL;
    }

    ///>
    if ( !pos[0] ||
         !pos[1] ||
         pos[1] - pos[0] == (unsigned long long)0 ||
         pos[1] - pos[0] > (unsigned long long)0xffff)
        return (unsigned long long)0;

    ///>
    return pos[1] - pos[0];
}

unsigned long long get_offset_from_dlopen_to_dlsym_in_libs(std::map<std::string, int> &libs)
{
    for (std::map<std::string, int>::iterator it = libs.begin();
         it != libs.end(); it++)
    {
        unsigned long long offset = 0;
        offset = get_offset_from_dlopen_to_dlsym(it->first);
        if ((unsigned long long)0 != offset)
            return offset;
    }

    return 0;
}
