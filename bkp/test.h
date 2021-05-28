#ifndef _TEST
#define _TEST
#include "ofMain.h"

template<typename T> class Test {
public:
	Test();
	T* page;
};
#endif;

template<typename T>
inline Test<T>::Test()
{
	page = new T();
}
