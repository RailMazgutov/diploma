// OPCUA.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <opc/ua/node.h>
#include <opc/ua/subscription.h>
#include <opc/ua/server/server.h>
#include "opc_ua.h"

using namespace OpcUa;
UaServer server(false);

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
	return 0;
}

void start_server()
{
	server.Start();
}

void stop_server()
{
	server.Stop();
}