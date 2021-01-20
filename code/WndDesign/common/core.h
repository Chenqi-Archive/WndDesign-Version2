#pragma once

#ifdef WNDDESIGNCORE_EXPORTS							 // For library
	#ifdef _USRDLL
		#define WNDDESIGNCORE_API __declspec(dllexport)		// For dynamic library
	#else
		#define WNDDESIGNCORE_API							// For static library
	#endif
#else
	#define WNDDESIGNCORE_API __declspec(dllimport)		 // For user program
#endif


#define __ToString(name) #name
#define _ToString(name) __ToString(name)
#define Remark	__FILE__ "(" _ToString(__LINE__) "): [" __FUNCTION__ "] Remark: "


#define BEGIN_NAMESPACE(name) namespace name {
#define END_NAMESPACE(name)   }
#define Anonymous

#define ABSTRACT_BASE _declspec(novtable)
#define pure = 0


#include <stdexcept>
#include <cassert>


BEGIN_NAMESPACE(WndDesign)


template<class T>
using ref_ptr = T*;   // A tag for raw pointer that is used only as a reference.

template<class T>
using alloc_ptr = T*;   // [BE CAREFUL] A tag for raw pointer that is allocated by operator new.
						// It is recommended that you use a smart pointer like unique_ptr or shared_ptr, etc.


using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using wchar = wchar_t;


template<class T>
inline const T max(T a, T b) { return a > b ? a : b; }
template<class T>
inline const T min(T a, T b) { return a < b ? a : b; }


END_NAMESPACE(WndDesign)