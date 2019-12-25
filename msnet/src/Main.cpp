#include <iostream>
#include <memory>
#include "SrvTcp.h"


int main(int argc, char* argv[])
{
	std::cout << "Hello World Asio\n";
	//TcpServer pServer(33000, 4);
	try
	{
		//pServer.run();
		asio::io_context ioContext;
		TcpServer server(ioContext, 33000);
		ioContext.run();
	}
	catch (std::exception* e)
	{
	}
	for(;;) {

	}
}
