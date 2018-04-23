#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>



#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 10080 /*port*/

int
main(int argc, char **argv)
{
 int sockfd;
 struct sockaddr_in servaddr;
 char sendline[MAXLINE], recvline[MAXLINE];
 FILE *fp;	
 struct tm  *tminfo;
 time_t rawtime;


 // alarm(300);  // to terminate after 300 seconds

 //basic check of the arguments
 //additional checks can be inserted

//new line added to print the number of arguments passed
 printf("Number of Arguments Passed    : %d\n",argc);

 //Changed from 2 to 3
 if (argc !=3)
 {
  perror("Usage: TCPClient <Server IP> <Server Port>");
  exit(1);
}

//if successful, print the IP Address and Port Number
 printf("Server IP Address  : %s\n", argv[1]);
 printf("Server Port Number : %s\n", argv[2]);

 time(&rawtime);	
 tminfo = localtime(&rawtime);

 /* Create Log file  for the Session */
 fp = fopen("a4p1Client1Log.txt","a+");
 if (fp == NULL)
 { 
	 perror("\nopen error on Client Log File (a4p1Client1Log.txt)\n ");
        exit(1);
 }
 else
 {
       fprintf(fp,"\n Client Log File (a4p1Client1Log.txt) Opened at %s\n",asctime(tminfo));
 }

 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  perror("Problem in creating the socket");
  fprintf(fp,"\n Problem in creating Socket");
  exit(2);
 }

 //Overwrite the SERV_PORT with passed argument port number
 //Creation of the socket
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 //servaddr.sin_addr.s_addr= inet_addr("127.0.0.1");
 servaddr.sin_addr.s_addr= inet_addr(argv[1]);
 servaddr.sin_port =  htons((int) strtol(argv[2], (char **)NULL, 10));
 //servaddr.sin_port =  htons(SERV_PORT); //convert to big-endian order


 //Connection of the client to the socket
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  perror("Problem in connecting to the server");
  perror(strerror(errno));
  fprintf(fp,"\n Problem in Connecting to the Server %s",strerror(errno));
  exit(3);
 }

 fprintf(fp,"\n Client Connected to Server on Port");
 printf("\n Client Connected to Server on Port");
 printf("\n Enter the SQL Command for the Server to Execute :\n");
while (fgets(sendline, MAXLINE, stdin) != NULL) {

  if(send(sockfd, sendline, strlen(sendline), 0) == -1){
  perror("Problem in Send Text to the server");
  perror(strerror(errno));
  fprintf(fp,"\n Problem in Sending Text to the Server %s",strerror(errno));
  exit(3);
 }

  if (recv(sockfd, recvline, MAXLINE,0) == 0){
   //error: server terminated prematurely
   perror("The server terminated prematurely");
   perror(strerror(errno));
   fprintf(fp,"\n The Server Terminated prematurely at %s",asctime(tminfo));
   exit(4);
  }
  printf("%s", "String received from the server:");
  fputs(recvline, stdout);
 }
 fclose(fp);
 exit(0);
}
