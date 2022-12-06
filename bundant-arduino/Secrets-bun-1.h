#include <pgmspace.h>
 
#define SECRET
 
//const char WIFI_SSID[] = "BT-H6CKZ8";               //Founders_Factory_2
//const char WIFI_PASSWORD[] = "CpEUKAtmaPm7rn";           //LaunchingStartups
 
#define THINGNAME "Bunda-1"
 
int8_t TIME_ZONE = 0; //NYC(USA): -5 UTC
 
const char MQTT_HOST[] = "removed.iot.eu-west-1.amazonaws.com";
 
 
static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

// removed

-----END CERTIFICATE-----
)EOF";
 
 
// Copy contents from XXXXXXXX-certificate.pem.crt here ▼
static const char client_cert[] PROGMEM = R"KEY(

// removed

-----END CERTIFICATE-----
)KEY";
 
 
// Copy contents from  XXXXXXXX-private.pem.key here ▼
static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----

// removed

-----END RSA PRIVATE KEY-----

)KEY";
