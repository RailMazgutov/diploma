// SERVER.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "server.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>

void callback_fcn(const OPCUA_Variable* variable)
{
	std::cout << "Variable id: " << variable->id << std::endl;
	std::cout << "Changed value, new value: ";
	switch (variable->data.type)
	{
	case(OPCUA_BOOLEAN): {
		std::cout << variable->data.value.bool_val << " - type BOOLEAN;" << std::endl;
		break;
	}
	case(OPCUA_BYTE): {
		std::cout << variable->data.value.byte_val << " - type BYTE;" << std::endl;
		break;
	}
	case(OPCUA_DOUBLE): {
		std::cout << variable->data.value.double_val << " - type DOUBLE;" << std::endl;
		break;
	}
	case(OPCUA_FLOAT): {
		std::cout << variable->data.value.float_val << " - type FLOAT;" << std::endl;
		break;
	}
	case(OPCUA_INT16): {
		std::cout << variable->data.value.int16_val << " - type INT16;" << std::endl;
		break;
	}
	case(OPCUA_INT32): {
		std::cout << variable->data.value.int32_val << " - type INT32;" << std::endl;
		break;
	}
	case(OPCUA_INT64): {
		std::cout << variable->data.value.int64_val << " - type INT64;" << std::endl;
		break;
	}
	case(OPCUA_LOCAL_TEXT): {
		std::cout << variable->data.value.local_text_val << " - type LOCAL_TEXT;" << std::endl;
		break;
	}
	case(OPCUA_S_BYTE): {
		std::cout << variable->data.value.sbyte_val << " - type sBYTE;" << std::endl;
		break;
	}
	case(OPCUA_UNIT16): {
		std::cout << variable->data.value.uint16_val << " - type UINT16;" << std::endl;
		break;
	}
	case(OPCUA_UINT32): {
		std::cout << variable->data.value.uint32_val << " - type UINT32;" << std::endl;
		break;
	}
	default: break;
	}
}

#pragma intrinsic(__rdtsc)

int main()
{
	//set_endpoint("opc.tcp://10.7.46.121:4856/opcua/server");
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
	unsigned __int64 i; 
	std::vector<unsigned __int64> times;
	times.reserve(100000);
	while(val < 100000)
	{		
		i = __rdtsc();
		set_variable_value_int32(&variable);
		i = __rdtsc() - i;
		times.push_back(i);
		//std::cout << i << std::endl;
		set_variable_value_boolean(&variable3);
		variable.data.value.int32_val = i;
		if (val % 2 == 0) {
			i = __rdtsc();
			variable2.data.value.int16_val = i / 1000;
			set_variable_value_int32(&variable2);
		}
		val++;
		variable3.data.value.bool_val = val % 2 == 0;
	}
	unsigned __int64 average = 0;
	unsigned __int64 max = 0;
	unsigned __int64 min = times[0];
	for (unsigned __int64 res : times)
	{
		average += res;
		if (res > max)
			max = res;

		if (res < min)
			res = min;

	}
	stop_server();
	average /= 100000;
	std::cout << "average: " << average << std::endl;
	std::cout << "min: " << min << std::endl;
	std::cout << "max: " << max << std::endl;
	return 0;
}


