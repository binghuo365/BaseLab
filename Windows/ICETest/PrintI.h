#include <Ice/Ice.h>
#include "print.h"

using namespace std;
using namespace Demo;

class PrinterI :public Printer{
public:
	virtual void printString(const string& s, const Ice::Current&);
};

void PrinterI::printString(const string& s, const Ice::Current&)
{
	cout << s << endl;
}
