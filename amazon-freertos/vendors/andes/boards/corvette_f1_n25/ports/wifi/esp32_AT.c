//Note:Config moudle ESP32 baud 9600, mode STATION(only connect with router),server singapore
/* Standard includes. */
#include <stdio.h>
#include <string.h>	/* String function definitions */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "platform.h"
#include "esp32_AT.h"
//////////////////////////////
//#include <uart.h>

/*
 * Enum definitions
 */
enum {
	ESP32_TIMEOUT = -1,
	ESP32_OK = 0,
	ESP32_ERROR,
	ESP32_READY,
	ESP32_WIFI_CONNECTED,
	ESP32_WIFI_DISCONNECT,
	ESP32_WIFI_GOT_IP,
	ESP32_CONNECT,
	ESP32_SEND_OK,
	ESP32_SEND_FAIL,
};

enum {
	MSG_AT = 0,
	MSG_RESET,
	MSG_ECHO_OFF,
	MSG_WIFI_QUERY,
	MSG_WIFI_CONNECT,
	MSG_IP_STATUS,
	MSG_IP_SSL_CONFIG,
	MSG_IP_SET_TIME_DOMAIN,
	MSG_IP_SSL_CONNECT,
	MSG_TEST_HTTPS_POST_SET,
	MSG_HTTPS_POST,
	MSG_HTTPS_GET,
};

/*
 * Struct definitions
 */
struct AccessPoint_ST {
	char* ssid;    // the SSID of the target AP
	int ssid_len;
	char* bssid;   // the AP's MAC address
	int bssid_len;
	char* pwd;     // password, MAX: 64-byte ASCII
	int pwd_len;
	char* channel; // channel
	char* rssd;    // signal strength
} s_ap;

struct ESP32_ST
{
    int id;
    void *data;
    int len;
} s_esp32;


#define RECEIVE_BUF_SIZE 1024
char rx_buf[RECEIVE_BUF_SIZE];
char s_ssid[100];
char s_pwd[100];
char s_bssid[100];

static int s_wifi_connect = ESP32_WIFI_DISCONNECT;
static int recv_msg_len = 0;

QueueHandle_t xQueueRx, xQueueTx;
//SemaphoreHandle_t xSemaphore = NULL;

static void prvUartRxTask( void *pvParameters );
static void prvESP32MainTask(void *pvParameters);

static void wifi_conn(const char* ssid, const char* ssid_pass);
static void reset_module(void);
static void echo_off(void);
static void ap_query_response(void);
static void ap_save_id(char* buf);
static int return_message(void);
static int process_message(void);
static int process_message1(char* s, int len);
static void ip_status(void);
static void ip_ssl_config(void);
static void ip_ssl_connect(const char* address, const char* port);
static void ip_set_time_domain(const char* timezone, const char* sntp_server);
static void https_post(const char *jsondata);
static void https_get(void);
static void test_post_get(void);
static void delay_s(int time);
static int receive_1line(char* dst, char* src, size_t len);
static void send_cmd(int id, void* data, int len);
static int read_xQueueRx(char* s, int len, int timeout);
static int clear_xQueueRx(void);
/*
 * Driver UART
 */
static void uart1_init(unsigned int baudrate);
static void uart1_putc(int c);
static int uart1_puts(const char *s);
static int uart1_receive_byte(void);
static int uart1_receive_1line(char* s, size_t len, int ignore);
// DEBUG
static void debug_putc(int c);
static int debug_puts(const char *s);


void esp32_init(unsigned int baudrate)
{
	/* Note:
	 * Because MAX baud rate of Andes board is 34800 < 115200 default baud rate of ESP32 WROOM/WROVER
	 * Use AT command to set baud rate of ESP32 WROOM/WROVER before connect to Andes board
	 * AT+UART_DEF=9600,8,1,0,0$0D$0A
	 */
	printf("Open uart1, baudrate=%d\r\n", baudrate);
	*(unsigned int*)0xF0101004 |= 0xA0; // Change (IO2, IO3) pinmux from (GPIO18, GPIO19) to (UART1_RXD, UART1_TXD)
	uart1_init(baudrate);

	s_ap.ssid = s_ssid;
	s_ap.pwd = s_pwd;
	s_ap.bssid = s_bssid;

	s_wifi_connect = ESP32_WIFI_DISCONNECT;
}

