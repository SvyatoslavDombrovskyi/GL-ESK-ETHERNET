#include "simple_http_server.h"
#include "main.h"
#include "lwip.h"
#include "sockets.h"
#include "cmsis_os.h"
#include <string.h>

#define SVERSION "udp_srv_Svyatoslav_Dombrovskyi_14042023\n"

#define PORTNUM 5678UL

#if (USE_UDP_SERVER_PRINTF == 1)
#include <stdio.h>
#define UDP_SERVER_PRINTF(...) do { printf("[udp_server.c: %s: %d]: ",__func__, __LINE__);printf(__VA_ARGS__); } while (0)
#else
#define UDP_SERVER_PRINTF(...)
#endif

static struct sockaddr_in serv_addr, client_addr;
static int socket_fd;
static uint16_t nport;

void UdpReturnMessage(char* buffer, socklen_t addr_len);


static int udpServerInit(void)
{
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd == -1) {
		UDP_SERVER_PRINTF("socket() error\n");
		return -1;
	}

	nport = PORTNUM;
	nport = htons((uint16_t)nport);

	bzero(&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = nport;

	if(bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))==-1) {
		UDP_SERVER_PRINTF("bind() error\n");
		close(socket_fd);
		return -1;
	}

	UDP_SERVER_PRINTF("Server is ready\n");
	return 0;
}

void StartUdpServerTask(void const * argument)
{
	osDelay(5000);// wait 5 sec to init lwip stack

	if(udpServerInit() < 0) {
		UDP_SERVER_PRINTF("udpSocketServerInit() error\n");
		return;
	}

	for(;;)
	{
		  bzero(&client_addr, sizeof(client_addr));

		  int lengthBytes;
		  const size_t buf_len = 256;
		  char buffer[buf_len];

		  for (int i=0; i < buf_len; i++)
		  {
			  buffer[i]='0';
		  }

		  socklen_t addr_len = sizeof(client_addr);

		  while ( (lengthBytes = recvfrom(socket_fd, buffer, (size_t)sizeof(buffer), 0, (struct sockaddr *)&client_addr, (socklen_t*)&addr_len)) > 0 )
		  {
			//exit
			if (strncmp(buffer, "exit", strlen("exit")) == 0)
			{
				sendto(socket_fd, "goodbye!", strlen("goodbye!"), 0, (const struct sockaddr*)&client_addr, addr_len);
				break;
			}

			UdpReturnMessage(buffer, addr_len);

			//echo
//			if (sendto(socket_fd, buffer, lengthBytes, 0, (const struct sockaddr*)&client_addr, addr_len) < 0)
//			{
//				UDP_SERVER_PRINTF("send() error\n");
//			}
		  }

		  close(socket_fd);
	}

}

void UdpReturnMessage(char* buffer, socklen_t addr_len)
{
	//sversion
	if (strncmp(buffer, "sversion", strlen("sversion")) == 0)
	{
		sendto(socket_fd, SVERSION, strlen(SVERSION), 0, (const struct sockaddr*)&client_addr, addr_len);
		sendto(socket_fd, "OK\n", strlen("OK\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		return;
	}

	//led3 - ORANGE
	else if (strncmp(buffer, "led3 on",strlen("led3 on")) == 0)
	{
		BSP_LED_On(ORANGE);
		sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		return;
	}
	else if (strncmp(buffer, "led3 off",strlen("led3 off")) == 0)
	{
		BSP_LED_Off(ORANGE);
		sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		return;
	}
	else if (strncmp(buffer, "led3 toggle",strlen("led3 toggle")) == 0)
	{
		BSP_LED_Toggle(ORANGE);
		sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		return;
	}
	else if (strncmp(buffer, "led3 status",strlen("led3 status")) == 0)
	{
		GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_13);
		if (pinState == GPIO_PIN_SET)
		{
			sendto(socket_fd, "LED3 ON\r\n", strlen("LED3 ON\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
			sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		}
		else if (pinState == GPIO_PIN_RESET)
		{
			sendto(socket_fd, "LED3 OFF\r\n", strlen("LED3 OFF\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
			sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		}
		return;
	}

	//led4 - GREEN
	else if (strncmp(buffer, "led4 on", strlen("led4 on")) == 0)
	{
		BSP_LED_On(GREEN);
		sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		return;
	}
	else if (strncmp(buffer, "led4 off", strlen("led4 off")) == 0)
	{
		BSP_LED_Off(GREEN);
		sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		return;
	}
	else if (strncmp(buffer, "led4 toggle", strlen("led4 toggle")) == 0)
	{
		BSP_LED_Toggle(GREEN);
		sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		return;
	}
	else if (strncmp(buffer, "led4 status", strlen("led4 status")) == 0)
	{
		GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_12);
		if (pinState == GPIO_PIN_SET)
		{
			sendto(socket_fd, "LED4 ON\r\n", strlen("LED4 ON\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
			sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		}
		else if (pinState == GPIO_PIN_RESET)
		{
			sendto(socket_fd, "LED4 OFF\r\n", strlen("LED4 OFF\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
			sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		}
		return;
	}

	//led5 - RED
	else if (strncmp(buffer, "led5 on", strlen("led5 on")) == 0)
	{
		BSP_LED_On(RED);
		sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		return;
	}
	else if (strncmp(buffer, "led5 off", strlen("led5 off")) == 0)
	{
		BSP_LED_Off(RED);
		sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		return;
	}
	else if (strncmp(buffer, "led5 toggle", strlen("led5 toggle")) == 0)
	{
		BSP_LED_Toggle(RED);
		sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		return;
	}
	else if (strncmp(buffer, "led5 status", strlen("led5 status")) == 0)
	{
		GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_14);
		if (pinState == GPIO_PIN_SET)
		{
			sendto(socket_fd, "LED5 ON\r\n", strlen("LED5 ON\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
			sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		}
		else if (pinState == GPIO_PIN_RESET)
		{
			sendto(socket_fd, "LED5 OFF\r\n", strlen("LED5 OFF\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
			sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		}
		return;
	}

	//led6 - BLUE
	else if (strncmp(buffer, "led6 on", strlen("led6 on")) == 0)
	{
		BSP_LED_On(BLUE);
		sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		return;
	}
	else if (strncmp(buffer, "led6 off", strlen("led6 off")) == 0)
	{
		BSP_LED_Off(BLUE);
		sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		return;
	}
	else if (strncmp(buffer, "led6 toggle", strlen("led6 toggle")) == 0)
	{
		BSP_LED_Toggle(BLUE);
		sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		return;
	}
	else if (strncmp(buffer, "led6 status", strlen("led6 status")) == 0)
	{
		GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15);
		if (pinState == GPIO_PIN_SET)
		{
			sendto(socket_fd, "LED6 ON\r\n", strlen("LED6 ON\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
			sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		}
		else if (pinState == GPIO_PIN_RESET)
		{
			sendto(socket_fd, "LED6 OFF\r\n", strlen("LED6 OFF\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
			sendto(socket_fd, "OK\r\n", strlen("OK\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
		}
		return;
	}

	//error
	else
	  {
		sendto(socket_fd, "ERROR\r\n", strlen("ERROR\r\n"), 0, (const struct sockaddr*)&client_addr, addr_len);
	  }

}
