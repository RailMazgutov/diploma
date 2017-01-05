// SERVER.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "server.h"

int main()
{
	set_endpoint("opc.tcp://localhost:4856/opcua/server");
	set_server_uri("urn://eniko.ru");
	size_t namespaces[4];
	size_t nodes_core_1[10];
	size_t nodes_core_2[10];

	// Надо задать большим в Properties -> Linker -> System -> Stack Reserve Size
	start_server();
	alloc_nodes(30);
	namespaces[0] = create_namespace("VVER1000_1");
	namespaces[1] = create_namespace("VVER1000_1");
	namespaces[2] = create_namespace("Service");
	namespaces[3] = create_namespace("Meta");

	OPCUA_Node main_parameters;
	main_parameters.namespace_id = namespaces[0];
	main_parameters.has_parent_node = false;
	nodes_core_1[0] = add_node(main_parameters, "INFO");

	main_parameters.namespace_id = namespaces[1];
	nodes_core_2[0] = add_node(main_parameters, "INFO");

	while(true)
	{		
		
	}
	stop_server();
	return 0;
}


