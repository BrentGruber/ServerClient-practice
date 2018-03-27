#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Event.h"

char* MY_CLIENT_FIFO = "./myclientfifo";


char* get_com(char *command);
void write_fifo(com input, char *fifo_name);
com read_input(int argc, char *argv[]);
void print_result(com myEvent);
void listen();


int main(int argc, char *argv[])
{

	com myEvent;
	char *fifo_name;

	if(argc < 5)
	{
		printf( "Not enough arguments");
	}
	else if(argc > 8)
	{
		printf( "Too many arguments");
	}
	else
	{
		fifo_name = malloc(strlen(argv[1]));
		strcpy(fifo_name,argv[1]);

		myEvent = read_input(argc, argv);

		//int s = atoi(myEvent.start);
		//int e = atoi(myEvent.end);


		//if(argc >= 7 && e < s)
		//{
			//printf("end cannot be before start\n");
			//return -1;
		//} 


		write_fifo(myEvent, fifo_name);
	}

    return 0;
}

void write_fifo(com input, char *fifo_name)
{
	int fd, ret;
	fd = open(fifo_name, O_WRONLY);
	if(fd < 0)
	{
		fprintf(stderr, "Error: could not open file");
	}
	if(write(fd, (void*)&input, sizeof(input)) < 0)
	{
		fprintf(stderr, "Error: could not write to server\n");
	}
	else
	{
		if(close(fd) < 0)
		{
			fprintf(stderr, "Error: could not close fifo\n");
		}
		listen();
	}
}

com read_input(int argc, char *argv[])
{
		com myEvent;
		int index=2;

		memcpy(myEvent.user, argv[index], MAX_USER);
		index++;
	
		memcpy(myEvent.command, argv[index], MAX_COMMAND);
		index++;

		char date[MAX_DATE];
		memcpy(date, argv[index], MAX_DATE);
		int d = atoi(date);
		myEvent.year = d % 100;
		d = d/100;
		myEvent.md = d;
		index++;


		if(argc > 5)
		{
			memcpy(myEvent.start, argv[index], MAX_START);
			index++;
		}
		if(argc > 6)
		{
			memcpy(myEvent.end, argv[index], MAX_END);
			index++;
		}
		if(argc > 7)
		{
			memcpy(myEvent.name, argv[index], MAX_NAME);
		}


		return myEvent;
}


//listen for response
void listen()
{
    com *myPtr;
    char buf[MAX_BUF];
    char *myfifo = MY_CLIENT_FIFO;
    bool keepreading = true;

    int fd, res;


    if(mkfifo(myfifo, 0666) < 0)
    {
    	printf(stdout, "Error: could not make fifo\n" );
    }
    fd = open(myfifo, O_RDONLY);
	if(fd < 0)
	{
		fprintf(stdout, "Error: could not open file");
	}

    while(keepreading)
    {
        while(read(fd, &buf[0], sizeof(com))>0 || keepreading)
        {
    		com myEvent;
            myPtr = (com*)&buf[0];
            memcpy(&myEvent, myPtr, sizeof(myEvent));
            if(myEvent.response != COM_SUCCESS_MORE_DATA)
            {
            	keepreading = false;
            }
            print_result(myEvent);
        }
        
        if(close(fd) < 0)
        {
        	fprintf(stderr, "Error: could not close fifo\n");
        }
    }
}


void print_result(com myEvent)
{
	printf("Command Executed: %s\n", myEvent.command);

	if(myEvent.response == COM_SUCCESS)
	{
		if(strcmp("get", myEvent.command) == 0)
		{
			print_com(myEvent);
		}
		else
		{
			printf("Operation was successful\n\n");
		}
	}
	else if(myEvent.response == COM_EVENT_NOT_FOUND)
	{
		printf("Could not find event\n\n");
	}
	else if(myEvent.response == COM_EVENT_EXISTS)
	{
		printf("Could not add, event already exist\n\n");
	}
	else if(myEvent.response == COM_SUCCESS_MORE_DATA)
	{
		print_com(myEvent);
	} 
}
