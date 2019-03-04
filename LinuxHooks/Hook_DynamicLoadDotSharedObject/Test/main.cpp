#include <unistd.h>
#include <stdio.h>
#include <dlfcn.h>
#include <map>
#include <string>


int main()
{
    printf("pid: %d\n", getpid());

    void* handle = dlopen("/home/eit/Documents/Qt/LinuxHooks/Hook_DynamicLoadDotSharedObject/bin/libOrgDlopenPos.so", RTLD_NOW);
    void* addr = dlsym(handle, "g_orgdlopen");

    return  0;
}

