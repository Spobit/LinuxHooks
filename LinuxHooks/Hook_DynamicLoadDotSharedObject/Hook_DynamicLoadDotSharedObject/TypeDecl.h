#ifndef TYPEDECL
#define TYPEDECL

typedef void* (*DLOPEN)(const char *filename, int flags);
typedef void* (*DLSYM)(void *handle, const char *symbol);

#endif // TYPEDECL

