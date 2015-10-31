#include "throwTest.h"


throwTest::throwTest()
{
}

throwTest::~throwTest()
{
}

int runThrowTest()
{
	try
	{
		throw std::exception();
	}
	catch (...)
	{
		std::cout << "catch throw" << std::endl;
	}

	std::cout << "continue run main" << std::endl;

	if (1)
	{
		try
		{
			throw std::exception();
		}
		catch (...)
		{
			std::cout << "catch throw in inner" << std::endl;
		}
		std::cout << "continue run inner" << std::endl;
	}
	std::cout << "continue run main" << std::endl;
}

