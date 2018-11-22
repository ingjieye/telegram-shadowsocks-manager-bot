#include <iostream>
#include <vector>
#include "database.h"
using namespace std;


int main(int argc, char const *argv[])
{
    Database db("test.sqlite");
    db.AddTraffic("1234", "1234");
    db.AddPort("1235", "1234");

    vector<string> time;
    vector<string> traffic;

    db.GetTraffic("1234", time, traffic);
    for(auto& a: time) {
        cout << a << endl;
    }

    vector<string> port;
    vector<string> password;
    db.GetPort(port, password);
    for(int i=0; i<port.size(); i++) {
        cout << "port :" << port[i] << ", password: " << password[i] << endl;
    }
    return 0;
}