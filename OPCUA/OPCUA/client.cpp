#include "client.h"

#include <opc/ua/client/client.h>
#include <opc/ua/node.h>
#include <opc/ua/subscription.h>
#include "server.h"

using namespace OpcUa;

OpcUa::UaClient client(false);
std::unique_ptr<Subscription> sub;
std::function<void(const OPCUA_Variable*)> callback_function;

void connect_to_server(char* endpoint) {
	client.Connect(endpoint);
}

void subscribe_datachange(void(*callback)(const OPCUA_Variable*))
{
	callback_function = callback;
}