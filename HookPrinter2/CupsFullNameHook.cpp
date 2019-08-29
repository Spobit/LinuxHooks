#include "CupsFullNameHook.h"

#include <Global.h>


int                             /* O - Job ID or 0 on error */
cupsCreateJob(
        http_t        *http,		/* I - Connection to server or @code CUPS_HTTP_DEFAULT@ */
        const char    *name,		/* I - Destination name */
        const char    *title,		/* I - Title of job */
        int           num_options,	/* I - Number of options */
        cups_option_t *options)		/* I - Options */
{
    EIT_SYSLOG(LOG_INFO, "Enter %s", __FUNCTION__);
    HookPrinterInit();
    return kCupsCreateJob(http, name, title, num_options, options);
}

int                             /* O - Job ID or 0 on error */
cupsPrintFiles(
        const char    *name,		/* I - Destination name */
        int           num_files,	/* I - Number of files */
        const char    **files,		/* I - File(s) to print */
        const char    *title,		/* I - Title of job */
        int           num_options,	/* I - Number of options */
        cups_option_t *options)		/* I - Options */
{
    EIT_SYSLOG(LOG_INFO, "Enter %s", __FUNCTION__);
    HookPrinterInit();
    return kCupsPrintFiles(name, num_files, files, title, num_options, options);
}

int cupsPrintFiles2(
        http_t        *http,		/* I - Connection to server or @code CUPS_HTTP_DEFAULT@ */
        const char    *name,		/* I - Destination name */
        int           num_files,	/* I - Number of files */
        const char    **files,		/* I - File(s) to print */
        const char    *title,		/* I - Title of job */
        int           num_options,	/* I - Number of options */
        cups_option_t *options)     /* I - Options */
{
    EIT_SYSLOG(LOG_INFO, "Enter %s", __FUNCTION__);
    HookPrinterInit();
    return kCupsPrintFiles2(http, name, num_files, files, title, num_options, options);
}

int cupsPrintFile(const char *name, const char *filename, const char *title, int num_options, cups_option_t *options)
{
    EIT_SYSLOG(LOG_INFO, "Enter %s", __FUNCTION__);
    HookPrinterInit();
    return kCupsPrintFile(name, filename, title, num_options, options);
}

int cupsPrintFile2(http_t *http, const char *name, const char *filename, const char *title, int num_options, cups_option_t *options)
{
    EIT_SYSLOG(LOG_INFO, "Enter %s", __FUNCTION__);
    HookPrinterInit();
    return kCupsPrintFile2(http, name, filename, title, num_options, options);
}

ipp_status_t cupsCreateDestJob(http_t *http, cups_dest_t *dest, cups_dinfo_t *info, int *job_id, const char *title, int num_options, cups_option_t *options)
{
    EIT_SYSLOG(LOG_INFO, "Enter %s", __FUNCTION__);
    HookPrinterInit();
    return kCupsCreateDestJob(http, dest, info, job_id, title, num_options, options);
}

http_status_t cupsStartDocument(http_t *http, const char *name, int job_id, const char *docname, const char *format, int last_document)
{
    EIT_SYSLOG(LOG_INFO, "Enter %s", __FUNCTION__);
    HookPrinterInit();
    return kCupsStartDocument(http, name, job_id, docname, format, last_document);
}

http_status_t cupsStartDestDocument(http_t *http, cups_dest_t *dest, cups_dinfo_t *info, int job_id, const char *docname, const char *format, int num_options, cups_option_t *options, int last_document)
{
    EIT_SYSLOG(LOG_INFO, "Enter %s", __FUNCTION__);
    HookPrinterInit();
    return kCupsStartDestDocument(http, dest, info, job_id, docname, format, num_options, options, last_document);
}

ipp_attribute_t *ippAddString(ipp_t *ipp, ipp_tag_t group, ipp_tag_t value_tag, const char *name, const char *language, const char *value)
{
    // qt:  *(int*)name == 0x2d626f6a ==> name == "job-name"
    // gdb: if 0==strcmp(name,"job-name")
    EIT_SYSLOG(LOG_INFO, "Enter %s", __FUNCTION__);
    HookPrinterInit();
    if (value && name)
        EIT_SYSLOG(LOG_INFO, "\t\t\t %s : %s", name, value);
    return kIppAddString(ipp, group, value_tag, name, language, value);
}
