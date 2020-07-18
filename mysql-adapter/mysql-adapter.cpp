#include "mysql-adapter.h"

using namespace std;

 // Constructor of MysqlAdapter.
MysqlAdapter::MysqlAdapter(string database, string table, string user, string password, string host, int port) {
  this->user = user;
  this->pswd = password;
  this->host = host;
  this->database = database;
  this->table = table;
  this->port = port;

  file_path = "defualt";
  filtered = false;

  mysql_init(&mySql);
  mysql_query(&mySql, "SET NAMES GBK");
  createDatabase();
  if (!mysql_real_connect(&mySql, host.c_str(), user.c_str(), pswd.c_str(),
                          database.c_str(), port, NULL, 0))
    throw exception("can't connect the database.");
  createTable();
  close();
}

// Create database if it isn't exist.
void MysqlAdapter::createDatabase() {
  string queryStr = "create database if not exists ";
  queryStr += database;
  if (0 == mysql_query(&mySql, queryStr.c_str())) {
    queryStr = "use ";
    queryStr += database;
    if (0 == mysql_query(&mySql, queryStr.c_str())) {
      return;
    }
  }
}

// Connect to database.
void MysqlAdapter::open() {
    mysql_init(&mySql);
    mysql_query(&mySql, "SET NAMES GBK");
    if (!mysql_real_connect(&mySql, host.c_str(), user.c_str(), pswd.c_str(), database.c_str(), port, NULL, 0))
      throw exception("can't connect the database.");
}

 // Cancel the connection to database.
void MysqlAdapter::close() {
    mysql_close(&mySql);
}

 // Create table if it isn't exist.
void MysqlAdapter::createTable() {
  string query = "CREATE TABLE IF NOT EXISTS `" + table + "` ( ";
  query += 
	    "`id` int AUTO_INCREMENT,\
        `ptype` varchar(100),\
		`v0` varchar(100),\
		`v1` varchar(100),\
		`v2` varchar(100),\
		`v3` varchar(100),\
        `v4` varchar(100),\
        `v5` varchar(100),\
        PRIMARY KEY(`id`)\
		) ";
  mysql_query(&mySql, query.c_str());
}

 // Drop the table.
void MysqlAdapter::dropTable() {

}

// LoadPolicy loads all policy rules from the storage.
void MysqlAdapter::LoadPolicy(Model* model) { 
    open();
    int res;
    MYSQL_RES* result = NULL;
    MYSQL_ROW sql_row;
    string query = "select * from " + table;

    res = mysql_query(&mySql, query.c_str());

    if (!res) {
        result = mysql_store_result(&mySql);
        if (result) {
          while (sql_row = mysql_fetch_row(result))
          {
            string line = "";

            for (int i = 7; i >= 2; i--) {
              if (string(sql_row[i]) != "") {
                line = ", " + string(sql_row[i]) + line;
              }
            }

            line = "p" + line;
            line = Trim(line);

            cout << "line:" << line << endl;
            LoadPolicyLine(line, model);
          }
        }
      }
    else {
        cout << "query sql failed!" << endl;
    }

    if (result != NULL)
        mysql_free_result(result);
    close();
}

// SavePolicy saves all policy rules to the storage.
void MysqlAdapter::SavePolicy(Model* model) {

}

// AddPolicy adds a policy rule to the storage.
void MysqlAdapter::AddPolicy(string sec, string p_type, vector<string> rule) {
  string query = "insert into " + table + " values(0,'" + p_type + "'";
  int len = rule.size();
  int i;

  for (i = 0; i < len; i++) {
    query = query + ",'" + rule[i] + "'";
  }
  for (; i < 6; i++) {
    query = query + ",''";
  }
  query += ")";

  excuteQuery(query);
}

// RemovePolicy removes a policy rule from the storage.
void MysqlAdapter::RemovePolicy(string sec, string p_type, vector<string> rule) {
  string query = "delete from `" + table + "` where ptype = '"+p_type+"'";
  int i = 0;
  int len = rule.size();

  for (i = 0; i < len; i++) {
    query = query + " and v" + to_string(i) + " = '" + rule[i] + "'";
  }
  for (; i < 6; i++) {
    query = query + " and v" + to_string(i) + " = ''";
  }

  excuteQuery(query);
 }

// RemoveFilteredPolicy removes policy rules that match the filter from
// the storage.
void MysqlAdapter::RemoveFilteredPolicy(string sec, string p_type,
    int field_index,
    vector<string> field_values) {

}

// IsFiltered returns true if the loaded policy has been filtered.
bool MysqlAdapter::IsFiltered() {
  return false;
}

 // LoadPolicyLine loads one policy rule to the model.
void MysqlAdapter::LoadPolicyLine(string line, Model* model) {
  if (line == "" || line.find("#") == 0) return;

  vector<string> tokens = Split(line, ",", -1);
  for (int i = 0; i < tokens.size(); i++) tokens[i] = Trim(tokens[i]);
  
  string key = tokens[0];
  string sec = key.substr(0, 1);
  
  vector<string> new_tokens(tokens.begin() + 1, tokens.end());
  
  if (model->m.find(sec) == model->m.end()) model->m[sec] = AssertionMap();

  (model->m[sec].assertion_map[key]->policy).push_back(new_tokens);
}

 // Excute sql query.
void MysqlAdapter::excuteQuery(string query) {
  open();
  int res = mysql_query(&mySql, query.c_str()); 
  close();
 }

// Excute sql queries.
 void MysqlAdapter::excuteQueries(vector<string> queries) {
  open();
  for (int i = 0; i < queries.size(); i++) {
    int res = mysql_query(&mySql, queries[i].c_str());
  }
  close();
 }

