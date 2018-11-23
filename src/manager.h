#include <string>
#include <memory>

class Manager {
  public:
    Manager(const int& port);
    std::string HandleCmd(const std::string& msg);
  private:
    class ManagerImpl;
    std::shared_ptr<ManagerImpl> pImpl;
};