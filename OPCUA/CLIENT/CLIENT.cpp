// CLIENT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "client.h"
#include <chrono>
#include <thread>
#include <ostream>
#include <iostream>

void callback_fcn(const OPCUA_Variable_Client* variable)
{
	std::cout << "Variable id: " << variable->id << ", name: " << variable->path.name << std::endl;
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

void set_correct_value(OPCUA_Variable_Client* variable)
{
	switch (variable->data.type)
	{
	case(OPCUA_BOOLEAN): {
		variable->data.value.bool_val = !variable->data.value.bool_val;
		break;
	}
	case(OPCUA_BYTE): {
		variable->data.value.byte_val = ++variable->data.value.byte_val;
		break;
	}
	case(OPCUA_DOUBLE): {
		variable->data.value.double_val +=0.1;
		break;
	}
	case(OPCUA_FLOAT): {
		
		break;
	}
	case(OPCUA_INT16): {
		variable->data.value.int16_val += 2;
		break;
	}
	case(OPCUA_INT32): {
		variable->data.value.int32_val += 1;
		break;
	}
	case(OPCUA_INT64): {
		variable->data.value.int64_val += 3;
		break;
	}
	case(OPCUA_LOCAL_TEXT): {
		
		break;
	}
	case(OPCUA_S_BYTE): {
		
		break;
	}
	case(OPCUA_UNIT16): {
		variable->data.value.uint16_val += 2;
		break;
	}
	case(OPCUA_UINT32): {
		variable->data.value.uint32_val += 2;
		break;
	}
	default: break;
	}
	set_value(variable);
}

int main()
{	
	subscribe_datachange_client(callback_fcn);
	connect_to_server("opc.tcp://localhost:4856/opcua/server");
	
	//connect_to_server("opc.tcp://10.7.46.121:4856/opcua/server");
	OPCUA_Node_Client main_node = get_main_node();
	OPCUA_Variable_Client vars;
	vars.has_next = false;
	if (!main_node.has_child_node)
		exit(-2);

	OPCUA_Node_Client *first = main_node.child_node;
	if (first->has_child_variables)
	{
		vars = *first->child;
	}
	else exit(-3);

	while (first->has_next)
	{
		first = first->next;
		if (first->has_child_variables)
			if (vars.has_next)
			{
				OPCUA_Variable_Client* next = vars.next;
				while(next->has_next)
				{
					next = next->next;
				}
				next->next = first->child;
				next->has_next = true;
			} else
			{
				vars = *first->child;
			}
	}

	int c = 1;
	while(c != 0)
	{
		OPCUA_Variable_Client* next = &vars;
		while(next->has_next)
		{
			set_correct_value(next);
			next = next->next;
		}
		set_correct_value(next);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	disconnect_from_server();
	return 0;
}

