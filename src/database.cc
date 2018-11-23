#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include "database.h"
#include "SQLiteCpp/SQLiteCpp.h"

using namespace std;

class Database::DatabaseImpl {
  public:
    DatabaseImpl(const std::string& name);

    void AddTraffic(const std::string& port, const std::string& traffic);

    void GetTraffic(const std::string& port, std::vector<std::string>& time, std::vector<std::string>& traffic);

    void AddPort(const std::string& port, const std::string& password);

    void DeletePort(const std::string& port);

    void GetPort(std::vector<std::string>& port, std::vector<std::string>& password);

  private:
    SQLite::Database db_;
};

Database::Database(const string& name)
    :pImpl(new DatabaseImpl(name))
{
}

void Database::AddTraffic(const string& port, const string& traffic)
{
    pImpl->AddTraffic(port, traffic);
}

void Database::GetTraffic(const string& port, vector<string>& time, vector<string>& traffic)
{
    pImpl->GetTraffic(port, time, traffic);
}

void Database::AddPort(const string& port, const string& password)
{
    pImpl->AddPort(port, password);
}

void Database::DeletePort(const string& port)
{
    pImpl->DeletePort(port);
}

void Database::GetPort(vector<string>& port, vector<string>& password)
{
    pImpl->GetPort(port, password);
}

Database::DatabaseImpl::DatabaseImpl(const string& name) : db_(name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) 
{
    if(!db_.tableExists("traffic")) {
        db_.exec("CREATE TABLE traffic (port INTEGER, time INTEGER, traffic INTEGER)");
    }
    if(!db_.tableExists("server")) {
        db_.exec("CREATE TABLE server (port INTEGER PRIMARY KEY, password TEXT)");
    }
}

void Database::DatabaseImpl::AddTraffic(const string& port, const string& traffic)
{
    time_t result = time(nullptr);
    SQLite::Statement query(db_, "INSERT INTO traffic VALUES (@port, @result, @traffic)");
    query.bind("@port", port);
    query.bind("@result", result);
    query.bind("@traffic", traffic);
    query.exec();
}

void Database::DatabaseImpl::GetTraffic(const string& port, vector<string>& time, vector<string>& traffic)
{
    SQLite::Statement query(db_, "SELECT * FROM traffic WHERE port = @port");
    query.bind("@port", port);
    while(query.executeStep()) {
        time.push_back(query.getColumn(1));
        traffic.push_back(query.getColumn(2));
    }
}

void Database::DatabaseImpl::AddPort(const string& port, const string& password)
{
    SQLite::Statement query(db_, "INSERT INTO server VALUES (@port, @password)");
    query.bind("@port", port);
    query.bind("@password", password);
    query.exec();
}

void Database::DatabaseImpl::DeletePort(const string& port)
{
    db_.exec("Delete from server WHERE port = " + port);
}

void Database::DatabaseImpl::GetPort(vector<string>& port, vector<string>& password)
{
    SQLite::Statement query(db_, "SELECT * FROM server");
    while(query.executeStep()) {
        port.push_back(query.getColumn(0));
        password.push_back(query.getColumn(1));
    }
}