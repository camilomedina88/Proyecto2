#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sqlite3.h> 

#define DATABASE_LOCATION "test.db"

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
