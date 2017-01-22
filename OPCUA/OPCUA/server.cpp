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
std::vector<OPCUA_Variable> export_variables;
std::map<uint32_t, OPCUA_Variable> handle_to_var;

std::function<void(const OPCUA_Variable*)> callback_function;

Node objects;
UaServer server(false);
std::unique_ptr<Subscription> sub;

class SubClient : public SubscriptionHandler
{
	void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeId attr) override
	{
		OPCUA_Variable* variable = &handle_to_var[handle];
		
		DataValue data = node.GetDataValue();		
		Variant value = data.Value;		
		switch (value.Type())
		{
		case(VariantType::BOOLEAN): {
			variable->data.value.bool_val = node.GetValue().As<Boolean>();
			break;
		}
		case(VariantType::BYTE): {
			variable->data.value.byte_val = node.GetValue().As<Byte>();
			break;
		}
		case(VariantType::DOUBLE): {
			variable->data.value.double_val = node.GetValue().As<Double>();
			break;
		}
		case(VariantType::FLOAT): {
			variable->data.value.float_val = node.GetValue().As<Float>();
			break;
		}
		case(VariantType::INT16): {
			variable->data.value.int16_val = node.GetValue().As<Int16>();
			break;
		}
		case(VariantType::INT32): {
			variable->data.value.int32_val = node.GetValue().As<Int32>();
			break;
		}
		case(VariantType::INT64): {
			variable->data.value.int64_val = node.GetValue().As<Int64>();
			break;
		}
		case(VariantType::LOCALIZED_TEXT): {
			variable->data.value.local_text_val = node.GetValue().As<LocalText>();
			break;
		}
		case(VariantType::SBYTE): {
			variable->data.value.sbyte_val = node.GetValue().As<sByte>();
			break;
		}
		case(VariantType::UINT16): {
			variable->data.value.uint16_val = node.GetValue().As<UInt16>();
			break;
		}
		case(VariantType::UINT32): {
			variable->data.value.uint32_val = node.GetValue().As<UInt32>();
			break;
		}
		default: {
			return;
		}
		}
		variable->data.timestamp = data.SourceTimestamp.Value;
		variable->data.picoseconds = data.SourcePicoseconds;
		variable->data.status = opcua_status(data.Status);
		callback_function(variable);
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

size_t create_namespace(char* name)
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

	switch (variable->data.type)
	{
	case(OPCUA_BOOLEAN): {
		variables.push_back(parent.AddVariable(variable->namespace_id, name, Variant(variable->data.value.bool_val)));
		break;
	}
	case(OPCUA_BYTE): {
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->data.value.byte_val));
		break;
	}
	case(OPCUA_DOUBLE): {
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->data.value.double_val));
		break;
	}
	case(OPCUA_FLOAT): {
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->data.value.float_val));
		break;
	}
	case(OPCUA_INT16): {
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->data.value.int16_val));
		break;
	}
	case(OPCUA_INT32): {
		variables.push_back(parent.AddVariable(variable->namespace_id, name, Variant(variable->data.value.int32_val)));
		break;
	}
	case(OPCUA_INT64): {
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->data.value.int64_val));
		break;
	}
	case(OPCUA_LOCAL_TEXT): {
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->data.value.local_text_val));
		break;
	}
	case(OPCUA_S_BYTE): {
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->data.value.sbyte_val));
		break;
	}
	case(OPCUA_UNIT16): {
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->data.value.uint16_val));
		break;
	}
	case(OPCUA_UINT32): {
		
		variables.push_back(parent.AddVariable(variable->namespace_id, name, variable->data.value.uint32_val));
		break;
	}
	default: break;
	}
	variable->id = variables.size() - 1;
	auto handle = sub->SubscribeDataChange(variables[variable->id]);
	handle_to_var.insert(std::pair<uint32_t, OPCUA_Variable>(handle, *variable));
	return variable->id;
}

