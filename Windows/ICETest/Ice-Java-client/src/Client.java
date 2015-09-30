public class Client {
    public static void main(String[] args) {
        int status = 0;
        Ice.Communicator ic = null;
        try {
            //初使化
            ic = Ice.Util.initialize(args);
            //传入远程服务单元的名称、网络协议、IP及端口，获取Printer的远程代理，这里使用的stringToProxy方式
            Ice.ObjectPrx base = ic.stringToProxy("SimplePrinter:default -p 10000");
            //通过checkedCast向下转换，获取Printer接口的远程，并同时检测根据传入的名称获取的服务单元是否Printer的代理接口，如果不是则返回null对象
            Demo.PrinterPrx printer = Demo.PrinterPrxHelper.checkedCast(base);
            if (printer == null) throw new Error("Invalid proxy");
            //把Hello World传给服务端，让服务端打印出来，因为这个方法最终会在服务端上执行
            printer.printString("Hello World!");
        } catch (Ice.LocalException e) {
            e.printStackTrace();
            status = 1;
        } catch (Exception e) {
            System.err.println(e.getMessage());
            status = 1;
        }
        if (ic != null) {
            // Clean up
            //
            try {
                ic.destroy();
            } catch (Exception e) {
                System.err.println(e.getMessage());
                status = 1;
            }
        }
        System.exit(status);
    }
}