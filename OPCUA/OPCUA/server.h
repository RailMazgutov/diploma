#pragma once
#ifdef OPCUA_EXPORTS
#define OPCUASERVER_API __declspec(dllexport) 
#else
#define OPCUASERVER_API __declspec(dllimport) 
#endif

#define NAME_MAX_LENGTH 100
typedef bool Boolean;
typedef unsigned char Byte;
typedef double Double;
typedef float Float;
typedef short Int16;
typedef int Int32;
typedef long int Int64;
typedef char* LocalText;
typedef char sByte;
typedef unsigned short UInt16;
typedef unsigned int UInt32;

const bool debug					= false;
//const char* namespace_name			= "http://eniko.ru";
//const char* endpoint				= "opc.tcp://localhost:4856/opcua/server";
//const char* server_uri				= "urn://eniko.ru";

enum opcua_data_type
{
	OPCUA_BOOLEAN,
	OPCUA_BYTE,
	OPCUA_DOUBLE,
	OPCUA_FLOAT,
	OPCUA_INT16,
	OPCUA_INT32,
	OPCUA_INT64,
	OPCUA_LOCAL_TEXT,
	OPCUA_S_BYTE,
	OPCUA_UNIT16,
	OPCUA_UINT32
};

struct OPCUA_Variable
{
	union opcua_value
	{
		Boolean				bool_val;
		Byte				byte_val;
		Double				double_val;
		Float				float_val;
		Int16				int16_val;
		Int32				int32_val;
		Int64				int64_val;
		LocalText			local_text_val;
		sByte				sbyte_val;
		UInt16				uint16_val;
		UInt32				uint32_val;
	};

	//Для представления значения 
	opcua_data_type type;
	opcua_value value;

	//Для идентификации
	unsigned int id;
	unsigned int parent_id;
	unsigned int namespace_id;
	//Для представления пользователю
	char name[NAME_MAX_LENGTH];
};

struct OPCUA_Namespace
{
	//Для идентификации
	unsigned int id;
	char name[NAME_MAX_LENGTH];
};

struct OPCUA_Node
{
	size_t id;
	size_t namespace_id;
	
	bool has_parent_node;
	size_t parent_node_id;
};
void OPCUASERVER_API set_endpoint(const char* endpoint);

void OPCUASERVER_API set_server_uri(const char* uri);

void OPCUASERVER_API start_server();

void OPCUASERVER_API stop_server();

size_t OPCUASERVER_API create_namespace(char* name);

size_t OPCUASERVER_API add_node(OPCUA_Node node, char* name);

void OPCUASERVER_API alloc_nodes(size_t count);

size_t OPCUASERVER_API add_variable(OPCUA_Variable* variable);

void OPCUASERVER_API alloc_variables(size_t count);

void OPCUASERVER_API get_variable_value(OPCUA_Variable* variable);

void OPCUASERVER_API get_variables_value(OPCUA_Variable* variables, size_t count);