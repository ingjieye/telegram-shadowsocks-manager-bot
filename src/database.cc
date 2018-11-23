#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include "database_impl.h"
#include "database.h"

using namespace std;

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
