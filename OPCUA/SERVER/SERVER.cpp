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
	/*std::cout << "Variable id: " << variable->id << std::endl;
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
	}*/
}

#pragma intrinsic(__rdtsc)

int main()
{
	//set_endpoint("opc.tcp://10.7.46.121:4856/opcua/server");
	set_endpoint("opc.tcp://localhost:4856/opcua/server");
	set_server_uri("urn://eniko.ru");

	// Надо задать большим в Properties -> Linker -> System -> Stack Reserve Size
	start_server();
	subscribe_datachange(callback_fcn);
	alloc_nodes(4);
	alloc_variables(8);
	size_t namespace_idx = create_namespace("TEST_MODEL");

	OPCUA_Node cistern;
	cistern.namespace_id = namespace_idx;
	cistern.has_parent_node = false;
	size_t cistern_idx =  add_node(cistern, "Cistern");
	cistern.id = cistern_idx;

	OPCUA_Node cistern_sizes;
	cistern_sizes.namespace_id = namespace_idx;
	cistern_sizes.has_parent_node = true;
	cistern_sizes.parent_node_id = cistern.id;
	size_t cistern_sizes_idx = add_node(cistern_sizes, "Cistern Sizes");
	cistern_sizes.id = cistern_sizes_idx;

	OPCUA_Variable height;
	height.namespace_id = namespace_idx;
	height.parent_id = cistern_sizes.id;
	height.data.type = OPCUA_UINT32;
	height.data.value.uint32_val = 100;
	add_variable(&height, "Height");

	OPCUA_Variable length;
	length.namespace_id = namespace_idx;
	length.parent_id = cistern_sizes.id;
	length.data.type = OPCUA_UINT32;
	length.data.value.uint32_val = 100;
	add_variable(&length, "Length");

	OPCUA_Variable width;
	width.namespace_id = namespace_idx;
	width.parent_id = cistern_sizes.id;
	width.data.type = OPCUA_UINT32;
	width.data.value.uint32_val = 100;
	add_variable(&width, "Width");

	//measurement
	OPCUA_Node measurement;
	measurement.namespace_id = namespace_idx;
	measurement.has_parent_node = true;
	measurement.parent_node_id = cistern.id;
	size_t measurement_idx = add_node(measurement, "Measurement");
	measurement.id = measurement_idx;

	OPCUA_Variable current_height;
	current_height.namespace_id = namespace_idx;
	current_height.parent_id = measurement.id;
	current_height.data.type = OPCUA_DOUBLE;
	current_height.data.value.double_val = 50;
	add_variable(&current_height, "Current Height");

	OPCUA_Node constants;
	constants.namespace_id = namespace_idx;
	constants.has_parent_node = true;
	constants.parent_node_id = measurement.id;
	size_t constants_idx = add_node(constants, "Constants");
	constants.id = constants_idx;
	
	OPCUA_Variable max_height;
	max_height.namespace_id = namespace_idx;
	max_height.parent_id = constants.id;
	max_height.data.type = OPCUA_DOUBLE;
	max_height.data.value.double_val = 80;
	add_variable(&max_height, "Maximum Height");

	OPCUA_Variable min_height;
	min_height.namespace_id = namespace_idx;
	min_height.parent_id = constants.id;
	min_height.data.type = OPCUA_DOUBLE;
	min_height.data.value.double_val = 20;
	add_variable(&min_height, "Minimum Height");

	OPCUA_Variable up_speed;
	up_speed.namespace_id = namespace_idx;
	up_speed.parent_id = constants.id;
	up_speed.data.type = OPCUA_DOUBLE;
	up_speed.data.value.double_val = 40;
	add_variable(&up_speed, "Water Up Speed");

	OPCUA_Variable down_speed;
	down_speed.namespace_id = namespace_idx;
	down_speed.parent_id = constants.id;
	down_speed.data.type = OPCUA_DOUBLE;
	down_speed.data.value.double_val = 10;
	add_variable(&down_speed, "Water Up Speed");

	while(true)
	{
		
	}
	//OPCUA_Variable variable;
	//variable.namespace_id = namespaces[0];
	//variable.parent_id = nodes_core_1[0];
	////variable.name= "TEST";
	//variable.data.type = OPCUA_UINT32;
	//variable.data.value.uint32_val = 0;
	//add_variable(&variable, "TEST");
	//
	//main_parameters.namespace_id = namespaces[1];
	//nodes_core_2[0] = add_node(main_parameters, "INFO");

	//OPCUA_Variable variable2;
	//variable2.namespace_id = namespaces[1];
	//variable2.parent_id = nodes_core_2[0];
	//variable2.data.type = OPCUA_INT16;
	//variable2.data.value.int16_val = 0;
	//add_variable(&variable2, "TEST");

	//OPCUA_Variable variable3;
	//variable3.namespace_id = namespaces[1];
	//variable3.parent_id = nodes_core_2[0];
	//variable3.data.type = OPCUA_BOOLEAN;
	//variable3.data.value.bool_val = true;
	//add_variable(&variable3, "TEST_BOOL");

	//
	//int val = 1;
	//variable.data.value.uint32_val = val;
	//variable2.data.value.int16_val = val;
	//variable3.data.value.bool_val = val % 2 == 0;
	//unsigned __int64 i; 
	//unsigned __int64 all;
	//std::vector<unsigned __int64> times;
	//times.reserve(100000);
	//all = __rdtsc();
	//while(val < 100000)
	//{		
	//	i = __rdtsc();
	//	set_variable_value_uint32(&variable);
	//	i = __rdtsc() - i;
	//	set_variable_value_boolean(&variable3);
	//	variable.data.value.uint32_val = val;
	//	/*if (val % 2 == 0) {
	//		i = __rdtsc();
	//		variable2.data.value.int16_val = i / 1000;
	//		set_variable_value_int16(&variable2);
	//	}*/
	//	val++;
	//	variable3.data.value.bool_val = val % 2 == 0;
	//	std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	//}
	//all = __rdtsc() - all;
	//unsigned __int64 average = 0;
	//unsigned __int64 max = 0;
	//unsigned __int64 min = times[0];
	//for (unsigned __int64 res : times)
	//{
	//	average += res;
	//	if (res > max)
	//		max = res;

	//	if (res < min)
	//		res = min;

	//}
	stop_server();
	/*average /= 100000;
	std::cout << "average: " << average << std::endl;
	std::cout << "min: " << min << std::endl;
	std::cout << "max: " << max << std::endl;
	std::cout << "all: " << all << std::endl;*/
	return 0;
}


