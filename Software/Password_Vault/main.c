// Password Vault
// Software to store encrypted data
// Distributed under the MIT License
// © Copyright Maxim Bortnikov 2021
// For more information please visit
// https://github.com/Northstrix/Password_Vault

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sqlite3.h" 

static int csv(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   FILE *fp;
   fp = fopen("passwords.csv", "a+");
   for(i = 0; i<argc; i++) {
      fprintf(fp, "%s,%s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   fclose(fp);
   return 0;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s\n", argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

void create_table(){
	   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;

   /* Open database */
   rc = sqlite3_open("passwords.db", &db);
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
   }

   /* Create SQL statement */
   sql = "CREATE TABLE DATA("  \
      "WEBSITE        TEXT    NOT NULL," \
      "LOGIN          TEXT    NOT NULL," \
      "PASSWORD       TEXT    NOT NULL);" ;

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Table created successfully\n");
   }
   sqlite3_close(db);
	return;
}

void insert(){
	   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;

   /* Open database */
   rc = sqlite3_open("passwords.db", &db);
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
   }
	char website[1000], login[1000], password[1000];
	fprintf(stdout, "Enter the website: ");
    scanf("%s", website);
	fprintf(stdout, "Enter the login: ");
    scanf("%s", login);
   	fprintf(stdout, "Enter the password: ");
    scanf("%s", password);
    char sql[1500] = "INSERT INTO DATA (WEBSITE,LOGIN,PASSWORD)VALUES ('";
	char sql2[] = "', '";
	char sql3[] = "', '";
	char sql4[] = "');";
	strcat(sql, website);
	strcat(sql, sql2);
	strcat(sql, login);
	strcat(sql, sql3);
	strcat(sql, password);
	strcat(sql, sql4);

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Records created successfully\n");
   }
   sqlite3_close(db);
   return;
}

void disp(){
	sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   const char* data = "Callback function called";

   /* Open database */
   rc = sqlite3_open("passwords.db", &db);
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
   } else {
      fprintf(stderr, "Passwords:\n\n");
   }

   /* Create SQL statement */
   sql = "SELECT * from DATA";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   
   if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "///The end///\n");
   }
   sqlite3_close(db);
   return;
}

void write_csv(){
	sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   const char* data = "Callback function called";

   /* Open database */
   rc = sqlite3_open("passwords.db", &db);
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
   }
   /* Create SQL statement */
   sql = "SELECT * from DATA";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, csv, (void*)data, &zErrMsg);
   
   if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {

   }
   sqlite3_close(db);
   return;
}

void remove_record(){
	sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   const char* data = "Callback function called";

   /* Open database */
   rc = sqlite3_open("passwords.db", &db);
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
   }
	char rm[100];
	fprintf(stdout, "Enter the WEBSITE: ");
    scanf("%s", rm);
   /* Create merged SQL statement */
   char sql[100] = "DELETE from DATA where WEBSITE='";
   char sql1[] = "'; ";
   	strcat(sql, rm);
	strcat(sql, sql1);
   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   
   if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
   sqlite3_close(db);
   return;
}

void update_content(){
	sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   const char* data = "Callback function called";

   /* Open database */
   rc = sqlite3_open("passwords.db", &db);
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

   } 
   /* Create merged SQL statement */
   char sql[150] = "UPDATE DATA set PASSWORD= '";
   char sql1[] = "' where WEBSITE='";
   char sql2[] = "'; ";
   	char nm[500], am[10];
	fprintf(stdout, "Enter the website (in encrypted form): ");
    scanf("%s", nm);
	fprintf(stdout, "Enter the new password: ");
    scanf("%s", am);
   	strcat(sql, am);
	strcat(sql, sql1);
	strcat(sql, nm);
	strcat(sql, sql2);

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   
   if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
   sqlite3_close(db);
}

int main(int argc, char* argv[]) {
	   int x = 0;
	   while (x !=7) {
		   printf("\nWhat do you want to do?\n1.Create the table\n2.Add record\n3.Remove record\n4.Display all records\n5.Export all records to passwords.csv file\n6.Change the password\n7.Quit\n");
		   scanf("%d", &x);
	   
	   		   if (x == 1) {
			   create_table();
		   }
		   
		   if (x == 2) {
			   insert();
		   }
		   
		   if (x == 3) {
			   remove_record();
		   }
		   
		   if (x == 4) {
			   disp();
		   }
		   
		   if (x == 5) {
			   write_csv();
		   }
		   
		   if (x == 6){
		   	update_content();
		   }

	   
	}
}