void esp32_at_test(void)
{
	send_cmd(MSG_AT, 0, 0);
	delay_ms(100);
}

static void reset_module(void)
{
	uart1_puts("AT+RST\r\n");
}

void esp32_reset_module(void)
{
	send_cmd(MSG_RESET, 0, 0);
	delay_s(5);
}

static void echo_off(void)
{
	/* ATE0: Switches echo off.
	 * ATE1: Switches echo on */
	uart1_puts("ATE0\r\n");
	delay_ms(100);
	if(return_message() == ESP32_OK){
		printf("ESP32 Switches echo off\r\n");
	}else{
		printf("ESP32 Switches echo off ERROR\r\n");
	}
}
void esp32_echo_off(void)
{
	send_cmd(MSG_ECHO_OFF, 0, 0);
	delay_ms(100);
}

void ap_query_response(void)
{
    /* Query the AP to which the ESP32 Station is already connected */
    uart1_puts("AT+CWJAP?\r\n");
}
void esp32_ap_query_response(void)
{
	send_cmd(MSG_WIFI_QUERY, 0, 0);
	delay_ms(100);
}

static void wifi_conn(const char* ssid, const char* pssid)
{
    /* Set Wifi mode (1: station mode) */
    uart1_puts("AT+CWMODE=1\r\n");
    delay_ms(100);

    uart1_puts("AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PASS "\"\r\n");
    delay_s(4);
}

void esp32_wifi_conn(const char* ssid, const char* pssid)
{
	send_cmd(MSG_WIFI_CONNECT, 0, 0);
	delay_s(5);
}

static void ip_status(void)
{
    uart1_puts("AT+CIPSTATUS\r\n");
}

void esp32_ip_status(void)
{
	send_cmd(MSG_IP_STATUS, 0, 0);
	delay_ms(100);
}

static void ip_ssl_config(void)
{
    /* Set Configuration of SSL Client */
    uart1_puts("AT+CIPSSLCCONF=3,0,0\r\n");
}

void esp32_ip_ssl_config(void)
{
	send_cmd(MSG_IP_SSL_CONFIG, 0, 0);
	delay_ms(100);
}

static void ip_ssl_connect(const char* address, const char* port)
{
    /* Establish SSL connection */
    uart1_puts("AT+CIPSTART=\"SSL\",\"" AWS_THING_HTTPS "\"," AWS_HTTPS_PORT "\r\n");
}

void esp32_ip_ssl_connect(const char* address, const char* port)
{
	send_cmd(MSG_IP_SSL_CONNECT, 0, 0);
	delay_s(6);
}

static void ip_set_time_domain(const char* timezone, const char* sntp_server)
{
	/* Example: AT+CIPSNTPCFG=1,1,"sg.pool.ntp.org"\r\n */
    /* Configures the time domain and SNTP server */
    uart1_puts("AT+CIPSNTPCFG=1," AWS_TIME_ZONE ",\"" AWS_TIME_DOMAIN "\"\r\n");
    if(return_message() != ESP32_OK)
    {
    	printf("%s return error", __func__);
    }
}

void esp32_ip_set_time_domain(const char* timezone, const char* sntp_server)
{
	send_cmd(MSG_IP_SET_TIME_DOMAIN, 0, 0);
}

//static void ip_send(const char *s)
//{
//    char cmd[512];
//    sprintf(cmd, "AT+CIPSEND=%d\r\n", strlen(s));
//    uart1_puts(cmd);
//    uart1_puts(s); /* Send data after '>' sign */
//
//
//}

