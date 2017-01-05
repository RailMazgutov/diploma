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

