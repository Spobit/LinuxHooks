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

#endif // TYPE_H
