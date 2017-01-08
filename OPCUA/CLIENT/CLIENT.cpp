// CLIENT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "client.h"
#include <chrono>
#include <thread>

int main()
{
	connect_to_server("opc.tcp://localhost:4856/opcua/server");
	get_main_node();
	std::this_thread::sleep_for(std::chrono::seconds(25));
	disconnect_from_server();
	return 0;
}

