#include "WhiteList.h"

#include <sys/types.h>
#include <stdio.h>
#include <limits.h>

#include "Global.h"

static char* GetInstallPath()
{
    static char kInstallPath[256] = "";
    if (0 == strlen(kInstallPath))
    {
        FILE* fp = fopen("/tmp/cups-pdf-eit90123/RetrievingPrinterWhitelist.psc", "r");
        if (fp)
        {
            fgets(kInstallPath, sizeof kInstallPath, fp);
            fclose(fp);
        }
    }
    return kInstallPath;
}



static void GetNameByPid(pid_t pid, char *task_name, int len)
{
    EIT_SYSLOG(LOG_DEBUG, "Enter %s!", __FUNCTION__);

    char proc_pid_path[1024];
    char buf[1024];

    sprintf(proc_pid_path, "/proc/%d/exe", pid);
    int pos = readlink(proc_pid_path, buf, 1024);
    if(pos < 0 || pos >= 1024)  return;

    buf[pos] = '\0';
    while(buf[pos-1] != '/')  pos--;

    if(pos > 0)
        strncpy(task_name, &buf[pos], len);
    else
        strncpy(task_name, buf, len);

    EIT_SYSLOG(LOG_DEBUG, "Leave %s!", __FUNCTION__);
}

/**We hook ippRead, but WY module need get printers infos, so there has unhook process list
 * the format is such as :
 * /
 * /SecWYManager/
 * /SecWYManager/SecWYManager1/
 * /SecWYManager/SecWYManager1/SecWYManager2/
 * /SecWYManager/SecWYManager1/SecWYManager2/.../
 * note: the buffer size is only 4096 bytes.
 */
static const char* GetUnhookProcList()
{
    char fullfilename[1024];
    static char s_szUnhookProcList[4096] = "/";
    FILE* pf = 0;
    char szBuff[NAME_MAX + 8] = {0};
    char* szTmp = 0;

    EIT_SYSLOG(LOG_DEBUG, "Enter getUnhookProcList()");


    if (1 != strlen(s_szUnhookProcList))
        return s_szUnhookProcList;

    EIT_SYSLOG(LOG_DEBUG, "kInstallPath = %s", GetInstallPath());
    snprintf(fullfilename, sizeof fullfilename, "%s/%s", GetInstallPath(), "Policy/RetrievingPrinterWhitelist.psc");
    EIT_SYSLOG(LOG_DEBUG, "fullfilename = %s", fullfilename);
    pf = fopen(fullfilename, "r");
    if (!pf)
        pf = fopen("/usr/local/inforsecclient/Policy/RetrievingPrinterWhitelist.psc", "r");
    if (!pf)
        pf = fopen("/usr/local/inforsecclient/Cfg/RetrievingPrinterWhitelist.psc", "r");
    if (!pf)
    {
        EIT_SYSLOG(LOG_DEBUG, "fopen() failed!");
        return s_szUnhookProcList;
    }

    while (fgets(szBuff, sizeof szBuff, pf) != NULL)
    {
        EIT_SYSLOG(LOG_DEBUG, "fgets(): %s", szBuff);

        if ((strlen(s_szUnhookProcList) + strlen(szBuff) + 1 + 1) > sizeof(s_szUnhookProcList))
        {
            EIT_SYSLOG(LOG_DEBUG, "Whilelist process is to large , %s and following proce is discard", szBuff);
            return s_szUnhookProcList;
        }

        // discard line feed

        szTmp = strstr(szBuff, "\n");
        if (szTmp)
            *szTmp = 0;

        if ('#' == szBuff[0])
            continue;

        if (!strlen(szBuff))
            continue;
        if (!strcmp(szBuff, ""))
            continue;
        if (!strcmp(szBuff, "\n"))
            continue;
        if (!strcmp(szBuff, "\r"))
            continue;
        if (!strcmp(szBuff, "\r\n"))
            continue;
        if (!strcmp(szBuff, "\n\r"))
            continue;

        strcat(s_szUnhookProcList, szBuff);
        strcat(s_szUnhookProcList, "/");
    }

    if (feof(pf))
        EIT_SYSLOG(LOG_DEBUG, "End of file reached");
    else
        EIT_SYSLOG(LOG_DEBUG, "fgets() failed");

    fclose(pf);
    EIT_SYSLOG(LOG_DEBUG, "s_szUnhookProcList: %s", s_szUnhookProcList);

    EIT_SYSLOG(LOG_DEBUG, "Leave getUnhookProcList()");
    return s_szUnhookProcList;

}

static int IsUnhook(const char* szUnhookProcList, const char* szProc)
{
    char szTmp[NAME_MAX + 8] = {0};


    if (!szUnhookProcList)
        return 0;
    if (!szProc)
        return 0;
    if (!strlen(szUnhookProcList))
        return 0;
    if (!strlen(szProc))
        return 0;
    if (strlen(szUnhookProcList) < strlen(szProc))
        return 0;

    strcat(szTmp, "/");
    strcat(szTmp, szProc);
    strcat(szTmp, "/");

    EIT_SYSLOG(LOG_DEBUG, "szUnhookProcList: %s\nszTmp: %s", szUnhookProcList, szTmp);
    if (strstr(szUnhookProcList, szTmp))
        return 1;

    return 0;
}

bool IsHook()
{
    const char* szUnhookProcList = GetUnhookProcList();

    char task_name[1024] = {0};
    GetNameByPid(getpid(), task_name, 1024 - 1);

    int iUnhook = IsUnhook(szUnhookProcList, task_name);
    if (1 == iUnhook)
        EIT_SYSLOG(LOG_DEBUG, "\n\n%s proc can retrieve printers!", task_name);
    else
        EIT_SYSLOG(LOG_DEBUG, "\n\n%s proc can`t retrieve printers!", task_name);

    return (1 == iUnhook) ? false : true;
}
