#ifndef LOGPRINT_H
#define LOGPRINT_H

#include <android/log.h>

#define ENABLE_DEBUG 0

#if ENABLE_DEBUG
	#define DEBUG_PRINT(format,args...) \
		__android_log_print(ANDROID_LOG_INFO,"monoInject", format, ##args)
#else
	#define DEBUG_PRINT(format,args...)
#endif

#endif