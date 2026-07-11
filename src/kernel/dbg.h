
#ifndef _DBG_H
#define _DBG_H

#ifdef DEBUG
#include <drivers/kprint.h>
#define LOG(...) kprint(__VA_ARGS__)
#else
#define LOG(...)
#endif

#endif
