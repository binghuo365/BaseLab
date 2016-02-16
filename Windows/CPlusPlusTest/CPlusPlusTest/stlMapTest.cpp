#include "stlMapTest.h"
#include <map>
#include <string>
#include <time.h>

stlMapTest::stlMapTest()
{

}

stlMapTest::~stlMapTest()
{

}

void runStlMapTest()
{
	std::map<int, std::string> testMap;
	int i = 100;
	srand((unsigned)time(NULL));
	while (--i)
	{
		testMap[rand() % 100000] = "hello test";
	}

	for (std::map<int, std::string>::iterator it = testMap.begin(); it != testMap.end(); ++it)
	{
		std::cout << it->first << std::endl;
	}
}
