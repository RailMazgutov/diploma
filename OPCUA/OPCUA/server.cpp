// OPCUA.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <opc/ua/node.h>
#include <opc/ua/subscription.h>
#include <opc/ua/server/server.h>
#include "server.h"

using namespace OpcUa;

std::vector<Node> nodes;
std::vector<Node> variables;
std::map<OPCUA_Variable, Node> receive_map;
std::map<Node, OPCUA_Variable> export_map;
std::map<uint32_t, size_t> handle_to_var;
std::map<uint32_t, std::function<void(const OPCUA_Variable*)>> handle_to_callback;

std::function<void(const OPCUA_Variable*)> callback_function;

Node objects;
UaServer server(false);
std::unique_ptr<Subscription> sub;

class SubClient : public SubscriptionHandler
{
	void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeId attr) override
	{
		//Node variable = variables[handle_to_var[handle]];
		OPCUA_Variable* exp_variable = &(export_map[node]);
		callback_function(exp_variable);
		/*std::cout << "Received DataChange event, value of Node " << node << " is now: " << val.ToString() << std::endl;
		std::cout << "handle " << handle << std::endl;*/
	}
};
SubClient sclt;

Node* getNode(size_t id)
{
	return &nodes[id];
}

void set_endpoint(const char* endpoint)
{
	server.SetEndpoint(endpoint);
}

void set_server_uri(const char* uri)
{
	server.SetServerURI(uri);
}

unsigned int create_namespace(char* name)
{	
	return server.RegisterNamespace(name);
}

void start_server()
{
	server.Start();
	
	sub = server.CreateSubscription(100, sclt);
	objects = server.GetObjectsNode();
}

void stop_server()
{
	server.Stop();
}

size_t add_node(OPCUA_Node node, char* name)
{
	Node* parent = &objects;
	if (node.has_parent_node)
	{
		parent = getNode(node.parent_node_id);
	}
	Node self = parent->AddObject(node.namespace_id, name);
	nodes.push_back(self);
	return nodes.size() - 1;
}

void alloc_nodes(size_t count)
{
	nodes.reserve(count);
}

void alloc_variables(size_t count)
{
	variables.reserve(count);
}

size_t add_variable(OPCUA_Variable* variable, char* name)
{
	Node parent = nodes[variable->parent_id];
	
	switch (variable->type)
	{
	case(OPCUA_BOOLEAN): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->value.bool_val));
		break;
	}
	case(OPCUA_BYTE): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->value.byte_val));
		break;
	}
	case(OPCUA_DOUBLE): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->value.double_val));
		break;
	}
	case(OPCUA_FLOAT): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->value.float_val));
		break;
	}
	case(OPCUA_INT16): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->value.int16_val));
		break;
	}
	case(OPCUA_INT32): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->value.int32_val));
		auto q = sub->SubscribeDataChange(variables[variable->id]);
		std::cout << q << std::endl;
		break;
	}
	case(OPCUA_INT64): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->value.int64_val));
		break;
	}
	case(OPCUA_LOCAL_TEXT): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->value.local_text_val));
		break;
	}
	case(OPCUA_S_BYTE): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->value.sbyte_val));
		break;
	}
	case(OPCUA_UNIT16): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->value.uint16_val));
		break;
	}
	case(OPCUA_UINT32): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->value.uint32_val));
		break;
	}
	default: break;
	}
	return variable->id;
}

void get_variable_value(OPCUA_Variable* variable)
{
	switch (variable->type)
	{
	case(OPCUA_BOOLEAN): {
		variable->value.bool_val = variables[variable->id].GetValue().As<Boolean>();
		break;
	}
	case(OPCUA_BYTE): {
		variable->value.byte_val = variables[variable->id].GetValue().As<Byte>();
		break;
	}
	case(OPCUA_DOUBLE): {
		variable->value.double_val = variables[variable->id].GetValue().As<Double>();
		break;
	}
	case(OPCUA_FLOAT): {
		variable->value.float_val = variables[variable->id].GetValue().As<Float>();
		break;
	}
	case(OPCUA_INT16): {
		variable->value.int16_val = variables[variable->id].GetValue().As<Int16>();
		break;
	}
	case(OPCUA_INT32): {
		variable->value.int32_val = variables[variable->id].GetValue().As<Int32>();
		break;
	}
	case(OPCUA_INT64): {
		variable->value.int64_val = variables[variable->id].GetValue().As<Int64>();
		break;
	}
	case(OPCUA_LOCAL_TEXT): {
		variable->value.local_text_val = variables[variable->id].GetValue().As<LocalText>();
		break;
	}
	case(OPCUA_S_BYTE): {
		variable->value.sbyte_val = variables[variable->id].GetValue().As<sByte>();
		break;
	}
	case(OPCUA_UNIT16): {
		variable->value.uint16_val = variables[variable->id].GetValue().As<UInt16>();
		break;
	}
	case(OPCUA_UINT32): {
		variable->value.uint32_val = variables[variable->id].GetValue().As<UInt32>();
		break;
	}
	}
}

void get_variables_value(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value(&variables[i]);
}

