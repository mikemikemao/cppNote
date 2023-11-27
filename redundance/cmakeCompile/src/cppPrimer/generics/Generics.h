#ifndef GENERICS_H
#define GENERICS_H

#include <cstring>
#include <functional>
using std::less; using std::greater;

template <typename T, size_t N>
void print(T (&arr)[N])
{
    for (auto elem : arr)
        cout << elem << endl;
}

// template <typename T>
// int compare(const T &v1, const T &v2)
// {
//     if (v1 < v2) return -1;
//     if (v2 < v1) return 1;
//     return 0;
// }

template <typename T, typename F = less<T>> 
int compare(const T &v1, const T &v2, F f = F()) 
{
	if (f(v1, v2)) return -1;
	if (f(v2, v1)) return 1;  
	return 0;
}

#endif