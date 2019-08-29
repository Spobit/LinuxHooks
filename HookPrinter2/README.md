# `HookPrinter`

This project have two ability:

  1. the hook of getting printers (implemented)
  2. the hook of setting full path of print file (unimplemented)

## `Maintenance`

  1. `DEFINES += _OFFSET_FROM_DLERROR_TO_DLSYM=0x280`
     the offset from dlerror to dlsym need maintenance.
  2. `SetDlsymByPltMethod()`
     This function is only works with arm v8 x64.
  3. `IsDlerrorPlt()`
     You should keep it right.
  4. `__attribute__ ((constructor)) void HookPrinterInit(void)`
     HookPrinterInit() maybe run after hook function, so `__attribute__ ((constructor))` is a problem, ensure running init function at the start of hook function.
  5. `__attribute__ ((constructor)) void HookPrinterInit(void)`
     fgets() will be closed loop. why?


## `The knowledge about hook`

If There have the case, someone library have the export funcions 'GetPrinter()' and 'GetPrinters()', 'GetPrinters()' invokes 'GetPrinter()' to implement its ability.
If just hook 'GetPrinter()', it can not prevent 'GetPrinters()', 'GetPrinters()' will invoke 'GetPrinter()' what is real but not we hooked.

I call it as ability point, who like as 'GetPrinter()'.
I call it as escape point, who like as 'GetPrinters()'.

## `The hook of getting printers`

The implemention thinking is that set our printer 'SecPrinter' to default printer.

We hook 'cupsDoRequest()', the upper level application will invoke the interface to get all printers or printer specified.

We hook 'ippRead()', the upper level application will invoke the interface to get printer infos.

## `The hook of setting full path of print file`

ability point: 'cupsDoRequest()' 'ippAddString()'.

```
ipp_attribute_t *ippAddString(ipp_t *ipp, ipp_tag_t group, ipp_tag_t value_tag, const char *name, const char *language, const char *value);
```

If the 'name' is the string 'job-name', the 'value' will be full path of print file.

```
ipp_t* cupsDoRequest(http_t *http, ipp_t *request, const char *resource);
```

'request->attrs' is a linked list, every element have 'name' and 'value', if the 'name' is the string 'job-name', the 'value' will be full path of print file.

## `The plt method for arm v8 x64`

> Arm® Architecture Reference Manual  
> &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;Armv8, for Armv8-A architecture profile

```
#include <dlfcn.h>


typedef void* (*Dlsym)(void*, const char*);
typedef char* (*Dlerror)(void);

/*
0x400590 <dlsym@plt>:   0x90    0x00    0x00    0xb0
0x400594 <dlsym@plt+4>: 0x11    0x0a    0x40    0xf9
*/

unsigned int AdrpOffset(unsigned int*  plt)
{
	unsigned int ret = 0;
	ret += (*plt & 0xffffe0) >> 3;
	ret += (*plt & 0x60000000) >> 29;
	return ret << 12;
}

unsigned long long AddrBase(unsigned int*  plt)
{
	unsigned long long base = (unsigned long long)plt;
	return (base + 4) >> 12 << 12;
}

unsigned long long X16(unsigned long long base, unsigned long long offset)
{
	return base + offset;
}

unsigned int LdrOffset(unsigned int*  plt)
{
	unsigned int* plt4 = plt + 1;
	unsigned int ret = 0;
	return (*plt4 << 10 >> 20) * 8;
}

void* x17(unsigned long long x16, unsigned int ldrOffset)
{
	return *(void**)(x16 + ldrOffset);
}

void* PltFunAddr(unsigned int*  plt)
{
	unsigned int adrpOffset = AdrpOffset(plt);
	unsigned long long addrBase = AddrBase(plt);
	unsigned long long x16 = X16(addrBase, adrpOffset);
	unsigned int ldrOffset = LdrOffset(plt);
	return x17(x16, ldrOffset);
}

int main()
{
	void* plt = 0;
	unsigned int offset = 0;
	void* addr = 0;

	dlsym(0, "dlerror");
	dlsym(0, "dlerror");
	dlsym(0, "dlerror");
	Dlsym pDlsym = (Dlsym)PltFunAddr((unsigned int*)dlsym);
	pDlsym(0, "dlerror");
	
	dlerror();
	dlerror();
	dlerror();
	Dlerror pDlerror = (Dlerror)PltFunAddr((unsigned int*)dlerror);
	pDlerror();

	dladdr((void*)0, (void*)0);
	dladdr((void*)0, (void*)0);
	dladdr((void*)0, (void*)0);

	return 0;
}
```
