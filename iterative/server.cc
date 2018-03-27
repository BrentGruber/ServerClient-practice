#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include "Event.h"

//name of fifo to read from
#define MY_SERVER_FIFO "./myserverfifo"
#define MY_SERVER_OUT_FIFO "./myclientfifo"

using std::vector;
using std::cout;
using std::endl;

//this is how events will be stored in the calendar
struct event
{
    int  year;
    int  md;
    char start[MAX_START];
    char end[MAX_END];
    char user[MAX_USER];
    char name[MAX_NAME];
};

//calendar
static vector<struct event> events; 

//menu and functions to perform desired actions
vector<com> menu(com myCom);
int add_event(struct event myEvent);
int remove_event(struct event myEvent);
int update_event(struct event myEvent);
vector<struct event> get_event(struct event myEvent);

//helper functions to perform actions for above functions
struct event convert(com myCom);
com convert_back(struct event myEvent);
vector<struct event>::iterator search_events(struct event myEvent);
bool event_eq(struct event first, struct event second);

//reply back to client
void reply(vector<com> v);

//print out an event object
void print_event(struct event myEvent)
{
    cout << "\nprinting event" << endl;
    cout << myEvent.user << endl;
    cout << myEvent.md << myEvent.year << endl;
    cout << myEvent.start << endl;
    cout << myEvent.end << endl;
    cout << myEvent.name << endl;
    cout << "done printing\n" << endl;
}




void print_list()
{
    int counter = 1;

    for(vector<struct event>::iterator it=events.begin(); it != events.end(); ++it)
	{
        cout << "\n" << counter << ".) ";
		cout << (*it).name << endl;
		cout << (*it).user << endl;
        cout << (*it).md << (*it).year << endl;
        cout << (*it).start << endl;
        cout << (*it).end << endl;
        cout << endl;
        counter++;
	}

    cout << "size = " << events.size() << endl;
}


int main()
{
    //set fifo information
    int fd, err;
    char * myfifo = MY_SERVER_FIFO;
    char buf[MAX_BUF];

    //used to read and stor input
    com myInput;
    com *myPtr;

    //responses to send back, probably just 1, but could be get query
    vector<com> responses;

    printf("%s", myfifo);

    /* create the FIFO (named pipe) */
    err = mkfifo(myfifo, S_IRUSR| S_IWUSR);
    if(err < 0)
    {
        fprintf(stderr, "Error: could not make fifo %d", err);
    }

    /* open, read, and display the message from the FIFO */
    while(1)
    {
        fd = open(myfifo, O_RDONLY);
        if(fd < 0)
        {
            fprintf(stderr, "Error: could not open file");
        }

        while(read(fd, &buf[0], sizeof(myInput))>0)
        {
            printf("Data Received\n");

            //read the data into a structure
            myPtr = (com*)&buf[0];
            memcpy(&myInput, myPtr, sizeof(myInput));

            //get the responses and reply
            responses = menu(myInput);
            reply(responses);
        }
        
        //close the fifo
        if(close(fd) != 0)
        {
            fprintf(stderr, "Error: could not close file" );
        }
    }

    /* remove the FIFO */
    if(unlink(myfifo) != 0)
    {
        fprintf(stderr, "Error: could not remove fifo " );
    }

    return 0;
}

//read in command from client and perform desired action
vector<com> menu(com myCom)
{
    vector<com> v;
    struct event myEvent = convert(myCom);
    com resp;


    if(strcmp(myCom.command, "add") == 0)
    {
            myCom.response = add_event(myEvent);
            v.push_back(myCom);
    }
    else if(strcmp(myCom.command, "remove") == 0)
    {
            myCom.response = remove_event(myEvent);
            v.push_back(myCom);
    }
    else if(strcmp(myCom.command, "update") == 0)
    {
            myCom.response = update_event(myEvent);
            v.push_back(myCom);
    }
    else if(strcmp(myCom.command, "get") == 0)
    {
           vector<struct event> found = get_event(myEvent);
           memcpy(myCom.command, "get", sizeof("get"));
           if(found.empty())
           {
                myCom.response = COM_EVENT_NOT_FOUND;
                v.push_back(myCom);
                print_list();
                return v;
           }
           for(vector<struct event>::iterator it = found.begin(); it != found.end(); it++)
           {
                myCom = convert_back((*it));
                memcpy(myCom.command, "get", sizeof("get"));
                myCom.response = COM_SUCCESS_MORE_DATA;
                v.push_back(myCom);
           }
           v.back().response = COM_SUCCESS;
    }
    else
    {
            myCom.response = COM_GEN_FAIL;
    }

    print_list();

    return v;
}



