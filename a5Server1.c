#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <sqlite3.h> 



/*
    CONCURRENT SERVER: THREAD EXAMPLE
    Must be linked with the "pthread" library also, e.g.:
       cc -o example example.c -lnsl -lsocket -lpthread
    This program creates a connection socket, binds a name to it, then
    listens for connections to the sockect.  When a connection is made,
    it accepts messages from the socket until eof, and then waits for
    another connection...
    This is an example of a CONCURRENT server -- by creating threads several
    clients can be served at the same time...
    This program has to be killed to terminate, or alternately it will abort in
    120 seconds on an alarm...
*/

#define PORTNUMBER 10080
#define MAXSZ 1025

struct serverParm {
           int connectionDesc;
       };

void *serverThread(void *parmPtr) {

#define PARMPTR ((struct serverParm *) parmPtr)
    int recievedMsgLen;
    char messageBuf[1025];

    /* Server thread code to deal with message processing */
    printf("DEBUG: connection made, connectionDesc=%d\n",
            PARMPTR->connectionDesc);
    if (PARMPTR->connectionDesc < 0) {
        printf("Accept failed\n");
        return(0);    /* Exit thread */
    }

    /* Receive messages from sender... */
    while ((recievedMsgLen=
            read(PARMPTR->connectionDesc,messageBuf,sizeof(messageBuf)-1)) > 0)
    {
        recievedMsgLen[messageBuf] = '\0';
        printf("Message: %s\n",messageBuf);
        if (write(PARMPTR->connectionDesc,"GOT IT\0",7) < 0) {
               perror("Server: write error");
               return(0);
           }
    }
    close(PARMPTR->connectionDesc);  /* Avoid descriptor leaks */
    free(PARMPTR);                   /* And memory leaks */
    return(0);                       /* Exit thread */
}

//signature changed main to return integer and include arguments parameter argc and **argv
int main (int argc, char **argv) {
    int listenDesc;
    int clAdrLn;
    struct sockaddr_in myAddr;
    struct sockaddr_in clAddr;
    struct serverParm *parmPtr;
    int connectionDesc;
    int pid;
    int n;
    int recievedMsgLen;
    char msgBuf[MAXSZ];
    FILE *fp;	
    struct tm  *tminfo;
    time_t rawtime;

    //pthread_t threadID;

    //Printing the number of Arguments Passed
    printf("Number of Arguments Passed    : %d\n",argc);

    /* For testing purposes, make sure process will terminate eventually */
   // alarm(120);  /* Terminate in 120 seconds */

    //I added argument validation
    if (argc !=2) {
        perror("Usage: Port # of the Sever to Bind <Server Port>");
        exit(1);
    }

    //Printing the port trying to connect
     printf("Connecting to Port Number :%s\n", argv[1]);

    time(&rawtime);	
    tminfo = localtime(&rawtime);

    /* Create Log file  for the Session */
    fp = fopen("a4p1ServerLog.txt","a+");
    if (fp == NULL)
    { 
	 perror("\nopen error on Log File (a4p1ServerLog.txt)\n ");
        exit(1);
    }
    else
    {
       fprintf(fp,"\n Log File (a4p1ServerLog.txt) Opened at %s\n",asctime(tminfo));
    }

    /* Create socket from which to read */
    if ((listenDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("open error on socket");
	fprintf(fp,"\n Open Socket Error");
	fclose(fp);
        exit(1);
    }
   
     //Overwrite the SERV_PORT with passed argument port number
    /* Create "name" of socket */
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = INADDR_ANY;
    myAddr.sin_port = htons((int) strtol(argv[1], (char **)NULL, 10));
   //myAddr.sin_port = htons(PORTNUMBER);


    if (bind(listenDesc, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0) {
        perror("bind error");
	fprintf(fp,"\n Socket Bind Error");
	fclose(fp);
        exit(1);
    }
    fprintf(fp,"\n Socket Opened Successfully");

    /* Start accepting connections.... */
    /* Up to 5 requests for connections can be queued... */
    if (listen(listenDesc,5) == -1){
        perror("Listing to Socket  error");
	fprintf(fp,"\n Listing to Socket Error%s",strerror(errno));
	fclose(fp);
        exit(1);
	}

    while (1) /* Do forever */ {
        /* Wait for a client connection */
	printf("\nServer Waiting for New Connection\n");
	fprintf(fp,"\nServer Waiting for New Connection\n");
	
        clAdrLn=sizeof(clAddr);

        connectionDesc = accept(listenDesc, (struct sockaddr *) &clAddr, &clAdrLn);
        
        if (connectionDesc == -1){
        perror("Error in Accepting from Connected Socket");
	fprintf(fp,"\n Error in Accepting to Socket Error%s",strerror(errno));
	fclose(fp);
        exit(1);
	}
       
    	
        /* Create a thread to actually handle this client */
	pid = fork();
        printf("Forked :%d\n\n",pid);
        if (pid==0)
	{
		while(1)
		{
		  n= recv(connectionDesc,msgBuf,MAXSZ,0);
		  if (n==0)
		  {
		    close(connectionDesc);
		    break;
		  }
		  msgBuf[n]=0;
                  SQLCall();
		  send(connectionDesc,msgBuf,n,0);
		  printf("\n Received for PID : %d [Msg]: %s\n",pid,msgBuf);
		  fprintf(fp,"\n Received for PID : %d [Msg]: %s\n",pid,msgBuf);
		}
		exit(0);
	}
	else
	{
	close(connectionDesc); //closed by PArent
	}    
    } //main while
    fclose(fp);
    return 0;
}


int SQLCall(void)
{
   sqlite3 *db;
   char *zErrMsg = 0;
   char *Buf;
   int rc,i;
   const char* data = "Callback function called";
   int ctotal, res;

   /* Open database */
   rc = sqlite3_open("/home/010/p/px/pxa160230/mysql/sqlite/test.db", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stderr, "Opened database successfully\n");
   }

   /* Create SQL statement */
   sqlite3_stmt *statement;    

   char *query = "select * from COMPANY;";

    if ( sqlite3_prepare(db, query, -1, &statement, 0 ) == SQLITE_OK ) 
    {
        ctotal = sqlite3_column_count(statement);
        res = 0;

        while ( 1 )         
        {
            res = sqlite3_step(statement);

            if ( res == SQLITE_ROW ) 
            {
                for ( i = 0; i < ctotal; i++ ) 
                {
                     Buf = (char*)sqlite3_column_text(statement, i);
                    // print or format the output as you want 
                    printf("%s ",Buf);
                }
                printf("\n");
            }
            
            if ( res == SQLITE_DONE || res==SQLITE_ERROR)    
            {
                printf("<< done >>\n");
                break;
            }    
        }
    }

   sqlite3_close(db);
   return 0;
}
