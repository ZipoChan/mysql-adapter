#pragma once
#include <Windows.h>
#include <iostream>
#include <mysql.h>
#include <string>
#include <casbin/persist.h>
#include <casbin/util.h>
#include <casbin/enforcer.h>

using namespace std;

string sql =
    "CREATE TABLE IF NOT EXISTS `policy`  ( \
		`ptype` varchar(100),\
		`v0` varchar(100),\
		`v1` varchar(100),\
		`v2` varchar(100),\
		`v3` varchar(100),\
		`v4` varchar(100),\
		`v5` varchar(100),\
		PRIMARY KEY(`id`)\
		) ";

class MysqlAdapter : public Adapter {
public:
 string user;
 string pswd ;
 string host;
 string database;
 string table;
 unsigned int port;
 MYSQL mySql;
 bool isConnected;

 // Constructor of MysqlAdapter.
 MysqlAdapter(string database = "casbin",string table = "policy",string user = "root",string password = "123456",string host = "localhost",int port =3306);

 // LoadPolicy loads all policy rules from the storage.
 void LoadPolicy(Model* model);

 // LoadPolicyLine loads one policy rule to the model.
 void LoadPolicyLine(string line,Model* model);

 // SavePolicy saves all policy rules to the storage.
 void SavePolicy(Model* model);

 // AddPolicy adds a policy rule to the storage.
 void AddPolicy(string sec, string p_type, vector<string> rule);

 // RemovePolicy removes a policy rule from the storage.
 void RemovePolicy(string sec, string p_type, vector<string> rule);

 // RemoveFilteredPolicy removes policy rules that match the filter from
 // the storage.
 void RemoveFilteredPolicy(string sec, string p_type, int field_index,
                           vector<string> field_values);

 // IsFiltered returns true if the loaded policy has been filtered.
 bool IsFiltered();

private:
 // Create database if it isn't exist.
 void createDatabase();
 // Connect to database.
 void open();
 // Cancel the connection to database.
 void close();
 // Create table if it isn't exist.
 void createTable();
 // Drop the table.
 void dropTable();
 // Excute sql query.
 void excuteQuery(string query);
 // Excute sql queries.
 void excuteQueries(vector<string> queries);

};