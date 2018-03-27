#ifndef EVENT_H
#define EVENT_H

#define MAX_BUF 1096
#define INPUT 256
#define MAX_USER 11
#define MAX_COMMAND 10
#define MAX_DATE 7
#define MAX_START 6
#define MAX_END 6
#define MAX_NAME 21
#define COM_SUCCESS_MORE_DATA 2
#define COM_SUCCESS 1
#define COM_EVENT_NOT_FOUND -1
#define COM_EVENT_EXISTS -2
#define COM_GEN_FAIL -99


typedef struct
{
	char command[MAX_COMMAND];
	char date[MAX_DATE];
	char start[MAX_START];
	char end[MAX_END];
	char user[MAX_USER];
	char name[MAX_NAME];
	char retfifo[MAX_NAME];
	int  year;
	int  md;
	int  response;

} com;


void print_com(com input)
{
	printf("\nUser: %s\n", input.user);
	printf("Event: %s\n", input.name);	
	printf("Start time: %s\n", input.start);
	printf("End time: %s\n", input.end);
	printf("MMDD: %d\n", input.md);
	printf("YY: %d\n", input.year);
	printf("command: %s\n", input.command);
	printf("Server Response: %d\n\n", input.response);
}


#endif
