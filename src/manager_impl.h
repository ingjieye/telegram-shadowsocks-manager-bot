#include <string>
#include "database.h"

class ManagerImpl
{
  public:

    ManagerImpl();
    std::string HandleCmd(const std::string& msg);

  private:
  
    int verbose = 1;
    int manager_port;
    Database db_("telegram-shadowsocks-manager-bot.sqlite");

    typedef struct traffic {
        int port;
        uint64_t data;
    } traffic;

    void AddPort(std::string port, std::string password);
    void DeletePort(std::string port);
    std::string GetTrafficString();
    std::string SendToManager(std::string message);
    vector<traffic> GetTrafficData();

};
