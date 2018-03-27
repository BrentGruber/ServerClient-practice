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

#define MY_CLIENT_FIFO "./myclientfifo"


char* get_com(char *command);
void write_fifo(com input, char *fifo_name);
com read_input(int argc, char *argv[]);
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
		write_fifo(myEvent, fifo_name);
	}

    return 0;
}

void write_fifo(com input, char *fifo_name)
{
	int fd, ret;
	fd = open(fifo_name, O_WRONLY);
	if(write(fd, (void*)&input, sizeof(input)) < 0)
	{
		fprintf(stderr, "Error: could not write to server\n");
	}
	else
	{
		listen();
	}
	close(fd);
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


void listen()
{


    com *myPtr;
    char buf[MAX_BUF];
    char *myfifo = MY_CLIENT_FIFO;
    bool keepreading = true;

    int fd, res;

    mkfifo(myfifo, 0666);

    while(keepreading)
    {

	    fd = open(myfifo, O_RDONLY);

        while(read(fd, &buf[0], sizeof(com))>0)
        {
    		com myEvent;
            myPtr = (com*)&buf[0];
            memcpy(&myEvent, myPtr, sizeof(myEvent));
            if(myEvent.response != COM_SUCCESS_MORE_DATA)
            {
            	keepreading = false;
            }
            print_com(myEvent);
        }
        
        close(fd);
    }
}


char* get_com(char *command)
{
	char *c = malloc(MAX_COMMAND);

	if(strcmp(command, "add") == 0)
			memcpy(c, "a", MAX_COMMAND);
	else if(strcmp(command, "remove") == 0)
			memcpy(c, "r", MAX_COMMAND);
	else if(strcmp(command, "update") == 0)
			memcpy(c, "u", MAX_COMMAND);
	else if(strcmp(command, "get") == 0)
			memcpy(c, "g", MAX_COMMAND);
	else
			memcpy(c, "z", MAX_COMMAND);

	return c;
}
