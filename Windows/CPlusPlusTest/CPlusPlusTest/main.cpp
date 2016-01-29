#include <iostream>
#include "sizeofTest.h"
#include "virtualFunction.h"
#include "throwTest.h"
#include "overFlowTest.h"
#include "const_castTest.h"

void test(const int& a);

int main()
{
	//sizeofTest
	//runTest();

	//VirtualFunction Test
	//runVirtualFunctionTest();

	//throwTest test;
	//runThrowTest();

	//runOverFloweTest();

	runConstCastTest();

	system("pause");
}
