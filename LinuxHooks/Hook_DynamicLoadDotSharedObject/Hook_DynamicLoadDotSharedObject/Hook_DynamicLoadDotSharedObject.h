#ifndef HOOK_DYNAMICLOADDOTSHAREDOBJECT_H
#define HOOK_DYNAMICLOADDOTSHAREDOBJECT_H

#include <string>
#include <map>


///> get dl.so pos
// The \r value 0 indicate no error; otherwise, error occurred, the error reason
// is written to the first string of \a libs.
__attribute__ ((visibility("hidden")))
int get_load_libs_string_in_maps(std::map<std::string, int>& libs);

__attribute__ ((visibility("hidden")))
void get_expected_libs_strings(std::map<std::string, int>& libs);

// The \r value 0 indicate error;
__attribute__ ((visibility("hidden")))
unsigned long long
get_offset_from_dlopen_to_dlsym_in_libs(std::map<std::string, int>& libs);

__attribute__ ((visibility("hidden")))
unsigned long long get_offset_from_dlopen_to_dlsym(const std::string& lib);


///> hook
extern "C"
void *dlsym (void *__restrict __handle, const char *__restrict __name)
__THROW __nonnull ((2));

#endif // HOOK_DYNAMICLOADDOTSHAREDOBJECT_H
