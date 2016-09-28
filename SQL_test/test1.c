#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sqlite3.h> 
#include <time.h>

#define DATABASE_LOCATION "/www/pages/SE_P2.db"
#define MEAN_SQL_SIZE 150

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
   }
   else
   {
      fprintf(stdout, "Project 2 Embedded Systems table created\n");
   }
   sqlite3_close(db);
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

int main(int argc, char* argv[])
{
   sqlite3 *db; //Definition of the DataBase Object
   char *zErrMsg = 0;
   int rc; //Result of the sqlite functions
   char *sql;
   const char* data = "Callback function called";

   
   if( access(DATABASE_LOCATION, F_OK) != -1)
   {
      puts("Database already exist");
   }
   else
   {
       create_database(db);
      puts("Database created");
   }

   create_dummy_db(db,300,25,532);

   // //Open Database
   // rc = sqlite3_open(DATABASE_LOCATION, &db);  //Location of the Database

   // if( rc ){
   //    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
   //    return(0);
   // }else{
   //    fprintf(stderr, "Opened database successfully\n");
   // }



   // // /* Create SQL statement */
   // // sql = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
   // //       "VALUES (1, 'Paul', 32, 'California', 20000.00 ); " \
   // //       "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
   // //       "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     \
   // //       "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
   // //       "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );" \
   // //       "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
   // //       "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";

   // /* Create SQL statement */
   // sql = "SELECT * from COMPANY";

   // /* Execute SQL statement */
   // rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   // if( rc != SQLITE_OK ){
   // fprintf(stderr, "SQL error: %s\n", zErrMsg);
   //    sqlite3_free(zErrMsg);
   // }else{
   //    fprintf(stdout, "Table created successfully\n");
   // }
   // sqlite3_close(db);
   // return 0;
}
