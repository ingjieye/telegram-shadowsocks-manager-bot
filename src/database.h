#include <string>
#include <vector>
#include <memory>

class Database
{
  public:
    Database(const std::string& name);

    void AddTraffic(const std::string& port, const std::string& traffic);

    void GetTraffic(const std::string& port, std::vector<std::string>& time, std::vector<std::string>& traffic);

    void AddPort(const std::string& port, const std::string& password);

    void DeletePort(const std::string& port);

    void GetPort(std::vector<std::string>& port, std::vector<std::string>& password);

  private:
    class DatabaseImpl;
    std::shared_ptr<DatabaseImpl> pImpl;
};