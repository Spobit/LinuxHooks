#include "CupsHook.h"
#include "Global.h"
#include "DlsymHook.h"
#include "WhiteList.h"


ipp_state_t ippRead(http_t *http, ipp_t *ipp)
{
    EIT_SYSLOG(LOG_DEBUG, "Enter %s!", __FUNCTION__);
    HookPrinterInit();
    ipp_state_t state = kOrgIppRead(http, ipp);

    // ************** hook printer bigen ******************//
    if (kIsHook)
    {
        hook_ipp_packet(ipp);
    }
    // ************** hook printer end ********************//

    EIT_SYSLOG(LOG_DEBUG, "Laeve %s!", __FUNCTION__);
    return state;
}

ipp_state_t	ippReadFile(int fd, ipp_t *ipp)
{
    EIT_SYSLOG(LOG_DEBUG, "Enter %s!", __FUNCTION__);
    HookPrinterInit();
    ipp_state_t state = kOrgIppReadFile(fd, ipp);

    // ************** hook printer bigen ******************//
    if (kIsHook)
    {
        hook_ipp_packet(ipp);
    }
    // ************** hook printer end ********************//

    EIT_SYSLOG(LOG_DEBUG, "Laeve %s!", __FUNCTION__);
    return state;
}

ipp_state_t	ippReadIO(void *src, ipp_iocb_t cb, int blocking,
                      ipp_t *parent, ipp_t *ipp)
{
    EIT_SYSLOG(LOG_DEBUG, "Enter %s!", __FUNCTION__);
    HookPrinterInit();
    ipp_state_t state = kOrgIppReadIO(src, cb, blocking, parent, ipp);

    // ************** hook printer bigen ******************//
    if (kIsHook)
    {
        hook_ipp_packet(ipp);
    }
    // ************** hook printer end ********************//

    EIT_SYSLOG(LOG_DEBUG, "Laeve %s!", __FUNCTION__);
    return state;
}

ipp_t *cupsDoRequest(http_t *http, ipp_t *request, const char *resource)
{
    EIT_SYSLOG(LOG_DEBUG, "Enter %s!", __FUNCTION__);
    HookPrinterInit();

    //************
    ipp_attribute_t* attrs = request->attrs;
    while (attrs)
    {
        if (!strcmp(request->attrs->name, "job-name"))
        {
            if (request->attrs->values[0].string.text)
            {
                EIT_SYSLOG(LOG_INFO, "\t\t\t %s : %s",
                           request->attrs->name,
                           request->attrs->values[0].string.text);
                break;
            }
        }
        attrs = attrs->next;
    }
    //************

    // ************** hook printer bigen ******************//
    bool changed = false;
    if (kIsHook)
    {
        if (IPP_OP_CUPS_GET_PRINTERS == request->request.op.operation_id)
        {
            request->request.op.operation_id = IPP_OP_CUPS_GET_DEFAULT;
            changed = true;
        }
    }
    // ************** hook printer end ********************//

    ipp_t* ipp = kCupsDoRequest(http, request, resource);

    // ************** hook printer bigen ******************//
    if (kIsHook)
    {
        if (changed)
            request->request.op.operation_id = IPP_OP_CUPS_GET_PRINTERS;
    }
    // ************** hook printer end ********************//

    EIT_SYSLOG(LOG_DEBUG, "Laeve %s!", __FUNCTION__);
    return ipp;
}

int cupsGetDests(cups_dest_t **dests)
{
    EIT_SYSLOG(LOG_DEBUG, "Enter %s!", __FUNCTION__);
    HookPrinterInit();
    EIT_SYSLOG(LOG_DEBUG, "Leave %s!", __FUNCTION__);
    return cupsGetDests2(0, dests);
}

int cupsGetDests2(http_t *http, cups_dest_t **dests)
{
    EIT_SYSLOG(LOG_DEBUG, "Enter %s!", __FUNCTION__);
    HookPrinterInit();

    int num_dests = kOrgCupsGetDests2(http, dests);

    // ************** hook printer bigen ******************//
    if (kIsHook)
    {
        bool found_secprinter = false;
        for (int i = num_dests -1; i >=0 ; i--)
        {
            cups_dest_t* dest = *dests + i;
            if (0 == strcmp("SecPrinter", dest->name))
                found_secprinter = true;
            else
                num_dests = cupsRemoveDest(dest->name, dest->instance, num_dests, dests);
        }

        if (found_secprinter)
            num_dests = 1;
        else
            num_dests = 0;
    }
    // ************** hook printer end ********************//

    EIT_SYSLOG(LOG_DEBUG, "Leave %s!", __FUNCTION__);
    return num_dests;
}

