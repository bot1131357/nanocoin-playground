/*
 Name:		NanoCoinWallet.ino
 Created:	3/12/2021 9:06:45 AM
 Author:	y.tan
*/

#include "Config.h"
#include "Display.h"
#include "NanoCoin.h"

// user setup 
#define ADDRESS "nano_135u9kpjwwtaraz9d7jz1tqae6szngdc6qe45ecoqu6rfrmt1ons7rgjktto"
const char* websockets_server = "wss://socket.nanos.cc:443/"; //server adress and port

#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;
bool check_WLAN();

void display_wallet(const char* status = "ONLINE");
void display_test_frame_message();
void display_frame_message(const char* param, const char* val);
void nano_transaction_callback(double val);

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	for (int i = 0; i < sizeof(ssid) / sizeof(const char*); ++i)
	{
		Serial.printf("Adding %s:%s\r\n", ssid[i], password[i]);
		wifiMulti.addAP(ssid[i], password[i]);
	}

	EINK::init();
	EINK::clear();
	EINK::print(String("Connecting to wifi..."), 125, 40, CENTER_ALIGNMENT);
	EINK::update();

	display_wallet();
	delay(2000);
	//display_wallet("OFFLINE");
	//display_test_frame_message();
	//display_frame_message("Received NANO:", "210210 N");
	//for (;;)
	//{
	//	nano_transaction_callback(123456789);
	//	delay(2000);
	//	nano_transaction_callback(123456.78);
	//	delay(2000);
	//	nano_transaction_callback(123.45678);
	//	delay(2000);
	//	nano_transaction_callback(0.12345678);
	//	delay(2000);
	//	nano_transaction_callback(0.00012345678);
	//	delay(2000);
	//	nano_transaction_callback(0.00000012345678);
	//	delay(2000);
	//}
	//display_frame_message("Received NANO:", "0.00012313N");
	//for (;;);
}

uint32_t next_disp_qr_ms = 0;
const uint32_t display_received_ms = 3000;

bool check_WLAN()
{
	static uint32_t nextTry{ 0 };
	const uint32_t reconnect_delay{ 1000 };
	static bool connected{ false };

	if (millis() > nextTry)
	{
		if (wifiMulti.run() == WL_CONNECTED)
		{
			if (!connected) // upon connection
			{
				Serial.println("WiFi connected!");
				Serial.printf("IP: %s\r\n", WiFi.localIP().toString().c_str());

				NANO::set_transaction_callback(nano_transaction_callback);
				NANO::set_wallet_address(ADDRESS);
				NANO::init(websockets_server);
				connected = true;
				nextTry = 0;

				// display wallet
				display_wallet();
			}
		}
		else
		{
			Serial.println("WiFi not connected!");
			connected = false;
			nextTry = millis() + reconnect_delay;

			// display wallet
			display_wallet("OFFLINE");
		}
	}
	return connected;
}

// the loop function runs over and over again until power down or reset
void loop() {
	if (check_WLAN())
	{
		NANO::update();
	}

	if (0 != next_disp_qr_ms)
	{
		if (millis() > next_disp_qr_ms)
		{
			next_disp_qr_ms = 0;
			display_wallet();
		}
	}
}

void display_wallet(const char* status)
{
	EINK::clear();
	EINK::drawQR(ADDRESS, 5, 5);
	EINK::drawBitmap("/nano_logo.bmp", 125, 10, true);
	EINK::print(String(status), 145, 105, LEFT_ALIGNMENT);
	EINK::update();
}

void display_test_frame_message()
{
	EINK::clear();
	EINK::drawFrame(5);
	EINK::print(String("TEST L"), 20, 40, LEFT_ALIGNMENT  , 2);
	EINK::print(String("TEST C"), 20, 60, CENTER_ALIGNMENT, 2);
	EINK::print(String("TEST R"), 20, 80, RIGHT_ALIGNMENT , 2);
	EINK::update();
}

void display_frame_message(const char* param, const char* val)
{
	EINK::clear();
	EINK::drawFrame(5);
	EINK::print(String(param), 20, 40, LEFT_ALIGNMENT);
	EINK::print(String(val), 10, 80, RIGHT_ALIGNMENT, 2);
	EINK::update();
}

//void display_frame_value_unit(const char* param, const char* val, const char* unit)
//{
//	EINK::clear();
//	EINK::drawFrame(5);
//	EINK::print(String(param), 20, 40, LEFT_ALIGNMENT);
//	EINK::print(String(val), 20, 80, RIGHT_ALIGNMENT, 2);
//	EINK::print(String(unit), 0, 80, RIGHT_ALIGNMENT, 2);
//	EINK::update();
//}

void nano_transaction_callback(double val)
{

	Serial.printf("val: %f\r\n", val);

	const int max_digits = 7;
	char unit_prefix = ' ';
	int frac_digits = 0;
	int tens_digits = 0;

	// determind tens_digits
	int tmp = val;
	while (tmp >= 1)
	{
		++tens_digits;
		tmp /= 10;
	}


	if (tens_digits > 9)
	{
		// infinity

		return;
	}

	if (tens_digits > 0)
	{
		if (tens_digits > 6)
		{
			unit_prefix = 'M';
			val /= pow10(6);
			frac_digits = max_digits - (tens_digits - 6)-1;
		}
		else if (tens_digits > 3)
		{
			unit_prefix = 'k';
			val /= pow10(3);
			frac_digits = max_digits - (tens_digits - 3)-1;
		}
		else
		{
			frac_digits = max_digits - tens_digits;
		}

	}
	else
	{
		double tmp = val;
		frac_digits = 0;
		for (int i = 0; i < 9; ++i)
		{
			if (int(tmp) == 0)
			{
				tmp *= 10;
				++frac_digits;
			}
			else
			{
				break;
			}
		}

		if (int(tmp) == 0)
		{
			// treat as 0
		}
		else if (frac_digits > 9)
		{
			unit_prefix = 'p';
			val *= pow10(12);
			frac_digits = max_digits - (tens_digits - 3) - 1;
		}
		else if (frac_digits >6)
		{
			unit_prefix = 'n';
			val *= pow10(9);
		}
		else if (frac_digits > 3)
		{
			unit_prefix = 'u';
			val *= pow10(6);
		}
		else if (frac_digits > 0)
		{
			unit_prefix = 'm';
			val *= pow10(3);
		}
	}

	char buf[16];
	sprintf(buf, "%0.*f%cN\r\n", 4, val, unit_prefix);
	display_frame_message("RECEIVED NANO: ", buf);
	next_disp_qr_ms = millis() + display_received_ms;




	//if (tens_digits > max_digits) // ignore fraction
	//{
	//	frac_digits = max_digits - tens_digits;
	//	val /= pow10(frac_digits);
	//}
	//else
	//{
	//	frac_digits = max_digits - 1 - tens_digits;
	//}

}
