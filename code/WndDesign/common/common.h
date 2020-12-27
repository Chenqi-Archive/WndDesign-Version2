#pragma once

#ifdef _USRDLL
#ifdef WNDDESIGNCORE_EXPORTS
#define WNDDESIGNCORE_API __declspec(dllexport)		// For exporting to a dynamic library
#else
#define WNDDESIGNCORE_API __declspec(dllimport)		// For importing from a dynamic library
#endif
#else
#define WNDDESIGNCORE_API							// For static library
#endif


#include <stdexcept>
#include <cassert>


#define BEGIN_NAMESPACE(name) namespace name {
#define END_NAMESPACE(name)   }

#define Anonymous
#define pure = 0


BEGIN_NAMESPACE(WndDesign)


template<class T>
using ref_ptr = T*;   // A tag for raw pointer that is used only as a reference.


using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using wchar = wchar_t;


template<class T>
inline const T max(T a, T b) { return a > b ? a : b; }
template<class T>
inline const T min(T a, T b) { return a < b ? a : b; }


END_NAMESPACE(WndDesign)