void fl_ippFreeAttr(ipp_attribute_t *attr)
{
    int		i;/* Looping var */
    ipp_value_t	*value;/* Current value */

    switch (attr->value_tag)
    {
    case IPP_TAG_TEXT :
    case IPP_TAG_NAME :
    case IPP_TAG_KEYWORD :
    case IPP_TAG_URI :
    case IPP_TAG_URISCHEME :
    case IPP_TAG_CHARSET :
    case IPP_TAG_LANGUAGE :
    case IPP_TAG_MIMETYPE :
        for (i = 0, value = attr->values;
             i < attr->num_values;
             i ++, value ++)
            // _cupsStrFree(value->string.text); // crash
            break;

    case IPP_TAG_TEXTLANG :
    case IPP_TAG_NAMELANG :
        for (i = 0, value = attr->values;
             i < 1; //i < attr->num_values;
             i ++, value ++)
        {
            //            if (value->StringLanguage && i == 0) {
            //                // _cupsStrFree(value->StringLanguage);
            //            }
            //            // _cupsStrFree(value->string.text);
        }
        break;

    case IPP_TAG_INTEGER :
    case IPP_TAG_ENUM :
    case IPP_TAG_BOOLEAN :
    case IPP_TAG_DATE :
    case IPP_TAG_RESOLUTION :
    case IPP_TAG_RANGE :
        break;

    case IPP_TAG_BEGIN_COLLECTION :
        for (i = 0, value = attr->values;
             i < attr->num_values;
             i ++, value ++)
            ippDelete(value->collection);
        break;

    case IPP_TAG_STRING :
        for (i = 0, value = attr->values;
             i < attr->num_values;
             i ++, value ++)
            free(value->unknown.data);
        break;

    default :
        if (!((int)attr->value_tag & IPP_TAG_COPY))
        {
            for (i = 0, value = attr->values;
                 i < attr->num_values;
                 i ++, value ++)
                if (value->unknown.data)
                    free(value->unknown.data);
        }
        break;
    }
    if (attr->name) {
        // _cupsStrFree(attr->name);
    }
    free(attr);
}

void fl_ippDeleteAttribute(
        ipp_t           *ipp,/* I - IPP message */
        ipp_attribute_t *attr)/* I - Attribute to delete */
{
    ipp_attribute_t	*current,/* Current attribute */
            *prev;/* Previous attribute */


    for (current = ipp->attrs, prev = NULL;
         current != NULL && current != attr;
         prev = current, current = current->next);

    if (current) {

        if (prev)
            prev->next = current->next;
        else
            ipp->attrs = current->next;

        if (current == ipp->last)
            ipp->last = prev;

        fl_ippFreeAttr(current);
    }
}

void hook_ipp_packet(ipp_t *ipp)
{
    ipp_attribute_t  *pa = 0;
    ipp_attribute_t  *next = 0;
    ipp_tag_t         value_tag;
    ipp_value_t      *val = 0;
    int               num_values = 1;

    int  k = 0;
    int  find_my_printer = 0;

    ipp_tag_t  group_filter = IPP_TAG_PRINTER;


    for(pa=ipp->attrs; pa; pa=next)
    {
        next = pa->next;
        if(pa->group_tag != group_filter)
            continue;
        if(strcmp(pa->name, "printer-name") != 0)
            continue;

        num_values = pa->num_values;
        if(num_values > 1)
            num_values = 1;

        find_my_printer = 0;
        for(k=0; k<num_values; ++k)
        {
            value_tag = (ipp_tag_t)(pa->value_tag & IPP_TAG_MASK);
            val = &pa->values[k];
            if(!val)  continue;

            switch(value_tag)
            {

            case IPP_TAG_NAME:
                if(strcmp("SecPrinter", val->string.text) == 0)
                    find_my_printer = 1;
                break;

            default:
                break;
            }
        }

        if(!find_my_printer) {
            fl_ippDeleteAttribute(ipp, pa);
        }
    }
}