static void test_post_get(void)
{
	if(0)
	{
		uart1_puts("AT+CIPSEND=195\r\n");
		delay_ms(100);
		uart1_puts("POST /things/MRLD2/shadow HTTP/1.1\r\nHost: as76wtq33csyg-ats.iot.us-east-2.amazonaws.com\r\nContent-Type: application/json\r\nContent-Length: 52\r\n\r\n{\"state\":{\"desired\":{\"speed\":\"29\"},\"reported\":{}}}\r\n");
		delay_ms(100);
	}
	if(0)
	{
		uart1_puts("AT+CIPSEND=195\r\n");
		delay_ms(100);
	    uart1_puts("POST " AWS_THING_SHADOW " HTTP/1.1\r\nHost: " AWS_THING_HTTPS "\r\nContent-Type: application/json\r\nContent-Length: 52\r\n\r\n{\"state\":{\"desired\":{\"speed\":\"33\"},\"reported\":{}}}\r\n");
	    delay_ms(100);
	}

	if(0)
	{
		uart1_puts("AT+CIPSEND=90\r\n");
		delay_ms(100);
		uart1_puts("GET " AWS_THING_SHADOW " HTTP/1.1\r\nHost: " AWS_THING_HTTPS "\r\n\r\n");
		delay_ms(100);
	}


}
void esp32_test_post_get(void)
{
	send_cmd(MSG_TEST_HTTPS_POST_SET, 0, 0);
}

static void https_post(const char *jsondata)
{
	char str[200];
	char str2[50];
	sprintf(str , "POST " AWS_THING_SHADOW " HTTP/1.1\r\nHost: " AWS_THING_HTTPS "\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s", strlen(jsondata), jsondata);
	sprintf(str2, "AT+CIPSEND=%d\r\n", strlen(str));
	uart1_puts(str2);
	delay_ms(100);
	uart1_puts(str); /* Send data after '>' sign */
	delay_ms(100);
}

void esp32_https_post(const char *jsondata)
{
	send_cmd(MSG_HTTPS_POST, (void*) jsondata, 0);
	delay_s(1);
}

// Notice, the GET-Request needs to be terminated by two CRLF!
static void https_get(void)
{
	char str[100] = "GET " AWS_THING_SHADOW " HTTP/1.1\r\nHost: " AWS_THING_HTTPS "\r\n\r\n";
	char str2[50];
//	char s[200];
	sprintf(str2, "AT+CIPSEND=%d\r\n", strlen(str));

	uart1_puts(str2);
	delay_ms(100);
//	clear_xQueueRx(); // cleared queue before GET
    uart1_puts(str); /* Send data after '>' sign */
    delay_ms(100);
//    read_xQueueRx(s, sizeof(s), 1000); // read queue
}

void esp32_https_get(void)
{
	send_cmd(MSG_HTTPS_GET, 0, 0);
	delay_s(1);
}

static int return_message(void)
{
    // Parse AT response to get the result
	int i = 0;
    int count = 0;
    char tmp[256];
    int ret = ESP32_ERROR;
    do {
        count = uart1_receive_1line(tmp, RECEIVE_BUF_SIZE, 0);
        if(count == -1)
        {
        	return ESP32_TIMEOUT;
        }

        tmp[count] = 0; // Add null character
        strncpy((char*)&rx_buf[i], tmp, count);

        if (memcmp(tmp, "OK", 2) == 0) {
        	i += count;
        	ret = ESP32_OK;
            break;
        } else if (memcmp(tmp, "ERROR", 5) == 0) {
        	i += count;
        	ret = ESP32_ERROR;
        	break;
        }

        i += count;
        if(i >= RECEIVE_BUF_SIZE)
        	break;
    }
    while (count > 0);
    recv_msg_len = i;
    rx_buf[i] = 0; // Add null character
    return ret;
}

static void ap_save_id(char* buf)
{//"+CWJAP:"ABCDEFG","48:f8:b3:2e:f1:76",1,-61"
	char* c = (char*)&buf[6];
	char* start_buf = c+1;
	int len = 0;
	while (*++c != ',') { len++; }
	*c = 0; /* null character manually added before calling strcpy*/
	strncpy(s_ap.ssid, (char*)&start_buf[1], len-2); // "abc" -> abc
	s_ap.ssid_len = len-2;
	printf("s_ap.ssid %s\r\n", s_ap.ssid);

	len = 0;
	start_buf = c+1;
	while (*++c != ',') { len++; }
	*c = 0;
	strncpy(s_ap.bssid, (char*)&start_buf[1], len-2); // "abc" -> abc
	s_ap.bssid_len = len-2;
	printf("s_ap.bssid %s\r\n", s_ap.bssid);
}