void get_variable_value(OPCUA_Variable* variable)
{
	switch (variable->data.type)
	{
	case(OPCUA_BOOLEAN): {
		variable->data.value.bool_val = variables[variable->id].GetValue().As<Boolean>();
		break;
	}
	case(OPCUA_BYTE): {
		variable->data.value.byte_val = variables[variable->id].GetValue().As<Byte>();
		break;
	}
	case(OPCUA_DOUBLE): {
		variable->data.value.double_val = variables[variable->id].GetValue().As<Double>();
		break;
	}
	case(OPCUA_FLOAT): {
		variable->data.value.float_val = variables[variable->id].GetValue().As<Float>();
		break;
	}
	case(OPCUA_INT16): {
		variable->data.value.int16_val = variables[variable->id].GetValue().As<Int16>();
		break;
	}
	case(OPCUA_INT32): {
		variable->data.value.int32_val = variables[variable->id].GetValue().As<Int32>();
		break;
	}
	case(OPCUA_INT64): {
		variable->data.value.int64_val = variables[variable->id].GetValue().As<Int64>();
		break;
	}
	case(OPCUA_LOCAL_TEXT): {
		variable->data.value.local_text_val = variables[variable->id].GetValue().As<LocalText>();
		break;
	}
	case(OPCUA_S_BYTE): {
		variable->data.value.sbyte_val = variables[variable->id].GetValue().As<sByte>();
		break;
	}
	case(OPCUA_UNIT16): {
		variable->data.value.uint16_val = variables[variable->id].GetValue().As<UInt16>();
		break;
	}
	case(OPCUA_UINT32): {
		variable->data.value.uint32_val = variables[variable->id].GetValue().As<UInt32>();
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
	DataValue value;
	value.Status = StatusCode(variable->data.status);
	value.SourceTimestamp = DateTime(variable->data.timestamp);
	value.SourcePicoseconds = variable->data.picoseconds;
	value.Encoding = DATA_VALUE;
	switch (variable->data.type)
	{
	case(OPCUA_BOOLEAN): {
		value.Value = variable->data.value.bool_val;		
		break;
	}
	case(OPCUA_BYTE): {
		value.Value = variable->data.value.byte_val;
		break;
	}
	case(OPCUA_DOUBLE): {
		value.Value = variable->data.value.double_val;
		break;
	}
	case(OPCUA_FLOAT): {
		value.Value = variable->data.value.float_val;
		break;
	}
	case(OPCUA_INT16): {
		value.Value = variable->data.value.int16_val;
		break;
	}
	case(OPCUA_INT32): {
		value.Value = variable->data.value.int32_val;
		break;
	}
	case(OPCUA_INT64): {
		value.Value = variable->data.value.int64_val;
		break;
	}
	case(OPCUA_LOCAL_TEXT): {
		value.Value = variable->data.value.local_text_val;
		break;
	}
	case(OPCUA_S_BYTE): {
		value.Value = variable->data.value.sbyte_val;
		break;
	}
	case(OPCUA_UNIT16): {
		value.Value = variable->data.value.uint16_val;
		break;
	}
	case(OPCUA_UINT32): {
		value.Value = variable->data.value.uint32_val;
		break;
	}
	}
	variables[variable->id].SetValue(value);
}

void set_variables_value(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value(&variables[i]);
}

void get_variable_value_boolean(OPCUA_Variable* variable)
{
	variable->data.value.bool_val = variables[variable->id].GetValue().As<Boolean>();
}

void get_variables_value_boolean(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_boolean(&variables[i]);
}

void set_variable_value_boolean(OPCUA_Variable* variable)
{
	DataValue value;
	value.Status = StatusCode(variable->data.status);
	value.SourceTimestamp = DateTime(variable->data.timestamp);
	value.SourcePicoseconds = variable->data.picoseconds;
	value.Value = variable->data.value.bool_val;
	value.Encoding = DATA_VALUE;
	variables[variable->id].SetValue(value);
}

void set_variables_value_boolean(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_boolean(&variables[i]);
}

void get_variable_value_byte(OPCUA_Variable* variable)
{
	variable->data.value.byte_val = variables[variable->id].GetValue().As<Byte>();
}

void get_variables_value_byte(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_byte(&variables[i]);
}

void set_variable_value_byte(OPCUA_Variable* variable)
{
	DataValue value;
	value.Status = StatusCode(variable->data.status);
	value.SourceTimestamp = DateTime(variable->data.timestamp);
	value.SourcePicoseconds = variable->data.picoseconds;
	value.Value = variable->data.value.byte_val;
	value.Encoding = DATA_VALUE;
	variables[variable->id].SetValue(value);
}

void set_variables_value_byte(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_byte(&variables[i]);
}

void get_variable_value_double(OPCUA_Variable* variable)
{
	
	variable->data.value.double_val = variables[variable->id].GetValue().As<Double>();
}

void get_variables_value_double(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_double(&variables[i]);
}

void set_variable_value_double(OPCUA_Variable* variable)
{
	DataValue value;
	value.Status = StatusCode(variable->data.status);
	value.SourceTimestamp = DateTime(variable->data.timestamp);
	value.SourcePicoseconds = variable->data.picoseconds;
	value.Value = variable->data.value.double_val;
	value.Encoding = DATA_VALUE;
	variables[variable->id].SetValue(value);
}

void set_variables_value_double(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_double(&variables[i]);
}

void get_variable_value_float(OPCUA_Variable* variable)
{
	variable->data.value.float_val = variables[variable->id].GetValue().As<Float>();
}

void get_variables_value_float(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_float(&variables[i]);
}

void set_variable_value_float(OPCUA_Variable* variable)
{
	DataValue value;
	value.Status = StatusCode(variable->data.status);
	value.SourceTimestamp = DateTime(variable->data.timestamp);
	value.SourcePicoseconds = variable->data.picoseconds;
	value.Value = variable->data.value.float_val;
	value.Encoding = DATA_VALUE;
	variables[variable->id].SetValue(value);
}

void set_variables_value_float(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_float(&variables[i]);
}

void get_variable_value_int16(OPCUA_Variable* variable)
{
	variable->data.value.int16_val = variables[variable->id].GetValue().As<Int16>();
}

void get_variables_value_int16(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_int16(&variables[i]);
}

void set_variable_value_int16(OPCUA_Variable* variable)
{
	DataValue value;
	value.Status = StatusCode(variable->data.status);
	value.SourceTimestamp = DateTime(variable->data.timestamp);
	value.SourcePicoseconds = variable->data.picoseconds;
	value.Value = variable->data.value.int16_val;
	value.Encoding = DATA_VALUE;
	variables[variable->id].SetValue(value);
}

void set_variables_value_int16(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_int16(&variables[i]);
}

void get_variable_value_int32(OPCUA_Variable* variable)
{
	variable->data.value.int32_val = variables[variable->id].GetValue().As<Int32>();
}

void get_variables_value_int32(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_int32(&variables[i]);
}

void set_variable_value_int32(OPCUA_Variable* variable)
{
	DataValue value;
	value.Status = StatusCode(variable->data.status);
	value.SourceTimestamp = DateTime(variable->data.timestamp);
	value.SourcePicoseconds = variable->data.picoseconds;
	value.Value = variable->data.value.int32_val;
	value.Encoding = DATA_VALUE;
	variables[variable->id].SetValue(value);
}

void set_variables_value_int32(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_int32(&variables[i]);
}

void get_variable_value_int64(OPCUA_Variable* variable)
{
	variable->data.value.int64_val = variables[variable->id].GetValue().As<Int64>();
}

void get_variables_value_int64(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_int64(&variables[i]);
}

void set_variable_value_int64(OPCUA_Variable* variable)
{
	DataValue value;
	value.Status = StatusCode(variable->data.status);
	value.SourceTimestamp = DateTime(variable->data.timestamp);
	value.SourcePicoseconds = variable->data.picoseconds;
	value.Value = variable->data.value.int64_val;
	value.Encoding = DATA_VALUE;
	variables[variable->id].SetValue(value);
}

void set_variables_value_int64(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_int64(&variables[i]);
}

void get_variable_value_local_text(OPCUA_Variable* variable)
{
	variable->data.value.local_text_val = variables[variable->id].GetValue().As<LocalText>();
}

void get_variables_value_local_text(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_local_text(&variables[i]);
}

void set_variable_value_local_text(OPCUA_Variable* variable)
{
	DataValue value;
	value.Status = StatusCode(variable->data.status);
	value.SourceTimestamp = DateTime(variable->data.timestamp);
	value.SourcePicoseconds = variable->data.picoseconds;
	value.Value = variable->data.value.local_text_val;
	value.Encoding = DATA_VALUE;
	variables[variable->id].SetValue(value);
}

void set_variables_value_local_text(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_local_text(&variables[i]);
}

void get_variable_value_sbyte(OPCUA_Variable* variable)
{
	variable->data.value.sbyte_val = variables[variable->id].GetValue().As<sByte>();
}

void get_variables_value_sbyte(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_sbyte(&variables[i]);
}

void set_variable_value_sbyte(OPCUA_Variable* variable)
{
	DataValue value;
	value.Status = StatusCode(variable->data.status);
	value.SourceTimestamp = DateTime(variable->data.timestamp);
	value.SourcePicoseconds = variable->data.picoseconds;
	value.Value = variable->data.value.sbyte_val;
	value.Encoding = DATA_VALUE;
	variables[variable->id].SetValue(value);
}

void set_variables_value_sbyte(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_sbyte(&variables[i]);
}

void get_variable_value_uint16(OPCUA_Variable* variable)
{
	variable->data.value.uint16_val = variables[variable->id].GetValue().As<UInt16>();
}

void get_variables_value_uint16(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_uint16(&variables[i]);
}

void set_variable_value_uint16(OPCUA_Variable* variable)
{
	DataValue value;
	value.Status = StatusCode(variable->data.status);
	value.SourceTimestamp = DateTime(variable->data.timestamp);
	value.SourcePicoseconds = variable->data.picoseconds;
	value.Value = variable->data.value.uint16_val;
	value.Encoding = DATA_VALUE;
	variables[variable->id].SetValue(value);
}

void set_variables_value_uint16(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_uint16(&variables[i]);
}

void get_variable_value_uint32(OPCUA_Variable* variable)
{
	variable->data.value.uint32_val = variables[variable->id].GetValue().As<UInt32>();
}

void get_variables_value_uint32(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		get_variable_value_uint32(&variables[i]);
}

void set_variable_value_uint32(OPCUA_Variable* variable)
{
	DataValue value;
	value.Status = StatusCode(variable->data.status);
	value.SourceTimestamp = DateTime(variable->data.timestamp);
	value.SourcePicoseconds = variable->data.picoseconds;
	value.Value = variable->data.value.uint32_val;
	value.Encoding = DATA_VALUE;
	variables[variable->id].SetValue(value);
}

void set_variables_value_uint32(OPCUA_Variable* variables, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
		set_variable_value_uint32(&variables[i]);
}

void subscribe_datachange(void(*callback)(const OPCUA_Variable*))
{
	callback_function = callback;
}
