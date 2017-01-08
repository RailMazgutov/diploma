#include "client.h"

#include <opc/ua/client/client.h>
#include <opc/ua/node.h>
#include <opc/ua/subscription.h>
#include "server.h"

using namespace OpcUa;

OpcUa::UaClient client(false);
std::unique_ptr<Subscription> sub_client;
std::function<void(const OPCUA_Variable*)> subscription;

OPCUA_Node_Client main_node;

void connect_to_server(char* endpoint) {
	client.Connect(endpoint);
}

void disconnect_from_server() {
	client.Disconnect();
}

void subscribe_datachange_client(void(*callback)(const OPCUA_Variable*))
{
	subscription = callback;
}

OPCUA_Node_Client get_main_node()
{
	Node objects = client.GetObjectsNode();
	for (OpcUa::Node node : objects.GetChildren())
		std::cout << "    " << node.GetBrowseName().NamespaceIndex << std::endl;
	return main_node;
}