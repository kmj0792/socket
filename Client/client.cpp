//client-라즈베리파이
#include <stdio.h>
#include <stdlib.h>?
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>?
#include <sys/types.h>
#include <sys/socket.h>?
#include <pthread.h>
#include <unistd.h>
#include <wiringPi.h>

#define TRIG_PIN 7?
#define ECHO_PIN 9
#define LED1 25

void camera();
int getDistance();

unsigned long preMillis = 0;

char cmd[] = "sudo raspistill ?o /home/pi/Pictures/test.jpg-t 2000";


static int isThread = 1;

static int sock;


void camera()

{

	if (wiringPiSetup() == -1)

	{

		printf("wiringPi setup fail.");

		return 0;

	}

	pinMode(TRIG_PIN, OUTPUT);

	pinMode(ECHO_PIN, INPUT);





	while (1)

	{

		if (millis() - preMillis >= 500)

		{

			int dis = Distance(); //Distance() 함수 호출

			if (dis <= 10)

			{
				pinMode(LED1, OUTPUT);
				printf("Distance = %dcm\n", dis);
				digitalWrite(LED1, 1);
				delay(300);
				system(cmd);
				digitalWrite(LED1, 0);

			}

			preMillis = millis();

		}


	}

	return 0;

}


int Distance()

{

	unsigned int duration = 0, start_time = 0, end_time = 0;

	float distance = 0;


	digitalWrite(TRIG_PIN, LOW);

	delay(500);

	digitalWrite(TRIG_PIN, HIGH);

	delayMicroseconds(10);

	digitalWrite(TRIG_PIN, LOW);


	while (digitalRead(ECHO_PIN) == LOW)

		start_time = micros();

	while (digitalRead(ECHO_PIN) == HIGH)

		end_time = micros();



	duration = (end_time - start_time) / 2;

	distance = 340 * ((float)duration / 1000000) * 100;



	return (int)distance;

}


void * ThreadFunction(void * arg) {

	char message2[100];

	while (isThread) {

		if (read(sock, message2, sizeof(message2) - 1) == -1) {

			printf("read error\n");

			exit(0);

		}

		printf("msg : %s", message2);

		if (strcmp(message2, "play\n") == 0) {

			camera();  //camera() 함수 호출

		}

	}

	printf("end");

	pthread_exit(0);

}



void main() {

	pthread_t tid;

	pthread_attr_t attr;

	pthread_attr_init(&attr);

	struct sockaddr_in serv_addr;

	char message[100];

	int str_len;

	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {

		printf("socket error\n");

		return;

	}

	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;

	serv_addr.sin_addr.s_addr = inet_addr("192.168.0.36");

	serv_addr.sin_port = htons(23000);



	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {

		printf("connect error\n");

		return;

	}

	printf("connected!!\n");

	isThread = 1;

	pthread_create(&tid, &attr, &ThreadFunction, (void *)0);

	while (1) {

		fgets(message, sizeof(message), stdin);

		if (strcmp(message, "exit\n") == 0) {

			isThread = 0;

			break;

		}

		write(sock, message, sizeof(message));

	}

	close(sock);

}