//add an event to the calendar
int add_event(struct event myEvent)
{
    cout << "inserting event" << endl;

    //list is empty, just insert new item and be done
    if(events.empty())
    {
        events.push_back(myEvent);
        return COM_SUCCESS;
    }

   vector<struct event>::iterator it = events.begin();

   //place the iterator at this users section
   while(it != events.end() && (strcmp((*it).user, myEvent.user) < 0))
   {
     it++;
   }

   //search through this user's entries to find where to place new entry
   while(it != events.end() && (strcmp((*it).user, myEvent.user) == 0) && (*it).year <= myEvent.year && (*it).md <= myEvent.md)
   {
     if(event_eq((*it), myEvent))
     {
        return COM_EVENT_EXISTS;
     }
     it++;
   }

   events.insert(it, myEvent);
   return COM_SUCCESS;
}

//remove an event from the calendar
int remove_event(struct event myEvent)
{
    cout << "removing event" << endl;

    vector<struct event>::iterator it = events.begin();
    it = search_events(myEvent);

    if(it == events.end())
    {
        //item not found, cannot delete
        return COM_EVENT_NOT_FOUND;
    }
    else
    {
        //found item, remove it from list
        events.erase(it);
        return COM_SUCCESS;
    }
}

int update_event(struct event myEvent)
{
    cout << "updating event" << endl;

    vector<struct event>::iterator it = events.begin();
    bool dates = ((*it).md == myEvent.md && (*it).year == myEvent.year);

    while(it != events.end())
    {

    if(strcmp((*it).user, myEvent.user) == 0 && dates &&  
       ((myEvent.start[0] == 0 ||(myEvent.start[0] != 0 && strcmp((*it).start, myEvent.start) == 0)) ||
       (myEvent.end[0] == 0 ||(myEvent.end[0] != 0 && strcmp((*it).end, myEvent.end) == 0)) ||
       (myEvent.name[0] == 0 ||(myEvent.name[0] != 0 && strcmp((*it).name, myEvent.name) == 0))))
        {
            //found the matching event, update the information
            memcpy((*it).user, myEvent.user, sizeof((*it).user));
            memcpy((*it).start, myEvent.start, sizeof((*it).start));
            memcpy((*it).end, myEvent.end, sizeof((*it).end));
            memcpy((*it).name, myEvent.name, sizeof((*it).name));
            (*it).year = myEvent.year;
            (*it).md = myEvent.md;
            return COM_SUCCESS;
        }
        ++it;
    }
    
    //event not found, cannot update
    return COM_EVENT_NOT_FOUND;
}


//return a list of stored events that match the input event
vector<struct event> get_event(struct event myEvent)
{
    cout << "getting event" << endl;

    vector<struct event> retvalue;
    vector<struct event>::iterator it = events.begin();
    bool found = false;

    while(it != events.end())
    {
        bool check = event_eq((*it), myEvent);
        if(check)
        {   found = true;
            retvalue.push_back(*it);
        }

        ++it;
    }

    return retvalue;
}

//convert from com to event
struct event convert(com myCom)
{
    struct event myEvent;

    memcpy(myEvent.user, myCom.user, MAX_USER);
    memcpy(myEvent.start, myCom.start, MAX_START);
    memcpy(myEvent.end, myCom.end, MAX_END);
    memcpy(myEvent.name, myCom.name, MAX_NAME);
    myEvent.year = myCom.year;
    myEvent.md = myCom.md;

    return myEvent;
}

//convert from event to com
com convert_back(struct event myEvent)
{
    com myCom;

    memcpy(myCom.user, myEvent.user, MAX_USER);
    memcpy(myCom.start, myEvent.start, MAX_START);
    memcpy(myCom.end, myEvent.end, MAX_END);
    memcpy(myCom.name, myEvent.name, MAX_NAME);
    myCom.command[0] = 0;

    myCom.year = myEvent.year;
    myCom.md = myEvent.md;

    return myCom;
}


//search through the events until a match
//efficiency can be increased by storing pointers to user locations
vector<struct event>::iterator search_events(struct event myEvent)
{
    vector<struct event>::iterator it = events.begin();

    while(it != events.end() && !event_eq((*it), myEvent))
    {
        ++it;
    }

    return it;
}

//compare the equality of 2 events
bool event_eq(struct event first, struct event second)
{
    bool dates = (first.md == second.md && first.year == second.year);

    if(strcmp(first.user, second.user) == 0 && dates && 
       (second.start[0] == 0 ||(second.start[0] != 0 && strcmp(first.start, second.start) == 0)) &&
       (second.end[0] == 0 ||(second.end[0] != 0 && strcmp(first.end, second.end) == 0)) &&
       (second.name[0] == 0 ||(second.name[0] != 0 && strcmp(first.name, second.name) == 0)))
    {
        return true;
    }
    return false;
}


//send back a vector of items
void reply(vector<com> v)
{
    int fd, ret;

    fd = open(MY_SERVER_OUT_FIFO, O_WRONLY);
    for(vector<com>::iterator it = v.begin(); it != v.end(); it++)
    {
        if(ret = write(fd, (void*)&(*it), sizeof((*it))) < 0)
        {
            fprintf(stderr, "Error: could not write to server: %d\n", ret);
        }
    }
    close(fd);
}
