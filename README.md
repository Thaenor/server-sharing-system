Server-Sharing-System
==========================

This project was developed for the Network classes.
It's done in "C" language using the Berkley sockets API.

There are 3 different components to this project:

.One server thread continuously broadcasts it's list of files
(currently it's a string of text, but could easily been changed
to the resutl of a command like "ls")

.A client thread of the program listens for broadcasts and updates
it's list of files.

.A 3rd thread recreates the HTTP protocol and listens for outside
clients, showing an html page to each visitor listing the files
currently existing in the server.

How to test this:
.Compile and execute the program 2 or more times.
.A thread should eventually start printing the names of files
on console
.Access the cgi component through the web browser on port 30473
(e.g if you are running localy http://127.0.0.1:30473 should work)

the http component is based on a Sanos webpage (www.jbox.dk/sanos/webserver.htm)
