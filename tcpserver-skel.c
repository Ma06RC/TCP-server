#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h> 
#include <string.h> 
#include <sys/wait.h> 
  
#define bufsize 1024

void eatZombies(int n){
  // This function removes the zombie process state left
  // after the child exits. Learn about this in NWEN 301!

  wait3(NULL,WNOHANG,NULL); // Nom Nom 
}

// Error function for printing out the error
void error(char *msg){
  perror(msg);
  exit(0);
}

// This process/modify the text that has been sent from the client and server process the text
void modifierText(char *txt){
  char *p = txt; 
  while(*p != '\0'){
    *p = toupper(*p);
    p++;
  }
}

int main(int argc, char *argv[]){
  int sock, length, msgsock, status;
  struct sockaddr_in server;
  char buf[bufsize];

  // for forking, and cleaning up zombie child state afterwards
  // You must not change this code. 

  pid_t id;       
  signal(SIGCHLD, &eatZombies);
 
  // OK, NWEN 243 code starts here. 

  //Create server socket() 
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0){   //Error checking if the socket has been succesfully created
    error("Error creating a server socket");
  }

  // Next, create the socket addressing structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(atoi(argv[1])); // this time 1st arg is port#

  // Next you need to BIND your socket.  
  // YOUR CODE HERE   
  socklen_t addrlen = sizeof(server);   //length of structure
  int bindServer = bind(sock, (struct sockaddr_in *) &server, addrlen);
  if(bindServer < 0){
    error("Address already in use");
  }

  // Right, now we are in the main server loop.
  // YOUR CODE HERE   
  // Start by listening, 5 is the maximum length to be queued
  int listenServer = listen(sock, 5); 
  if(listenServer < 0){
    error("Error on listening");
  }

  while(1){
    
    // you need to accept the connection request
    // child
    int newsock = accept(sock, (struct sockaddr *) 0, 0);
    if(newsock < 0){
      error("Cannot accept the connection request");
    }

    // the next call makes a new child process that will actually handle the client.
    id = fork();
    
    // when id == 0, this is the child and needs to do the work for the server. 
    // when if > 0, this is the parent, and it should just loop around,
    // when id < 0, we had an error.

    //Child socket
    if(id == 0){  
      int readServer = read(newsock, buf, bufsize);
      if(readServer < 0){
        error("Error cannot read into the buffer\n");
      }

      modifierText(buf);

      int writeServer = write(newsock, buf, bufsize);
      if(readServer < 0){
        error("Error cannot write from the buffer\n");
      }
      //  print result
      fprintf(stdout,"%s\n", buf);
      close(newsock);
      exit(0);
    }
    if(id < 0){
      error("Error for connecting a client");
    }
    // Your code here for the child process, that will read from the connection socket, process the data 
    // write back to the socket, and then close and finally call exit(0) when done.

    // Note:  make sure the parent process (id > 0) does not execute this code, but immediately loops back
    // around (via the while) to get another connection request.

  }
}
