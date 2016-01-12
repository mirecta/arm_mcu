
#include <stdarg.h>

#ifndef _EE_PRINTF_

#define _EE_PRINTF_


#ifdef __cplusplus
extern "C" {
#endif

int ee_vsprintf(char *buf, const char *fmt, va_list args);

#ifdef __cplusplus
}
#endif

#endif //_EE_PRINTF_

