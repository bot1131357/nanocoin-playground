// NanoCoin.h

#ifndef _NANOCOIN_h
#define _NANOCOIN_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

namespace NANO
{
	bool init(const char* host);
	int set_wallet_address(const char* address);
	const char* get_wallet_address();
	int set_transaction_callback(void(*transaction_cb)(double));
	void update();
	float get_balance();
	float get_last_transaction();


}

#endif

