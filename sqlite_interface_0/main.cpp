#include <iostream>
#include <conio.h>
#include <list>
#include <string>
#include <fstream>
#include "sqlite3.h"
#include "FileExist.h"
#include "LoginStructure.h"

char db_name[128];
int key = 0;
void CreateDB(const char* name);
void CreateLoginTable();
void AddNewUser(std::string login, std::string pasword);
char* GetLogin(std::string try_login);
void DeleteUser(const std::string del_login);
void ShowInfo();
int GetRowsNumber();
void DBMenu();

void main()
{
	while (true)
	{
		printf("1.  Open db\n2.  Create db\n3.  Exit\nKey: ");
		std::cin >> key;
		switch (key)
		{
		case 1:
		{
			printf("enter db name: ");
			std::cin >> db_name;
			if (IsFileExist(db_name))
			{
				printf("db exist\n");
				DBMenu();
			}
			else
			{
				printf("no such file\n");
			}
			break;
		}
		case 2:
		{
			printf("enter new name of db: ");
			std::cin >> db_name;
			if (IsFileExist(db_name))
			{
				printf("file alredy exist\n");
			}
			else
			{
				CreateDB(db_name);
				CreateLoginTable();
			}
			break;
		}
		case 3:
		{
			exit(0);
		}
		default:
		{
			printf("wrong key\n");
			break;
		}
		}
	}
	_getch();
}

void CreateDB(const char* name)
{
	sqlite3* connection;
	int rc;
	rc = sqlite3_open(name, &connection);
	printf("db created with code: %i \n", rc);
	sqlite3_close(connection);
}

void CreateLoginTable()
{
	sqlite3* connection;
	int rc;
	rc = sqlite3_open(db_name, &connection);
	printf("db opened with code %i at <CreateLoginTable>\n", rc);
	sqlite3_stmt* stmt;
	rc = sqlite3_prepare(connection, "CREATE TABLE IF NOT EXISTS USERS(ID INTEGER NOT NULL, LOGIN varchar(255), PASSWORD varchar(255), PRIMARY KEY(ID));", -1, &stmt, 0);
	printf("table created with code %i at <CreateLoginTable>\n", rc);
	sqlite3_step(stmt);
	sqlite3_reset(stmt);
	sqlite3_finalize(stmt);
	rc = sqlite3_close(connection);
	printf("db closed with code %i at <CreateLoginTable>\n", rc);
}

void AddNewUser(std::string login, std::string password)
{
	sqlite3* connection;
	int rc;
	rc = sqlite3_open(db_name, &connection);
	printf("db opened with code %i at <AddNewUser>\n", rc);
	std::string raw_query = "INSERT INTO USERS (LOGIN, PASSWORD) VALUES ('" + login + "', '" + password + "');";
	char query[256];
	std::strcpy(query, raw_query.c_str());
	sqlite3_stmt* stmt;
	rc = sqlite3_prepare(connection, query, -1, &stmt, 0);
	std::cout << "char   > " << query << std::endl;
	std::cout << "string > " << raw_query << std::endl;
	printf("user created with code %i at <AddNewUser>\n", rc);
	sqlite3_step(stmt);
	sqlite3_reset(stmt);
	sqlite3_finalize(stmt);
	rc = sqlite3_close(connection);
	printf("db closed with code %i at <AddNewUser>\n", rc);
}


char* GetLogin(std::string try_login)
{
	sqlite3* connection;
	int rc;
	rc = sqlite3_open(db_name, &connection);
	printf("db opened with code %i at <GetLogin>\n", rc);
	std::string raw_query = "SELECT LOGIN FROM 'USERS' WHERE LOGIN = '" + try_login + "';";
	char query[128];
	std::strcpy(query, raw_query.c_str());
	std::cout << "char   > " << query << std::endl;
	std::cout << "string > " << raw_query << std::endl;
	sqlite3_stmt* stmt;
	rc = sqlite3_prepare(connection, query, -1, &stmt, 0);
	printf("query done with code %i at <GetLogin>\n");
	rc = sqlite3_step(stmt);
	printf("step done with code %i at <GetLogin>\n");
	if (rc != 100)
	{
		return (char*)"no such login";
	}
	char* result = new char;
	std::strcpy(result, (char*)sqlite3_column_text(stmt, 0));
	sqlite3_finalize(stmt);
	rc = sqlite3_close(connection);
	printf("db closed with code %i at <GetLogin>\n", rc);
	return result;
}

