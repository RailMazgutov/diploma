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



Node objects;
UaServer server(false);


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

size_t add_variable(OPCUA_Variable* variable)
{
	Node parent = nodes[variable->parent_id];
	
	switch (variable->type)
	{
	case(OPCUA_BOOLEAN): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, variable->name, variable->value.bool_val));
		break;
	}
	case(OPCUA_BYTE): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, variable->name, variable->value.byte_val));
		break;
	}
	case(OPCUA_DOUBLE): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, variable->name, variable->value.double_val));
		break;
	}
	case(OPCUA_FLOAT): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, variable->name, variable->value.float_val));
		break;
	}
	case(OPCUA_INT16): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, variable->name, variable->value.int16_val));
		break;
	}
	case(OPCUA_INT32): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, variable->name, variable->value.int32_val));
		break;
	}
	case(OPCUA_INT64): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, variable->name, variable->value.int64_val));
		break;
	}
	case(OPCUA_LOCAL_TEXT): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, variable->name, variable->value.local_text_val));
		break;
	}
	case(OPCUA_S_BYTE): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, variable->name, variable->value.sbyte_val));
		break;
	}
	case(OPCUA_UNIT16): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, variable->name, variable->value.uint16_val));
		break;
	}
	case(OPCUA_UINT32): {
		variable->id = variables.size();
		variables.push_back(parent.AddVariable(variable->namespace_id, variable->name, variable->value.uint32_val));
		break;
	}
	default: break;
	}
	return variable->id;
}

void add_variables(OPCUA_Variable* variables, size_t count)
{
	for (int i = 0; i < count; i++)
		add_variable(&variables[i]);
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