#include <iostream>
#include "sizeofTest.h"
#include "virtualFunction.h"
#include "throwTest.h"
#include "overFlowTest.h"
#include "const_castTest.h"
#include "stlMapTest.h"

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

	//runConstCastTest();

	runStlMapTest();

	system("pause");
}
