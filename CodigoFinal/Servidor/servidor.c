#include "serial_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sqlite3.h> 
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <malloc.h>
#include <memory.h>

#define DATABASE_LOCATION "/www/pages/SE_P2.db"
#define MEAN_SQL_SIZE 150

struct sockaddr_un; //Socket Structure

int init_socket_server(int *socket_fd,struct sockaddr_in *address_st)
{
   int portno;
	struct sockaddr_in address;
	//*socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
   *socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (*socket_fd < 0){
		puts("socket() failed");
		return -1;
	}
	memset(&address, 0, sizeof(struct sockaddr_in));  //fill address with zeros
   portno = 51717;
   address.sin_addr.s_addr = INADDR_ANY;
   address.sin_port = htons(portno);
   if (bind(*socket_fd, (struct sockaddr *) &address, sizeof(struct sockaddr_in)) != 0) {
      puts("bind() failed");
      return -2;
   }
	if(listen(*socket_fd, 5) != 0) {
		puts("listen() failed");
		return -3;
	}
	*address_st=address;
	return 0;
}

static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int create_database(sqlite3 *db)
{
   char * sql;
   char *zErrMsg = 0;
   int rc; //Result of the sqlite functions

   //Open Database
   rc = sqlite3_open(DATABASE_LOCATION, &db);  

   /* Create SQL statement */
   sql = "CREATE TABLE TEMPERATURE("  \
         /*"ID INT PRIMARY KEY     NOT NULL," \*/
         "VALUE          REAL    NOT NULL," \
         "TIMESTAMP      DATETIME     NOT NULL);" \
         "CREATE TABLE ILUMINATION("  \
         /*"ID INT PRIMARY KEY     NOT NULL," \*/
         "VALUE          REAL    NOT NULL," \
         "TIMESTAMP      DATETIME     NOT NULL);" \
         "CREATE TABLE COMMANDS("  \
         /*"ID INT PRIMARY KEY     NOT NULL," \*/
         "TEMP_F         INT    NOT NULL," \
         "ILUM_F         INT    NOT NULL," \
         "CONSOL_F       INT    NOT NULL," \
         "TIMESTAMP      DATETIME     NOT NULL);";

   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK )
   {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return 0;
   }
   else
   {
      fprintf(stdout, "Project 2 Embedded Systems table created\n");
   }
   sqlite3_close(db);
   return 1;
}

void create_dummy_db(sqlite3 *db, int n_temp, int n_lux, int n_com)
{
   char buffer[80];
   time_t raw_time;
   struct tm * current_time;
   int i;

   char * sql;
   sql = (char*) malloc((MEAN_SQL_SIZE)*(n_temp + n_com + n_lux));
   if(sql == NULL)
   {
      printf("malloc failed\n");
   }
   else
   {
      printf("malloc success\n");
   }
   char *zErrMsg = 0;
   int rc; //Result of the sqlite functions

   raw_time = time(0);
   current_time = localtime(&raw_time);
   strftime(buffer,80,"%F %T",current_time);
   printf("%s",buffer);

   rc = sqlite3_open(DATABASE_LOCATION, &db);  

   for(i=0;i<n_temp;i++)
   {
      /* Create SQL statement */
      sprintf(sql,"%s INSERT INTO TEMPERATURE(VALUE,TIMESTAMP) VALUES(%f,'%s');",sql, 3.1416 + i,buffer);
      printf("%d\n",i);
   }
   for(i=0;i<n_lux;i++)
   {
      /* Create SQL statement */
      sprintf(sql,"%s INSERT INTO ILUMINATION(VALUE,TIMESTAMP) VALUES(%f,'%s');",sql, 1.123 + i,buffer);
      printf("%d\n",i);
   }
   for(i=0;i<n_com;i++)
   {
      /* Create SQL statement */
      sprintf(sql,"%s INSERT INTO COMMANDS(TEMP_F,ILUM_F,CONSOL_F,TIMESTAMP) VALUES(%d,%d,%d,'%s');", sql, i+1, i+2, i+3, buffer);
      printf("%d\n",i);
   }

   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK )
   {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }

   free(sql);
   sqlite3_close(db);
}

