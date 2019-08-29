#ifndef CUPSHOOK_H
#define CUPSHOOK_H

#include <cups/cups.h>
#include "Type.h"

// ipp
EXTERNC ipp_state_t ippRead(http_t *http, ipp_t *ipp);
EXTERNC ipp_state_t	ippReadFile(int fd, ipp_t *ipp);
EXTERNC ipp_state_t	ippReadIO(void *src, ipp_iocb_t cb, int blocking,
                              ipp_t *parent, ipp_t *ipp);

// ipp python3.5 system-config-printer
EXTERNC ipp_t* cupsDoRequest(http_t *http, ipp_t *request, const char *resource);

// cups
EXTERNC int cupsGetDests(cups_dest_t **dests);
EXTERNC int cupsGetDests2(http_t *http, cups_dest_t **dests);
__attribute__((visibility("hidden"))) void hook_ipp_packet(ipp_t *ipp);
__attribute__((visibility("hidden"))) void fl_ippDeleteAttribute( ipp_t *ipp, ipp_attribute_t *attr);
__attribute__((visibility("hidden"))) void fl_ippFreeAttr(ipp_attribute_t *attr);

#endif // CUPSHOOK_H
