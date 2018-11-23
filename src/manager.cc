#include "manager.h"
#include "manager_impl.h"
using namespace std;

Manager::Manager(const int& port) : pImpl(new ManagerImpl(port))
{

}

std::string Manager::HandleCmd(const std::string& msg)
{
    pImpl->HandleCmd(msg);
}