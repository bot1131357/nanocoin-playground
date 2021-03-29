// 
// 
// 

#include "NanoCoin.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <WebSockets2_Generic.h>
using namespace websockets2_generic;
WebsocketsClient client;

StaticJsonDocument<200> doc;
StaticJsonDocument<1024> rx_doc;

char wallet_address[70]{ 0 };
float last_transaction{ 0 };
float balance{ 0 };
void(*_transaction_cb)(double) { nullptr };

void onMessageCallback(WebsocketsMessage message)
{
	Serial.print("Got Message: ");
	Serial.println(message.data());

	DeserializationError error = deserializeJson(rx_doc, message.data());
	Serial.printf("DeserializationError %d\r\n", error);
	const char* ack = rx_doc["ack"];
	const char* confirmation = rx_doc["confirmation"];
	const char* amount = rx_doc["message"]["amount"];

	// ack
	if (nullptr != ack)
	{
		Serial.printf("Ack: [%s]\r\n", ack);
	}

	// transaction confirmation
	if (nullptr != confirmation)
	{
		Serial.printf("Transaction confirmation: [%s]\r\n", confirmation);
	}

	if (nullptr != amount)
	{
		const int amount_len = 40;
		const int trimmed_digits = 20;

		Serial.printf("Amount: [%s]\r\n", amount);
		char buf[amount_len]{ 0 };
		strncpy(buf, amount, amount_len);

		int digit_count = 0;
		for (int i = amount_len - 1; i >= 0; --i)
		{
			if ('\0' != buf[i])
			{
				++digit_count;
				if (digit_count == trimmed_digits)
				{
					buf[i] = '\0';
					break;
				}
			}
		}

		Serial.printf("Amount (snipped): [%s]\r\n", buf);
		int tmp_int = -12345;
		double tmp_float1 = 0.123;
		double tmp_float2 = 0.123;
		sscanf(buf, "%d", &tmp_int);
		tmp_float1 = tmp_int;
		tmp_float2 = tmp_float1 / 10000000000;
		Serial.printf("Conversion: [%d]->[%f]->[%f]\r\n", tmp_int, tmp_float1, tmp_float2);
		if (_transaction_cb) _transaction_cb(tmp_float2);
	}
}

void onEventsCallback(WebsocketsEvent event, String data)
{
	if (event == WebsocketsEvent::ConnectionOpened)
	{
		Serial.println("Websocket Opened");

		doc["action"] = "subscribe";
		doc["topic"] = "confirmation";
		JsonObject options = doc.createNestedObject("options");
		JsonArray accounts = options.createNestedArray("accounts");
		accounts.add(wallet_address);

		char output[512];
		serializeJson(doc, output);
		client.send(output);
		Serial.printf("JSON: %s\r\n", output);
	}
	else if (event == WebsocketsEvent::ConnectionClosed)
	{
		Serial.println("Connnection Closed");
	}
	else if (event == WebsocketsEvent::GotPing)
	{
		Serial.println("Got a Ping!");
	}
	else if (event == WebsocketsEvent::GotPong)
	{
		Serial.println("Got a Pong!");
	}
}

bool NANO::init(const char* host)
{
	// run callback when messages are received
	client.onMessage(onMessageCallback);

	// run callback when events are occuring
	client.onEvent(onEventsCallback);

	// Connect to server
	//client.setCACert(ssl_ca_cert); // (-9984) X509 - Certificate verification failed, e.g. CRL, CA or signature check failed
	//client.connect(websockets_server_host, websockets_server_port, "/");
	client.connect(host);

	return true;
}

int NANO::set_wallet_address(const char* address)
{
	return sprintf(wallet_address, "%s", address);
}

const char* NANO::get_wallet_address()
{
	return nullptr;
}

int NANO::set_transaction_callback(void(*transaction_cb)(double))
{
	if (nullptr != transaction_cb) _transaction_cb = transaction_cb;
	return 0;
}

void NANO::update()
{
	client.poll();
}

float NANO::get_balance()
{
	return balance;
}

float NANO::get_last_transaction()
{
	return last_transaction;
}
