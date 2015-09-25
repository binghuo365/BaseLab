#include <iostream>
#include "sizeofTest.h"

void runTest()
{
	std::cout << "struct S1 sizeof size :" << sizeof(s1) << std::endl;
	std::cout << "struct S1 sizeof size :" << sizeof(s2) << std::endl;
	std::cout << "class c1 sizeof size :" << sizeof(c1) << std::endl;
	std::cout << "class c2 sizeof size :" << sizeof(c2) << std::endl;
	std::cout << "class c3 sizeof size :" << sizeof(c3) << std::endl;
	std::cout << "class c4 sizeof size :" << sizeof(c4) << std::endl;
	std::cout << "class c5 sizeof size :" << sizeof(c5) << std::endl;
	std::cout << "class c6 sizeof size :" << sizeof(c6) << std::endl;
}