void save_db(sqlite3 *db, datosTemp * d_temp, datosLuz * d_luz, comandos * d_comandos, indices index)
{
   char buffer[80];
   struct tm * current_time;
   int i;

   char * sql;
   sql = (char*) malloc(sizeof(char)*(MEAN_SQL_SIZE)*(index.indTemp + index.indLuz + index.indCom));
   if(sql == NULL)
   {
      printf("malloc failed\n");
   }
   else
   {
      printf("malloc success\n");
   }
   char *zErrMsg = 0;
   int rc; //Result of the sqlite functions
   
   //printf("%s",buffer);
   rc = sqlite3_open(DATABASE_LOCATION, &db);
	
   current_time = localtime(&d_temp[0].tiempo);
   strftime(buffer,80,"%F %T",current_time);
   sprintf(sql,"INSERT INTO TEMPERATURE(VALUE,TIMESTAMP) VALUES(%f,'%s');", d_temp[0].datoTemperatura ,buffer);

   for(i=i; i<index.indTemp;i++)
	{
	  /* Create SQL statement */
		current_time = localtime(&d_temp[i].tiempo);
		strftime(buffer,80,"%F %T",current_time);
		sprintf(sql,"%s INSERT INTO TEMPERATURE(VALUE,TIMESTAMP) VALUES(%f,'%s');",sql, d_temp[i].datoTemperatura ,buffer);
	}
	for(i=0;i<index.indLuz;i++)
	{
	  /* Create SQL statement */
		current_time = localtime(&d_luz[i].tiempo);
		strftime(buffer,80,"%F %T",current_time);
		sprintf(sql,"%s INSERT INTO ILUMINATION(VALUE,TIMESTAMP) VALUES(%f,'%s');",sql, d_luz[i].datoLuz ,buffer);
	}
	for(i=0;i<index.indCom;i++)
	{
	  	/* Create SQL statement */
      printf("\n\n\n");
      printf("consol = %d, %d, %d",d_comandos[i].freqTemp, d_comandos[i].freqLuz, d_comandos[i].freqconsol );
      printf("\n\n\n");
		current_time = localtime(&d_comandos[i].tiempo);
		strftime(buffer,80,"%F %T",current_time);
		sprintf(sql,"%s INSERT INTO COMMANDS(TEMP_F,ILUM_F,CONSOL_F,TIMESTAMP) VALUES(%d,%d,%d,'%s');", sql, d_comandos[i].freqTemp, d_comandos[i].freqLuz, d_comandos[i].freqconsol, buffer);
	}
   //printf("%s",sql);
   
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK )
   {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }

   free(sql);
   sqlite3_close(db);
}


int main(int argc, char* argv[])
{
   sqlite3 *db; //Definition of the DataBase Object
   char *zErrMsg = 0;
   int rc; //Result of the sqlite functions
   char *sql;
   const char* data = "Callback function called";
   int   result;  
   struct sockaddr_in address;
   int connection_fd;
   int socket_fd;
   char buffer[60000];
   datosTemp d_temp[600];
   datosLuz d_luz[600];
   comandos d_comandos[600];
   indices index;
   socklen_t address_length;
   address_length = sizeof(address);

   if( access(DATABASE_LOCATION, F_OK) != -1)
   {
      puts("Database already exist");
   }
   else
   {
		create_database(db);
		puts("Database created");
   }

   puts("Init socket server");
   result=init_socket_server(&socket_fd, &address);
   
   while((connection_fd = accept(socket_fd,(struct sockaddr *)&address,&address_length)) < 0);
   
   for(;;)
   {

      if (read(connection_fd, buffer, 60000) < 0)
         puts("ERROR reading from socket");
      else
      {
         printf("Here is the message: %s\n", buffer);
         int result = deserialize(buffer, d_temp, d_luz, d_comandos, &index);
         if(result != 1)
         {
            puts("Error while deserializing");
         }
         else
         {
            save_db(db, d_temp, d_luz, d_comandos, index);
         }
      }
   }
   

}
