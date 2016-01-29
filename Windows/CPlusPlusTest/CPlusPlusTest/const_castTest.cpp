#include "const_castTest.h"
#include <iostream>

void runConstCastTest()
{
	int a = 10;
	std::cout << a << std::endl;
	test(a);
	std::cout << a << std::endl;
}

void test(const int& a)
{
	const_cast<int&>(a) = 100;
}