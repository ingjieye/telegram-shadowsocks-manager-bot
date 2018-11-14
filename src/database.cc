#include <iostream>
#include <string>
#include <SQLiteCpp/SQLiteCpp.h>
#include <ctime>

using namespace std;

class Database
{
  public:
    Database(string name) : db_(name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) 
    {
        db_.exec("CREATE TABLE traffic (port INTEGER, time INTEGER, traffic INTEGER)");
    }

    void AddTrafficRecord(string port, string traffic)
    {
        time_t result = time(nullptr);
        db_.exec("INSERT INTO traffic VALUES (" + port + "," + to_string(result) + "," + traffic + ")");
    }

  private:
    SQLite::Database db_;
};

int main(int argc, char const *argv[])
{
    Database db("test.sqlite");
    return 0;
}
