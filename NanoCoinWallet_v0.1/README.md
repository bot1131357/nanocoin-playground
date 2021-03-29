# NanoCoin Wallet v0.1
This is a hardware-based Nano coin wallet running on an TTGO (T5 v2.3) ESP32 board combined with an E-ink display module, which is used to display the QR code of the wallet address. 

When connected to the internet, the wallet subscribes to wss://socket.nanos.cc:443. If a transaction is received, the wallet displays the amount on the E-ink for a few seconds before displaying the QR code once again.

Even when powered off, the QR code of the Nano wallet will continue to remain on the E-ink display.  

![NanoCoin Wallet](https://github.com/bot1131357/nanocoin-playground/blob/main/media/nano_coin_wallet.jpg)


## Application
- A digital wallet or a keychain to create awareness
- Point-of-sale (POS) accessory to display QR for buyers to make their payments

## Setup
Currently on arduino-esp32 1.0.4 with the following libraries:
- Adafruit GFX library 1.75
- Arduino Json 6.17.3
- GxEPD 3.0.5
- QRCode 0.0.1
- WebSockets2_Generic-1.1.0


## Features to add:

### - Button interactions
Detecting click, double-click, and long press

### AP and Web mode
Enable Wireless settings 

### - Day dream
Upload quotes or images to display. To display QR code, press the user button on the board. 

