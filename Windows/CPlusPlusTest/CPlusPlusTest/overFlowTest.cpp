#include "overFlowTest.h"
#include <iostream>

void runOverFloweTest()
{
	long long j = 50 * 365 * 24 * 3600 * 1000;
	int i = j;
	i = 2147483647;
	i = i + 1000 * 3600 * 24 * 10;
	std::cout << i << std::endl;
}