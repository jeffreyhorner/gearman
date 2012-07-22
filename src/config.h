#include <R.h>
#define PACKAGE_VERSION "0.33"
#if defined WIN32 
#include "win_config.h"

#elif defined __APPLE__ 
#include "osx_config.h"

#else
#include "unix_config.h"
#endif
