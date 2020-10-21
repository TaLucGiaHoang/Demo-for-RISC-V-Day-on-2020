/*
 * esp32_AT.h
 *
 *  Created on: Oct 10, 2019
 *      Author: SHC Company
 */

#ifndef __ESP32_AT_H__
#define __ESP32_AT_H__

#define WIFI_SSID "SHCVN02"
#define WIFI_PASS "khongduoc"

// AMAZON INFO
// HTTPS for MRLD2 Thing
// Update your Thing Shadow using this Rest API Endpoint.

#define AWS_THING_HTTPS "as76wtq33csyg-ats.iot.us-east-2.amazonaws.com"  // MRLD2-demo
#define AWS_THING_SHADOW "/things/MRLD2/shadow"   // MRLD2-demo
#define AWS_TIME_DOMAIN "us.pool.ntp.org"  // MRLD2-demo

#define AWS_TIME_ZONE           "1"

#define AWS_HTTPS_PORT      "8443"  // default port of AWS IoT (https://docs.aws.amazon.com/iot/latest/developerguide/protocols.html)

void esp32_init(unsigned int baudrate);
void esp32_create_tasks(void);
void esp32_at_test(void);
void esp32_reset_module(void);
void esp32_echo_off(void);
void esp32_wifi_conn(const char* ssid, const char* pssid);
void esp32_ap_query_response(void);
void esp32_ip_status(void);
void esp32_ip_ssl_config(void);
void esp32_ip_ssl_connect(const char* address, const char* port);
void esp32_ip_set_time_domain(const char* timezone, const char* sntp_server);
void esp32_https_post(const char *jsondata);
void esp32_https_get(void);
void esp32_test_post_get(void);

#endif /* __ESP32_AT_H__ */