static int process_message(void)
{
    // Parse AT response to get the result
    int count;
    char *s = rx_buf;
    int len = recv_msg_len;
    char tmp[256];
    do {
    	count = receive_1line(tmp, s , len);
    	s += count; // move to next line

        if (count >= 2) tmp[count - 2] = 0;

        // Process each line
        if (memcmp(tmp, "CONNECT", 7) == 0) {
//            return ESP32_CONNECT;
        } else if (memcmp(tmp, "SEND OK", 7) == 0) {
//            return ESP32_SEND_OK;
        } else if (memcmp(tmp, "WIFI CONNECTED", 14) == 0) {
        	s_wifi_connect = ESP32_WIFI_CONNECTED;
//        	return ESP32_WIFI_CONNECTED;
		}
        else if (memcmp(tmp, "WIFI DISCONNECT", 14) == 0) {
        	s_wifi_connect = ESP32_WIFI_DISCONNECT;
//        	return ESP32_WIFI_DISCONNECT;
       	}
        else if (memcmp(tmp, "WIFI GOT IP", 11) == 0) {
//			return ESP32_WIFI_GOT_IP;
		}
        else if (memcmp(tmp, "ready", 5) == 0) {
//			return ESP32_READY;
		}
        else if (memcmp(tmp, "+CWJAP:", 7) == 0)
        {
        	ap_save_id(tmp);
        }
        len -= count;
    }
    while (len > 0);
    return 0;
}

static int process_message1(char* s, int len)
{
    char *tmp = s;
    printf("esp32(%d)> %s", len, s);
	if (len >= 2) tmp[len - 2] = 0;

	// Process each line
	if (memcmp(tmp, "CONNECT", 7) == 0) {
	} else if (memcmp(tmp, "SEND OK", 7) == 0) {
	} else if (memcmp(tmp, "WIFI CONNECTED", 14) == 0) {
		debug_puts("wifi connected\r\n");
		s_wifi_connect = ESP32_WIFI_CONNECTED;
	}
	else if (memcmp(tmp, "WIFI DISCONNECT", 14) == 0) {
		debug_puts("wifi disconnect\r\n");
		s_wifi_connect = ESP32_WIFI_DISCONNECT;
	}
	else if (memcmp(tmp, "WIFI GOT IP", 11) == 0) {
	}
	else if (memcmp(tmp, "ready", 5) == 0) {
	}
	else if (memcmp(tmp, "+CWJAP:", 7) == 0)
	{
		ap_save_id(tmp);
	}
    return 0;
}
void esp32_create_tasks(void)
{
	printf( "%s\r\n", __func__ );
//  xSemaphore = xSemaphoreCreateMutex();
//  if ( ( xSemaphore ) != NULL )
//      xSemaphoreGive( ( xSemaphore ) );
//
  xQueueRx = xQueueCreate( 512, sizeof( char ) );
  if( xQueueRx == NULL )
  {
      /* Queue was not created and must not be used. */
      printf("xQueueRx was not created\r\n");
  }

  xQueueTx = xQueueCreate( 10, sizeof( s_esp32 ) );
  if( xQueueTx == NULL )
  {
      /* Queue was not created and must not be used. */
      printf("xQueueTx was not created\r\n");
  }

  xTaskCreate( prvUartRxTask, "UartRxTask", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 1 ), NULL );
  xTaskCreate( prvESP32MainTask, "prvESP32MainTask", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 1 ), NULL );

}

