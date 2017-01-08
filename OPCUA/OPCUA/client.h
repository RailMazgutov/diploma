#pragma once

#ifdef OPCUA_EXPORTS
#define OPCUACLIENT_API __declspec(dllexport) 
#else
#define OPCUACLIENT_API __declspec(dllimport) 
#endif
#include "server.h"
struct OPCUA_BrowsePath
{
	UInt16							namespace_id;
	char*							browse_name;
};

struct OPCUA_Variable_Client
{
	size_t							id;
	OPCUA_BrowsePath				path;
	
	OPCUA_DataValue					data;

	bool							has_next;
	OPCUA_Variable_Client*			next;
};

struct OPCUA_Node_Client
{
	size_t							id;
	OPCUA_BrowsePath				path;
	
	bool							has_next;
	OPCUA_Node_Client*				next;

	bool							has_child_node;
	OPCUA_Node_Client*				child_node;

	bool							has_child_variables;
	OPCUA_Variable_Client*			child;
};

struct OPCUA_Namespace_Client
{
	size_t							id;
	char*							name;
};

void OPCUACLIENT_API connect_to_server(char* endpoint);

void OPCUACLIENT_API disconnect_from_server();

void OPCUACLIENT_API subscribe_datachange_client(void(*callback)(const OPCUA_Variable*));

OPCUA_Node_Client OPCUACLIENT_API get_main_node();