void set_variable_value(OPCUA_Variable* variable)
{
	switch (variable->type)
	{
	case(OPCUA_BOOLEAN): {
		variables[variable->id].SetValue(variable->value.bool_val);
		break;
	}
	case(OPCUA_BYTE): {
		variables[variable->id].SetValue(variable->value.byte_val);
		break;
	}
	case(OPCUA_DOUBLE): {
		variables[variable->id].SetValue(variable->value.double_val);
		break;
	}
	case(OPCUA_FLOAT): {
		variables[variable->id].SetValue(variable->value.float_val);
		break;
	}
	case(OPCUA_INT16): {
		variables[variable->id].SetValue(variable->value.int16_val);
		break;
	}
	case(OPCUA_INT32): {
		variables[variable->id].SetValue(variable->value.int32_val);
		break;
	}
	case(OPCUA_INT64): {
		variables[variable->id].SetValue(variable->value.int64_val);
		break;
	}
	case(OPCUA_LOCAL_TEXT): {
		variables[variable->id].SetValue(variable->value.local_text_val);
		break;
	}
	case(OPCUA_S_BYTE): {
		variables[variable->id].SetValue(variable->value.sbyte_val);
		break;
	}
	case(OPCUA_UNIT16): {
		variables[variable->id].SetValue(variable->value.uint16_val);
		break;
	}
	case(OPCUA_UINT32): {
		variables[variable->id].SetValue(variable->value.uint32_val);
		break;
	}
	}
}

void set_variables_value(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value(&variables[i]);
}

void get_variable_value_boolean(OPCUA_Variable* variable)
{
	variable->value.bool_val = variables[variable->id].GetValue().As<Boolean>();
}

void get_variables_value_boolean(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_boolean(&variables[i]);
}

void set_variable_value_boolean(OPCUA_Variable* variable)
{
	variables[variable->id].SetValue(variable->value.bool_val);
}

void set_variables_value_boolean(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_boolean(&variables[i]);
}

void get_variable_value_byte(OPCUA_Variable* variable)
{
	variable->value.byte_val = variables[variable->id].GetValue().As<Byte>();
}

void get_variables_value_byte(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_byte(&variables[i]);
}

void set_variable_value_byte(OPCUA_Variable* variable)
{
	variables[variable->id].SetValue(variable->value.byte_val);
}

void set_variables_value_byte(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_byte(&variables[i]);
}

void get_variable_value_double(OPCUA_Variable* variable)
{
	variable->value.double_val = variables[variable->id].GetValue().As<Double>();
}

void get_variables_value_double(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_double(&variables[i]);
}

void set_variable_value_double(OPCUA_Variable* variable)
{
	variables[variable->id].SetValue(variable->value.double_val);
}

void set_variables_value_double(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_double(&variables[i]);
}

void get_variable_value_float(OPCUA_Variable* variable)
{
	variable->value.float_val = variables[variable->id].GetValue().As<Float>();
}

void get_variables_value_float(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_float(&variables[i]);
}

void set_variable_value_float(OPCUA_Variable* variable)
{
	variables[variable->id].SetValue(variable->value.float_val);
}

void set_variables_value_float(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_float(&variables[i]);
}

void get_variable_value_int16(OPCUA_Variable* variable)
{
	variable->value.int16_val = variables[variable->id].GetValue().As<Int16>();
}

void get_variables_value_int16(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_int16(&variables[i]);
}

void set_variable_value_int16(OPCUA_Variable* variable)
{
	variables[variable->id].SetValue(variable->value.int16_val);
}

void set_variables_value_int16(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_int16(&variables[i]);
}

void get_variable_value_int32(OPCUA_Variable* variable)
{
	variable->value.int32_val = variables[variable->id].GetValue().As<Int32>();
}

void get_variables_value_int32(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_int32(&variables[i]);
}

void set_variable_value_int32(OPCUA_Variable* variable)
{
	variables[variable->id].SetValue(variable->value.int32_val);
}

void set_variables_value_int32(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_int32(&variables[i]);
}

void get_variable_value_int64(OPCUA_Variable* variable)
{
	variable->value.int64_val = variables[variable->id].GetValue().As<Int64>();
}

void get_variables_value_int64(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_int64(&variables[i]);
}

void set_variable_value_int64(OPCUA_Variable* variable)
{
	variables[variable->id].SetValue(variable->value.int64_val);
}

void set_variables_value_int64(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_int64(&variables[i]);
}

void get_variable_value_local_text(OPCUA_Variable* variable)
{
	variable->value.local_text_val = variables[variable->id].GetValue().As<LocalText>();
}

void get_variables_value_local_text(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_local_text(&variables[i]);
}

void set_variable_value_local_text(OPCUA_Variable* variable)
{
	variables[variable->id].SetValue(variable->value.local_text_val);
}

void set_variables_value_local_text(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_local_text(&variables[i]);
}

void get_variable_value_sbyte(OPCUA_Variable* variable)
{
	variable->value.sbyte_val = variables[variable->id].GetValue().As<sByte>();
}

void get_variables_value_sbyte(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_sbyte(&variables[i]);
}

void set_variable_value_sbyte(OPCUA_Variable* variable)
{
	variables[variable->id].SetValue(variable->value.sbyte_val);
}

void set_variables_value_sbyte(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_sbyte(&variables[i]);
}

void get_variable_value_uint16(OPCUA_Variable* variable)
{
	variable->value.uint16_val = variables[variable->id].GetValue().As<UInt16>();
}

void get_variables_value_uint16(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_uint16(&variables[i]);
}

void set_variable_value_uint16(OPCUA_Variable* variable)
{
	variables[variable->id].SetValue(variable->value.uint16_val);
}

void set_variables_value_uint16(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_uint16(&variables[i]);
}

void get_variable_value_uint32(OPCUA_Variable* variable)
{
	variable->value.uint32_val = variables[variable->id].GetValue().As<UInt32>();
}

void get_variables_value_uint32(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_uint32(&variables[i]);
}

void set_variable_value_uint32(OPCUA_Variable* variable)
{
	variables[variable->id].SetValue(variable->value.uint32_val);
}

void set_variables_value_uint32(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_uint32(&variables[i]);
}

void subscribe_datachange(OPCUA_Variable* variable, void(*callback)(OPCUA_Variable))
{
	
}