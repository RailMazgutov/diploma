// SERVER.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "opc_ua.h"
#include <iostream>

int main()
{
	set_endpoint("opc.tcp://localhost:4856/opcua/server");
	set_server_uri("urn://eniko.ru");
	// Надо задать большим в Properties -> Linker -> System -> Stack Reserve Size
	start_server();
	int stop;
	while(true)
	{		
		std::cin >> stop;
		if (stop == 0)
			break;
	}
	stop_server();
	return 0;
}

