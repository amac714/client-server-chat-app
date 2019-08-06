# CS372 Programming assignment 1

# Instructions:

1) compile chatclient.c

		gcc chatclient.c -o chatclient

2) Make sure chatserve has permission to execute

		chmod +x chatserve

3) Run `./chatserve x` with x as the port number

4) Run chatclient

		./chatclient hostname port#

		Hostname is hostname where chatserve is running, for example flip1.

		port# is the port chatserve is on.


5) Enter handle name for both server and client

6) chatclient must send message first. Client and server takes turns sending messages. 

7) To end connection, type `\quit` in either client or server. 

8) To exit the client or server program, press `Ctrl+C`