static void prvESP32MainTask(void *pvParameters)
{
	for(;;)
	{
		if( xQueueTx != 0 )
		{
			if( xQueueReceive( xQueueTx, ( void * )&s_esp32 , ( TickType_t ) 100 ) != pdPASS )
			{
			  /* Failed to post the message, even after 100 ticks. */
			  //  debug_puts("xQueueTx Full\r\n");
			} else
			{
				int id = s_esp32.id;
				switch (id)
				{
				case MSG_AT:
				{
					uart1_puts("AT\r\n");
					break;
				}
				case MSG_RESET:
				{
					reset_module();
					break;
				}

				case MSG_ECHO_OFF:
				{
					echo_off();
					break;
				}
				case MSG_WIFI_CONNECT:
				{
					wifi_conn(WIFI_SSID, WIFI_PASS);
					break;
				}
				case MSG_WIFI_QUERY:
				{
					ap_query_response();
					break;
				}
				case MSG_IP_STATUS:
				{
					ip_status();
					break;
				}
				case MSG_IP_SSL_CONFIG:
				{
					ip_ssl_config();
					break;
				}
				case MSG_IP_SET_TIME_DOMAIN:
				{
					ip_set_time_domain(AWS_TIME_ZONE, AWS_TIME_DOMAIN);
					break;
				}
				case MSG_IP_SSL_CONNECT:
				{
					ip_ssl_connect(AWS_THING_HTTPS, AWS_HTTPS_PORT);
					break;
				}
				case MSG_TEST_HTTPS_POST_SET:
				{
					test_post_get();
					break;
				}
				case MSG_HTTPS_POST:
				{
					https_post(s_esp32.data);
					break;
				}
				case MSG_HTTPS_GET:
				{
					https_get();
					break;
				}
				default:
				{
					break;
				}
				}
			}
		} else
		{
			debug_puts("xQueueTx was not created\r\n");
		}
	}
}

static void prvUartRxTask( void *pvParameters )
{
	/* Prevent the compiler warning about the unused parameter. */
	( void ) pvParameters;
	char c;
	int i = 0;
	char s[200];
//	xSemaphoreTake(xSemaphore, portMAX_DELAY);
//	printf("prvUartRxTask init\r\n");
//	xSemaphoreGive(xSemaphore);
	for( ;; )
	{
		if( xQueueRx != 0 )
		{
			c = uart1_receive_byte();
			if(i == 0)
			{
				debug_puts("esp32> ");
			}
			 debug_putc((int)c);  // DEBUG

			/* Collect 1 line */
			 i++;
			s[i] = c;
			if(c == '\n')
			{
				s[i] = 0;
//				printf("esp32(%d)> %s", i, s);
//				process_message1(s, i);
				i = 0;
			}

			if( xQueueSend( xQueueRx, ( void * ) &c, ( TickType_t )0  /*portMAX_DELAY*/ ) != pdPASS )
			{
			  /* Failed to post the message, even after 10 ticks. */
			}
		}
	}
}

static void send_cmd(int id, void* data, int len)
{
    s_esp32.id = id;
    s_esp32.data = data;
    s_esp32.len = len;
    if( xQueueSend( xQueueTx, ( void * ) &s_esp32,( TickType_t ) 2000 ) != pdPASS )
    {
      /* Failed to post the message */
      printf("%s: xQueueTx was full\r\n", __func__);
    }
}

static int read_xQueueRx(char* s, int len, int timeout)
{
	char c;
	int i = 0;
	do
	{
		if( xQueueReceive( xQueueRx, ( void * ) &c, ( TickType_t ) 1000 ) != pdPASS )
		{
			s[i] = 0;
			printf("xQueueRx cleared\r\n");
			break;
		}
		s[i++] = c;
	}while(i < len);
	return 0;
}

static int clear_xQueueRx(void)
{
	char c;
	do
	{
		if( xQueueReceive( xQueueRx, ( void * ) &c, ( TickType_t ) 0 ) != pdPASS )
		{
			printf("xQueueRx is cleared\r\n");
			break;
		}
	}while(1);
	return 0;
}

static void delay_s(int time)
{
//    while(time--);
    vTaskDelay( ( TickType_t )(1000*time) / portTICK_PERIOD_MS );//delay 1s
}

void delay_ms(int ms)
{
    vTaskDelay( ( TickType_t )ms / portTICK_PERIOD_MS ); //delay ms
}

