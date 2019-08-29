#ifndef CUPSFULLNAMEHOOK_H
#define CUPSFULLNAMEHOOK_H

#include <cups/cups.h>


extern "C" int                             /* O - Job ID or 0 on error */
cupsCreateJob(
        http_t        *http,		/* I - Connection to server or @code CUPS_HTTP_DEFAULT@ */
        const char    *name,		/* I - Destination name */
        const char    *title,		/* I - Title of job */
        int           num_options,	/* I - Number of options */
        cups_option_t *options)		/* I - Options */;

extern "C" ipp_status_t				/* O - IPP status code */
cupsCreateDestJob(
        http_t        *http,		/* I - Connection to destination */
        cups_dest_t   *dest,		/* I - Destination */
        cups_dinfo_t  *info, 		/* I - Destination information */
        int           *job_id,		/* O - Job ID or 0 on error */
        const char    *title,		/* I - Job name */
        int           num_options,		/* I - Number of job options */
        cups_option_t *options)		/* I - Job options */;

extern "C" int
cupsPrintFile(
        const char *name,
        const char *filename,
        const char *title,
        int num_options,
        cups_option_t *options);

extern "C" int
cupsPrintFile2(
        http_t *http,
        const char *name,
        const char *filename,
        const char *title,
        int num_options,
        cups_option_t *options) _CUPS_API_1_1_21;

extern "C" int                             /* O - Job ID or 0 on error */
cupsPrintFiles(
        const char    *name,		/* I - Destination name */
        int           num_files,	/* I - Number of files */
        const char    **files,		/* I - File(s) to print */
        const char    *title,		/* I - Title of job */
        int           num_options,	/* I - Number of options */
        cups_option_t *options)		/* I - Options */;

extern "C"
int cupsPrintFiles2(
        http_t        *http,		/* I - Connection to server or @code CUPS_HTTP_DEFAULT@ */
        const char    *name,		/* I - Destination name */
        int           num_files,	/* I - Number of files */
        const char    **files,		/* I - File(s) to print */
        const char    *title,		/* I - Title of job */
        int           num_options,	/* I - Number of options */
        cups_option_t *options)     /* I - Options */;


typedef http_status_t
(*CupsStartDocument)(
        http_t *http,
        const char *name,
        int job_id,
        const char *docname,
        const char *format,
        int last_document) _CUPS_API_1_4;

extern "C" http_status_t
cupsStartDocument(
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



extern "C" http_status_t
cupsStartDestDocument(
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

extern "C" ipp_attribute_t *			/* O - New attribute */
ippAddString(ipp_t      *ipp,		/* I - IPP message */
             ipp_tag_t  group,		/* I - IPP group */
             ipp_tag_t  value_tag,	/* I - Type of attribute */
             const char *name,		/* I - Name of attribute */
             const char *language,	/* I - Language code */
             const char *value)		/* I - Value */;

#endif // CUPSFULLNAMEHOOK_H
