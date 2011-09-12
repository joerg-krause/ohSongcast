
#include <mach/mach_types.h>
#include "Branding.h"

// This file is largely generated by Xcode but has been copied so that the driver can be built with make.
 
extern kern_return_t _start(kmod_info_t *ki, void *data);
extern kern_return_t _stop(kmod_info_t *ki, void *data);
 
__attribute__((visibility("default"))) KMOD_EXPLICIT_DECL(BRANDING_KEXTINFO_NAME, BRANDING_KEXTINFO_VERSION, _start, _stop)
__private_extern__ kmod_start_func_t *_realmain = 0;
__private_extern__ kmod_stop_func_t *_antimain = 0;
__private_extern__ int _kext_apple_cc = __APPLE_CC__ ;

