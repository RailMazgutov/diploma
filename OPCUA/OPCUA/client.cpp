#include "client.h"

#include <opc/ua/client/client.h>
#include <opc/ua/node.h>
#include <opc/ua/subscription.h>

using namespace OpcUa;

OpcUa::UaClient client(false);

void connect_to_server(char* endpoint) {
	client.Connect(endpoint);
}