// Get 1 line in receive buffer
static int receive_1line(char* dst, char* src, size_t len)
{
    int i = 0;
    dst[0] = 0;
    for (i = 0; i < len; i++) {
    	dst[i] = src[i];
//    	*(dst++) = *(src++);
        if(dst[i] == '\r')
        {
        	i++;
        	dst[i] = src[i];
            if(dst[i] == '\n')
            {
            	i++;
            	break;
            }
        }
    }
    return i;
}
/************************************************************
 * UART driver
 ************************************************************/

#define BAUD_RATE(n)            (UCLKFREQ / (n) / 16)
#define DEFAULT_BAUDRATE	38400

static void uart1_init(unsigned int baudrate)
{
	/* Clear everything */
	DEV_UART1->IER = 0UL;
	DEV_UART1->LCR = 0UL;

	uart_set_baudrate(DEV_UART1, baudrate);
//	/* Set DLAB to 1 */
//	DEV_UART1->LCR |= 0x80;
//
//	/* Set DLL for baudrate */
//	DEV_UART1->DLL = (BAUD_RATE(baudrate) >> 0) & 0xff;
//	DEV_UART1->DLM = (BAUD_RATE(baudrate) >> 8) & 0xff;

	/* LCR: Length 8, no parity, 1 stop bit. */
	DEV_UART1->LCR = 0x03;

	/* FCR: Enable FIFO, reset TX and RX. */
	DEV_UART1->FCR = 0x07;
}

static void uart1_putc(int c)
{
#define SERIAL_LSR_THRE         0x20
	while ((DEV_UART1->LSR & SERIAL_LSR_THRE) == 0) ;

	DEV_UART1->THR = c;
}

static int uart1_puts(const char *s)
{
	int len = 0;
	debug_puts(s); // DEBUG
	while (*s) {
		uart1_putc(*s);
		s++;
		len++;
	}
	return len;
}

static int uart1_receive_byte(void)
{
	while(!(DEV_UART1->LSR & 0x1));
	return DEV_UART1->RBR;
}

//static void uart1_set_baudrate(unsigned int baudrate)
//{
//	unsigned int div;
//
//	/*
//	 * Oversampling is fixed to 16
//	 * divider = frequency of uclk / (desired baud rate x oversampling)
//	 */
//	div = (UCLKFREQ / (16 * baudrate));
//
//	DEV_UART1->LCR |= 0x80;
//	DEV_UART1->DLL  = (div >> 0) & 0xff;
//	DEV_UART1->DLM  = (div >> 8) & 0xff;
//	DEV_UART1->LCR &= (~0x80);	/* Reset DLAB bit */
//}

static int uart1_receive_1line(char* s, size_t len, int ignore)
{
    int received = 0;
    int match_count = 0;
    s[0] = 0;
    for (received = 0; received < len; received++) {
//        s[received] = uart1_receive_byte();
		if( xQueueReceive( xQueueRx, ( void * ) &s[received], ( TickType_t ) 5000 ) != pdPASS )
		{
		  debug_puts("timeout\r\n");
		  s[received] = 0;
		  return -1; // timeout
		}

		if(s[received] == '\r')
        {
        	match_count++;
        	received++;
//        	s[received] = uart1_receive_byte();
    		if( xQueueReceive( xQueueRx, ( void * ) &s[received], ( TickType_t ) 5000 ) != pdPASS )
    		{
    		  debug_puts("timeout\r\n");
    		  s[received] = 0;
    		  return -1; // timeout
    		}

            if(s[received] == '\n')
            {
            	match_count++;
            	received++;
            	break;
            }
        }
    }
    /* Ignore "\r\n" */
    if(ignore == 1)
    	if(received == 2)
    		s[0] = 0;

    return received;
}

// DEBUG
static void debug_putc(int c)
{
#define SERIAL_LSR_THRE         0x20
	while ((DEV_UART2->LSR & SERIAL_LSR_THRE) == 0) ;

	DEV_UART2->THR = c;
}

static int debug_puts(const char *s)
{
	int len = 0;

	while (*s) {
		debug_putc(*s);
		s++;
		len++;
	}
	return len;
}

