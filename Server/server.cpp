#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <process.h>

#include <WinSock2.h>

#include <iostream>

#pragma comment(lib, "ws2_32.lib")

static int isThread = 1;

static int client_sock = 0;

static char buf2[255];

unsigned __stdcall ThreadFunsion(void *arg) {

	while (isThread) {

		if (recv(client_sock, buf2, 255, 0) <= 0) return 0;// read error

		printf("¼ö½Å : %s", buf2);

		if (strcmp(buf2, "exit\n") == 0) {

			isThread = 0;

			exit(0);

		}

	}

	return 0;

}



void main() {

	int server_sock = 0;

	int client_len = 0;

	WSADATA WSAdata;

	::WSAStartup(WINSOCK_VERSION, &WSAdata);

	struct sockaddr_in clientaddr;

	struct sockaddr_in serveraddr;

	char buf[255];

	client_len = sizeof(clientaddr);

	if ((server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)

		exit(0); // socket error

	memset(&serveraddr, 0, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;

	serveraddr.sin_addr.s_addr = INADDR_ANY;

	serveraddr.sin_port = htons(23000);

	if (bind(server_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1)

		exit(0);// bind error

	if (listen(server_sock, 5) == -1)

		exit(0);// listen error

	printf("Waiting¡¦\n");

	client_sock = accept(server_sock, (struct sockaddr*)&clientaddr, &client_len);

	if (client_sock == -1) {

		printf("accept errer\n");

		return;//accept error

	}



	isThread = 1;

	_beginthreadex(NULL, 0, ThreadFunsion, 0, 0, NULL);

	while (1) {

		fgets(buf, sizeof(buf), stdin);

		send(client_sock, buf, sizeof(buf), 0);

		if (strcmp(buf, "exit\n") == 0) {

			isThread = 0;

			break;

		}

	}

	closesocket(client_sock);

	::WSACleanup();

}