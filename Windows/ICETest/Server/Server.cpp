#include "PrintI.h"

int main(int argc, char** argv)
{
	Ice::CommunicatorPtr ic;

	//建立通信器
	ic = Ice::initialize(argc, argv);

	//建立对象适配器，绑定端口
	Ice::ObjectAdapterPtr adapter = ic->createObjectAdapterWithEndpoints(
		"simplePrinterAdapter", "default -p 10000");

	Ice::ObjectPtr object = new PrinterI;

	//加入Ice对象实例，指定标识符
	adapter->add(object, ic->stringToIdentity("SimplePrinter"));

	//启动适配器
	adapter->activate();

	//挂起该进程
	ic->waitForShutdown();
}