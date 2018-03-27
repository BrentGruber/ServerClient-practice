LIBS = 
HEADERS = 
OBJECTS1 = server.o 
OBJECTS2 = client.o
FLAGS =  -O3 -g 

default: all

%.o: %.c $(HEADERS)
	gcc $(FLAGS) -c $< -o $@ 

all: server client
    
server: $(OBJECTS1)
	g++ $(FLAGS) $(OBJECTS1) $(LIBS) -o $@

client: $(OBJECTS2)
	gcc $(FLAGS) $(OBJECTS2) $(LIBS) -o $@

clean:
	 -rm -f $(OBJECTS)
	 -rm -f server client *~
