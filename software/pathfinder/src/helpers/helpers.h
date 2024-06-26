#include <string>
#include <sstream>

std::string decToBinary(int n);

// REMOVE SUPER ANNOYING C++ ALTERA IFDENTIFIER NOT FOUND ISSUE
extern int __builtin_ldwio(const volatile void* src);
extern void __builtin_stwio(volatile void* dst, int data);
extern int __builtin_custom_inii(int a, int b, int c);

int* cpyArray(int *src, int size);