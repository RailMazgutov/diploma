// OPCUA.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "opc_ua.h"

const OPCUA_Namespace* add_namespace(char* name)
{
	OPCUA_Namespace opcua_namespace;
	return &opcua_namespace;
}