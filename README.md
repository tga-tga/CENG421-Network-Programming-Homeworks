# CENG421-Network-Programming-Homeworks
Network Programming course assignments
---------------------------------------------------------------------
Coursebook is 'The Definitive Guide to Linux Network Programming' by Nathan Yocom, John Turner, and Keir Davis

Homeworks are focused on the book and codes are highly correlated with the coursebook.

Assignment 1
---------------------------------------------------------------------

Create a simple server (single client) that can return the time and date whenever a client connects to it.
That is, the server responds with date and time, then disconnects.

Sample ouput:

*connected...*

*Thu Apr 22 20:10:26 2021*

*..disconnected..*

Assignment 2
---------------------------------------------------------------------

Take multiprocess and multithreaded servers, and add 

*daemonizing,*

*dropping priviledges,*

*chroot jailing,* 

*and logging* to those servers. So, that they log their activities to syslog, and run as a regular user, and run chrooted to some directory.
