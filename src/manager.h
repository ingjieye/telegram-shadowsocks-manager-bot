#include <string>
#include <memory>

class ManagerImpl;

class Manager {
  public:
    Manager(const int& port);
    std::string HandleCmd(const std::string& msg);
  private:
    std::shared_ptr<ManagerImpl> pImpl;
};