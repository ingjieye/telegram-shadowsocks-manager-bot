#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include "SQLiteCpp/SQLiteCpp.h"
#include "database.h"

using namespace std;

Database::Database(const string& name) : db_(name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) 
{
    if(!db_.tableExists("traffic")) {
        db_.exec("CREATE TABLE traffic (port INTEGER, time INTEGER, traffic INTEGER)");
    }
    if(!db_.tableExists("server")) {
        db_.exec("CREATE TABLE server (port INTEGER PRIMARY KEY, password TEXT)");
    }
}

void Database::AddTraffic(const string& port, const string& traffic)
{
    time_t result = time(nullptr);
    SQLite::Statement query(db_, "INSERT INTO traffic VALUES (@port, @result, @traffic)");
    query.bind("@port", port);
    query.bind("@result", result);
    query.bind("@traffic", traffic);
    query.exec();
}

void Database::GetTraffic(const string& port, vector<string>& time, vector<string>& traffic)
{
    SQLite::Statement query(db_, "SELECT * FROM traffic WHERE port = @port");
    query.bind("@port", port);
    while(query.executeStep()) {
        time.push_back(query.getColumn(1));
        traffic.push_back(query.getColumn(2));
    }
}

void Database::AddPort(const string& port, const string& password)
{
    SQLite::Statement query(db_, "INSERT INTO server VALUES (@port, @password)");
    query.bind("@port", port);
    query.bind("@password", password);
    query.exec();
}

void Database::DeletePort(const string& port)
{
    db_.exec("Delete from server WHERE port = " + port);
}

void Database::GetPort(vector<string>& port, vector<string>& password)
{
    SQLite::Statement query(db_, "SELECT * FROM server");
    while(query.executeStep()) {
        port.push_back(query.getColumn(0));
        password.push_back(query.getColumn(1));
    }
}
