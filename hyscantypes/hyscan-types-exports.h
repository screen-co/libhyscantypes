#ifndef __HYSCAN_TYPES_EXPORTS_H__
#define __HYSCAN_TYPES_EXPORTS_H__

#if defined (_WIN32)
  #if defined (hyscantypes_EXPORTS)
    #define HYSCAN_TYPES_EXPORT __declspec (dllexport)
  #else
    #define HYSCAN_TYPES_EXPORT __declspec (dllimport)
  #endif
#else
  #define HYSCAN_TYPES_EXPORT
#endif

#endif /* __HYSCAN_TYPES_EXPORTS_H__ */
