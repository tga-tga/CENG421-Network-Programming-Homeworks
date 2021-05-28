#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include <syslog.h>

// some dependencies were missing
// https://github.com/Apress/def-guide-to-linux-network-programming/pull/1/files

int daemonize()
{
  pid_t pid;
  long n_desc;
  int i;

  if ((pid = fork()) != 0) {
    exit(0);
  }

  setsid();

  if ((pid = fork()) != 0) {
    exit(0);
  }

  chdir("/");
  umask(0);

  n_desc = sysconf(_SC_OPEN_MAX);
  for (i = 0; i < n_desc; i++) {
    close(i);
  }

  return 1;
}


void sigchld_handler(int signo)
{
  while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{

    struct passwd *pws;
    const char *user = "nopriv";

    // attemt to retrieve the user ID that with our nonprivileged user
    pws = getpwnam(user);

    if (pws == NULL) {
        printf("Unknown user: %s\n", user);
        return 0;
    }

    // chroot jailing
    int res1 = chroot("/");
    int res2 = chdir("/");
    
    // daemonizing
    daemonize();   
 
    // dropping privileges
    setuid(pws->pw_uid);    


    struct sockaddr_in sAddr;
    int listensock;
    int newsock;
    char buffer[25];
    int result;
    int nread;
    int pid;
    int val;
    
    listensock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    val = 1;
    result = setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (result < 0) {
        perror("server2");
        return 0;
    }

    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(1997);
    sAddr.sin_addr.s_addr = INADDR_ANY;

    result = bind(listensock, (struct sockaddr *) &sAddr, sizeof(sAddr));
    if (result < 0) {
        perror("exserver2");
        return 0;
    }

    result = listen(listensock, 5);
    if (result < 0) {
        perror("exserver2");
        return 0;
    }

    signal(SIGCHLD, sigchld_handler);

    while (1) {
        newsock = accept(listensock, NULL ,NULL);
        if ((pid = fork()) == 0) {

			// Child process created message for syslog 
			char message [50] = "child process ";
            char pid[50] = "";
			sprintf(pid,"%i",getpid());
            strcat(message,pid);
            strcat(message," created.\n");

            // Logging 
            openlog("MPServerLogger", LOG_PID, LOG_USER);
            syslog(LOG_INFO, "%s", message);
            closelog();

            printf("child process %i created.\n", getpid());
            close(listensock);
            nread = recv(newsock, buffer, 25, 0);
            buffer[nread] = '\0';
            printf("%s\n", buffer);
            send(newsock, buffer, nread, 0);
            close(newsock);

			// Child process finished message for syslog 
			strcpy(message,"child process ");
            strcpy(pid,"");
			sprintf(pid,"%i",getpid());
            strcat(message,pid);
            strcat(message," finished.\n");

            // Logging
            openlog("MPServerLogger ", LOG_PID, LOG_USER);
            syslog(LOG_INFO, "%s", message);
            closelog();

            printf("child process %i finished.\n", getpid());
            exit(0);
        }
        close(newsock);
    }
}

