#include "client.h"

#include <opc/ua/client/client.h>
#include <opc/ua/node.h>
#include <opc/ua/subscription.h>
#include "server.h"

using namespace OpcUa;

std::mutex main_mutex;

OpcUa::UaClient client(false);

std::vector<Node> client_nodes;
std::vector<Node> client_variables;

std::vector<OPCUA_Node_Client* > exp_client_nodes;
std::vector<OPCUA_Variable_Client* > exp_client_variables;

std::unique_ptr<Subscription> sub_client;
std::function<void(const OPCUA_Variable_Client*)> subscription;

OPCUA_Node_Client main_node;

std::map<uint32_t, OPCUA_Variable_Client*> handle_to_var_client;

class SubClientData : public SubscriptionHandler
{
	void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeId attr) override
	{
		main_mutex.lock();
		OPCUA_Variable_Client* variable = handle_to_var_client[handle];

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
		main_mutex.unlock();
		subscription(variable);
	}
};
SubClientData data_change;

void connect_to_server(char* endpoint) {
	client.Connect(endpoint);
	sub_client = client.CreateSubscription(100, data_change);
}

void disconnect_from_server() {
	client.Disconnect();
}

void subscribe_datachange_client(void(*callback)(const OPCUA_Variable_Client*))
{
	subscription = callback;
	
}

void find_children(std::vector<Node> children, OPCUA_Node_Client* exp_parent)
{
	if (children.empty())
	{
		return;
	}
	for (Node child : children)
	{
		std::vector<Node> rec_children = child.GetChildren();
		if (rec_children.empty())
		{
			//Variable;
			client_variables.push_back(child);
			OPCUA_Variable_Client* exp_variable = new OPCUA_Variable_Client;
			QualifiedName name = child.GetBrowseName();
			if (!exp_parent->has_child_variables)
			{
				exp_parent->has_child_variables = true;
				exp_variable->has_next = false;
			}
			else
			{
				exp_variable->has_next = true;
				exp_variable->next = exp_parent->child;
			}
			exp_variable->id = exp_client_variables.size();
			exp_client_variables.push_back(exp_variable);

			exp_variable->path.namespace_id = name.NamespaceIndex;
			size_t size = name.Name.size();
			exp_variable->path.name = new char[size + 1];
			strncpy_s(exp_variable->path.name, size + 1, name.Name.c_str(), size);
			DataValue data = child.GetDataValue();
			Variant value = data.Value;
			switch (value.Type())
			{
			case(VariantType::BOOLEAN): {
				exp_variable->data.value.bool_val = child.GetValue().As<Boolean>();
				exp_variable->data.type = OPCUA_BOOLEAN;
				break;
			}
			case(VariantType::BYTE): {
				exp_variable->data.value.byte_val = child.GetValue().As<Byte>();
				exp_variable->data.type = OPCUA_BYTE;
				break;
			}
			case(VariantType::DOUBLE): {
				exp_variable->data.value.double_val = child.GetValue().As<Double>();
				exp_variable->data.type = OPCUA_DOUBLE;
				break;
			}
			case(VariantType::FLOAT): {
				exp_variable->data.value.float_val = child.GetValue().As<Float>();
				exp_variable->data.type = OPCUA_FLOAT;
				break;
			}
			case(VariantType::INT16): {
				exp_variable->data.value.int16_val = child.GetValue().As<Int16>();
				exp_variable->data.type = OPCUA_INT16;
				break;
			}
			case(VariantType::INT32): {
				exp_variable->data.value.int32_val = child.GetValue().As<Int32>();
				exp_variable->data.type = OPCUA_INT32;
				break;
			}
			case(VariantType::INT64): {
				exp_variable->data.value.int64_val = child.GetValue().As<Int64>();
				exp_variable->data.type = OPCUA_INT64;
				break;
			}
			case(VariantType::LOCALIZED_TEXT): {
				exp_variable->data.value.local_text_val = child.GetValue().As<LocalText>();
				exp_variable->data.type = OPCUA_LOCAL_TEXT;
				break;
			}
			case(VariantType::SBYTE): {
				exp_variable->data.value.sbyte_val = child.GetValue().As<sByte>();
				exp_variable->data.type = OPCUA_S_BYTE;
				break;
			}
			case(VariantType::UINT16): {
				exp_variable->data.value.uint16_val = child.GetValue().As<UInt16>();
				exp_variable->data.type = OPCUA_UNIT16;
				break;
			}
			case(VariantType::UINT32): {
				exp_variable->data.value.uint32_val = child.GetValue().As<UInt32>();
				exp_variable->data.type = OPCUA_UINT32;
				break;
			}
			default: {
				continue;
			}
			}
			exp_parent->child = exp_variable;
			exp_variable->data.timestamp = data.SourceTimestamp.Value;
			exp_variable->data.picoseconds = data.SourcePicoseconds;
			exp_variable->data.status = opcua_status(data.Status);
			auto handle = sub_client->SubscribeDataChange(child);
			handle_to_var_client.insert(std::pair<uint32_t, OPCUA_Variable_Client*>(handle, exp_variable));
		}
		else
		{
			OPCUA_Node_Client* exp_node = new OPCUA_Node_Client;
			QualifiedName name = child.GetBrowseName();
			if (!exp_parent->has_child_node)
			{
				exp_parent->has_child_node = true;
				exp_node->has_next = false;
			}
			else
			{
				exp_node->has_next = true;
				exp_node->next = exp_parent->next;
			}
			exp_parent->child_node = exp_node;

			exp_node->id = exp_client_nodes.size();
			exp_client_nodes.push_back(exp_node);
			client_nodes.push_back(child);
			exp_node->path.namespace_id = name.NamespaceIndex;
			size_t size = name.Name.size();
			exp_node->path.name = new char[size + 1];
			strncpy_s(exp_node->path.name, size + 1, name.Name.c_str(), size);
			find_children(rec_children, exp_node);
		}
	}
}

OPCUA_Node_Client get_main_node()
{
	main_node.has_child_node = false;
	main_node.has_child_variables = false;
	main_node.has_next = false;
	Node objects = client.GetObjectsNode();
	for (OpcUa::Node node : objects.GetChildren()) {
		QualifiedName name = node.GetBrowseName();
		if (name.NamespaceIndex == 0)
			continue;

		OPCUA_Node_Client* exp_node = new OPCUA_Node_Client;
		exp_node->has_child_node = false;
		exp_node->has_child_variables = false;
		exp_node->has_next = false;
		if (!main_node.has_child_node)
		{
			main_node.has_child_node = true;
			exp_node->has_next = false;
		} 
		else
		{
			exp_node->has_next = true;
			exp_node->next = main_node.child_node;
		}
		main_node.child_node = exp_node;

		exp_node->id = exp_client_nodes.size();
		exp_client_nodes.push_back(exp_node);
		
		exp_node->path.namespace_id = name.NamespaceIndex;
		size_t size = name.Name.size();
		exp_node->path.name = new char[size + 1];
		strncpy_s(exp_node->path.name, size+1, name.Name.c_str(), size);
		find_children(node.GetChildren(), exp_node);
	}
	return main_node;
}

void set_value(OPCUA_Variable_Client* variable)
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
	client_variables[variable->id].SetValue(value);
}

int get_variables_count()
{
	return client_variables.size();
}