void DeleteUser(std::string del_login)
{
	sqlite3* connection;
	int rc;
	rc = sqlite3_open(db_name, &connection);
	printf("db opened with code %i at <DeleteUser>\n", rc);
	std::string raw_query = "DELETE FROM USERS WHERE LOGIN = '" + del_login + "';";
	char query[128];
	std::strcpy(query, raw_query.c_str());
	sqlite3_stmt* stmt;
	rc = sqlite3_prepare(connection, query, -1, &stmt, 0);
	std::cout << "char   > " << query << std::endl;
	std::cout << "string > " << raw_query << std::endl;
	printf("user deleted with code %i at <DeleteUser>\n", rc);
	rc = sqlite3_step(stmt);
	printf("steped with code %i at <DeleteUser>\n", rc);
	sqlite3_finalize(stmt);
	rc = sqlite3_close(connection);
	printf("db closed with code %i at <DeleteUser>\n", rc);
}

int GetRowsNumber()
{
	int result;
	sqlite3* connection;
	int rc;
	rc = sqlite3_open(db_name, &connection);
	printf("db opened with code %i at <GetRowsNumber>\n", rc);
	sqlite3_stmt* stmt;
	rc = sqlite3_prepare(connection, "SELECT COUNT(*) FROM USERS;", -1, &stmt, 0);
	sqlite3_step(stmt);
	printf("COUNT(*) code %i\n", rc);
	result = sqlite3_column_int(stmt, 0);
	printf("rows: %i\n", result);
	sqlite3_finalize(stmt);
	sqlite3_close(connection);
	return result;
}

void ShowInfo()
{
	sqlite3* connection;
	int rc;
	rc = sqlite3_open(db_name, &connection); 
	printf("db opened with code %i at <ShowInfo>\n", rc);
	sqlite3_stmt* stmt;
	rc = sqlite3_prepare(connection, "SELECT * FROM USERS;", -1, &stmt, 0);
	printf("SELECT ID, LOGIN, PASSWORD FROM USERS; done with code %i at <ShowInfo>\n", rc);
	LoginStructure result_struct;
	char* raw_result_login = new char;
	char* raw_result_password = new char;
	int counter = 0;
	while (sqlite3_step(stmt) != SQLITE_DONE)
	{
		result_struct.id = sqlite3_column_int(stmt, 0);
		std::strcpy(raw_result_login, (char*)sqlite3_column_text(stmt, 1));
		result_struct.username = raw_result_login;
		std::strcpy(raw_result_password, (char*)sqlite3_column_text(stmt, 2));
		result_struct.password = raw_result_password;
		printf("row(%i): id[%i] username[%s] password[%s]\n", counter, result_struct.id, result_struct.username, result_struct.password);
		counter++;
	}
	printf("loop end\n");
	rc = sqlite3_finalize(stmt);
	printf("finalized %i\n", rc);
	rc = sqlite3_close(connection);
	printf("db closed with code %i at <ShowInfo>\n", rc);
}

void DBMenu()
{
	while (true)
	{
		printf("selected db: %s\n1. Add new user\n2. Delete user\n3. Show info\n4. Exit\nKey: ", db_name);
		std::cin >> key;
		switch (key)
		{
		case 1:
		{
			char new_password[64];
			char new_login[20];
			printf("enter new login(20 symbols): ");
			std::cin >> new_login;
			if (std::strcmp(new_login, GetLogin(new_login)) == 0)
			{
				printf("login has beed taken");
				break;
			}
			else
			{
				printf("login is free\nenter password: ");
				std::cin >> new_password;
				AddNewUser(new_login, new_password);
				break;
			}
		}
		case 2:
		{
			char delete_login[20];
			printf("enter login: ");
			std::cin >> delete_login;
			DeleteUser(delete_login);
			break;
		}
		case 3:
		{
			ShowInfo();
			break;
		}
		case 4:
		{
			exit(0);
		}
		default:
		{
			printf("wrong key\n");
			break;
		}
		}
	}
}