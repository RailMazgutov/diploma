// SERVER.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "server.h"
#include <chrono>
#include <thread>
#include <iostream>

void callback_fcn(const OPCUA_Variable* variable)
{
	std::cout << "WORKS!!! for var: " << variable->id << std::endl;
}

int main()
{
	set_endpoint("opc.tcp://localhost:4856/opcua/server");
	set_server_uri("urn://eniko.ru");
	size_t namespaces[4];
	size_t nodes_core_1[10];
	size_t nodes_core_2[10];

	// Надо задать большим в Properties -> Linker -> System -> Stack Reserve Size
	start_server();
	subscribe_datachange(callback_fcn);
	alloc_nodes(30);
	namespaces[0] = create_namespace("VVER1000_1");
	namespaces[1] = create_namespace("VVER1000_1");
	namespaces[2] = create_namespace("Service");
	namespaces[3] = create_namespace("Meta");


	OPCUA_Node main_parameters;
	main_parameters.namespace_id = namespaces[0];
	main_parameters.has_parent_node = false;
	nodes_core_1[0] = add_node(main_parameters, "INFO");

	OPCUA_Variable variable;
	variable.namespace_id = namespaces[0];
	variable.parent_id = nodes_core_1[0];
	//variable.name= "TEST";
	variable.data.type = OPCUA_INT32;
	variable.data.value.int32_val = 0;
	add_variable(&variable, "TEST");
	
	main_parameters.namespace_id = namespaces[1];
	nodes_core_2[0] = add_node(main_parameters, "INFO");

	OPCUA_Variable variable2;
	variable2.namespace_id = namespaces[1];
	variable2.parent_id = nodes_core_2[0];
	variable2.data.type = OPCUA_INT16;
	variable2.data.value.int16_val = 0;
	add_variable(&variable2, "TEST");

	OPCUA_Variable variable3;
	variable3.namespace_id = namespaces[1];
	variable3.parent_id = nodes_core_2[0];
	variable3.data.type = OPCUA_BOOLEAN;
	variable3.data.value.bool_val = true;
	add_variable(&variable3, "TEST_BOOL");

	
	int val = 1;
	variable.data.value.int32_val = val;
	variable2.data.value.int16_val = val;
	variable3.data.value.bool_val = val % 2 == 0;
	while(true)
	{		
		set_variable_value_int32(&variable);
		set_variable_value_boolean(&variable3);
		variable.data.value.int32_val = val;
		if (val % 2 == 0) {
			variable2.data.value.int16_val = val / 2;
			set_variable_value_int32(&variable2);
		}
		++val;
		variable3.data.value.bool_val = val % 2 == 0;
		std::this_thread::sleep_for(std::chrono::milliseconds(20000));
	}
	stop_server();
	return 0;
}


