#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <syslog.h>

#include <cups/cups.h>

#ifdef __cplusplus
#   define EXTERNC extern "C"
#else
#   define EXTERNC
#endif




typedef ipp_state_t	(*IppRead)(http_t *http, ipp_t *ipp);
typedef ipp_state_t	(*IppReadFile)(int fd, ipp_t *ipp);
typedef ipp_state_t	(*IppReadIO)(void *src, ipp_iocb_t cb, int blocking,
                      ipp_t *parent, ipp_t *ipp);
typedef ipp_t* (*CupsDoRequest)(http_t *http, ipp_t *request, const char *resource);
typedef int (*CupsGetDests2)(http_t *http, cups_dest_t **dests);
typedef void* (*Dlsym)(void * __restrict handle, const char * __restrict symbol);


///> cups full name hook
typedef int (*CupsCreateJob)(
        http_t        *http,		/* I - Connection to server or @code CUPS_HTTP_DEFAULT@ */
        const char    *name,		/* I - Destination name */
        const char    *title,		/* I - Title of job */
        int           num_options,	/* I - Number of options */
        cups_option_t *options)		/* I - Options */;

typedef ipp_status_t				/* O - IPP status code */
(*CupsCreateDestJob)(
        http_t        *http,		/* I - Connection to destination */
        cups_dest_t   *dest,		/* I - Destination */
        cups_dinfo_t  *info, 		/* I - Destination information */
        int           *job_id,		/* O - Job ID or 0 on error */
        const char    *title,		/* I - Job name */
        int           num_options,		/* I - Number of job options */
        cups_option_t *options)		/* I - Job options */;

typedef int                             /* O - Job ID or 0 on error */
(*CupsPrintFiles)(
        const char    *name,		/* I - Destination name */
        int           num_files,	/* I - Number of files */
        const char    **files,		/* I - File(s) to print */
        const char    *title,		/* I - Title of job */
        int           num_options,	/* I - Number of options */
        cups_option_t *options)		/* I - Options */;

typedef int
(*CupsPrintFiles2)(
        http_t        *http,		/* I - Connection to server or @code CUPS_HTTP_DEFAULT@ */
        const char    *name,		/* I - Destination name */
        int           num_files,	/* I - Number of files */
        const char    **files,		/* I - File(s) to print */
        const char    *title,		/* I - Title of job */
        int           num_options,	/* I - Number of options */
        cups_option_t *options)     /* I - Options */;

typedef int
(*CupsPrintFile)(
        const char *name,
        const char *filename,
        const char *title,
        int num_options,
        cups_option_t *options);

typedef int
(*CupsPrintFile2)(
        http_t *http,
        const char *name,
        const char *filename,
        const char *title,
        int num_options,
        cups_option_t *options) _CUPS_API_1_1_21;

typedef http_status_t
(*CupsStartDocument)(
        http_t *http,
        const char *name,
        int job_id,
        const char *docname,
        const char *format,
        int last_document) _CUPS_API_1_4;

typedef http_status_t
(*CupsStartDestDocument)(
        http_t *http,
        cups_dest_t *dest,
        cups_dinfo_t *info,
        int job_id,
        const char *docname,
        const char *format,
        int num_options,
        cups_option_t *options,
        int last_document) _CUPS_API_1_6;

typedef ipp_attribute_t *			/* O - New attribute */
(*IppAddString)(ipp_t      *ipp,		/* I - IPP message */
             ipp_tag_t  group,		/* I - IPP group */
             ipp_tag_t  value_tag,	/* I - Type of attribute */
             const char *name,		/* I - Name of attribute */
             const char *language,	/* I - Language code */
             const char *value)		/* I - Value */;

#endif // TYPE_H
