All=Server Client
all : $(ALL)

Server : server.c
	gcc -g -Wall -o $@ $^

Client : client.c
	gcc -g -Wall -o $@ $^

clean:
	rm -f $(ALL)
