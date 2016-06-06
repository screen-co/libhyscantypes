#ifndef __HYSCAN_API_H__
#define __HYSCAN_API_H__

#if defined (_WIN32)
  #if defined (HYSCAN_API_EXPORTS)
    #define HYSCAN_API __declspec (dllexport)
  #else
    #define HYSCAN_API __declspec (dllimport)
  #endif
#else
  #define HYSCAN_API
#endif

#endif /* __HYSCAN_API_H__ */
