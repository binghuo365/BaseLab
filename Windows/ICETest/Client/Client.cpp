#include <Ice/Ice.h>
#include "../slice/print.h"

using namespace std;
using namespace Demo;

int main(int argc, char** argv)
{
	Ice::CommunicatorPtr ic;
	//建立通信器
	ic = Ice::initialize(argc, argv);
	//获得Ice对象代理，SimplePrinter-对象标识符，default -p 10000-协议与端口
	Ice::ObjectPrx base = ic->stringToProxy("SimplePrinter:default -p 10000");
	//向下转换
	PrinterPrx printer = PrinterPrx::checkedCast(base);
	if (!printer) throw "Invalid proxy";
	//调用操作
	printer->printString("Hello World!");
}
