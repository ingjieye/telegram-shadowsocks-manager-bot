#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "manager.h"
#include "database.h"

using namespace std;

typedef struct traffic {
    int port;
    uint64_t data;
} traffic;

class Manager::ManagerImpl
{
  public:

    ManagerImpl(const int& port);
    std::string HandleCmd(const std::string& msg);

  private:
  
    int verbose = 1;
    int manager_port;
    Database db_;

    void AddPort(std::string port, std::string password);
    void DeletePort(std::string port);
    std::string GetTrafficString();
    std::string SendToManager(std::string message);
    vector<traffic> GetTrafficData();

};

Manager::Manager(const int& port) : pImpl(new ManagerImpl(port))
{

}

std::string Manager::HandleCmd(const std::string& msg)
{
    return pImpl->HandleCmd(msg);
}

Manager::ManagerImpl::ManagerImpl(const int& port)
    :db_("telegram-shadowsocks-manager-bot.sqlite")
{
    manager_port = port;
}

string Manager::ManagerImpl::SendToManager(string message)
{
	if(verbose) {
		cout << "SendToManager: " << message << endl;
	}
	int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(manager_port);
	inet_aton("127.0.0.1" , &sin.sin_addr);

	socklen_t sin_len = sizeof(sin);
	char buf[500];
	sendto(socket_fd, message.data(), message.length(), 0, (struct sockaddr *) &sin, sin_len);
	int lenth = recvfrom(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *) &sin, &sin_len);
	buf[lenth] = '\0';
	return string(buf);
}

void Manager::ManagerImpl::AddPort(string port, string password)
{
	db_.AddPort(port, password);
	string msg = "add: {\"server_port\": " + port + ", \"password\":\"" + password + "\"}";
	string ret = SendToManager(msg);
	if(ret != "ok") throw runtime_error(ret);
}

void Manager::ManagerImpl::DeletePort(string port)
{
	db_.DeletePort(port);
	string msg = "remove: {\"server_port\": " + port + "}";
	string ret = SendToManager(msg);
	if(ret != "ok") throw runtime_error(ret);
}

vector<traffic> Manager::ManagerImpl::GetTrafficData()
{
	vector<traffic> ret;
	// stat: {"50001":33024961447,"50002":6140700730,"50003":2057743160,"50004":87410}
	string raw = SendToManager("ping");
	if(raw.find("stat: {") != 0) throw logic_error("ping response invalid");

	regex e("(\")(\\d+)(\")(:)(\\d+)");
	smatch match;

	while(regex_search(raw, match, e)) {
		traffic t;
		t.port = stoi(match[2]);
		t.data = stoll(match[5]);
		ret.push_back(t);
		raw = match.suffix();
	}
	return ret;
}

string Manager::ManagerImpl::GetTrafficString()
{
	const vector<string> data_unit = {"B", "KB", "MB", "GB", "TB", "PB"};
	string ret;
	auto v = GetTrafficData();
	char buf[16];
	for(auto& a: v) {
		ret += to_string(a.port) + ": ";
		int time = 0;
		double traffic = a.data;
		while(traffic > 1024) {
			traffic /= 1024;
			time ++;
		}
		snprintf(buf, 16, "%.2lf", traffic);
		ret += buf;
		ret += data_unit[time] + "\n";
	}
	return ret;
}

string Manager::ManagerImpl::HandleCmd(const string& msg)
{
	string ret;
	if(msg.find("/traffic") == 0) {
		ret = GetTrafficString();
	} else if (msg.find("/add") == 0) { // add 50001 eawgfasdf
		if(count(msg.begin(), msg.end(), ' ') != 2) throw invalid_argument("to add port: /add port password");
		string pattern("\\/add\\s(\\d+\\b)\\s(\\S+)");
		regex e(pattern);
		cmatch cm;
		if(regex_match(msg.data(), cm, e) == false || cm.size() != 3) throw invalid_argument("regex match failed (" + pattern + ")");

		string port = cm[1];
		string passwd = cm[2];
		if(atoi(port.data()) > 65535) throw invalid_argument("port exceed maximum number 65535");

		AddPort(port, passwd);
		ret = "add port: " + port + ", password: " + passwd;
	} else if (msg.find("/del") == 0) {
		if(count(msg.begin(), msg.end(), ' ') != 1) throw invalid_argument("to delete port: /del port");
		string pattern("\\/del\\s(\\d+\\b)");
		regex e(pattern);
		cmatch cm;
		if(regex_match(msg.data(), cm, e) == false || cm.size() != 2) throw invalid_argument("regex match failed (" + pattern + ")");

		string port = cm[1];
		if(atoi(port.data()) > 65535) throw invalid_argument("port exceed maximum number 65535");

		DeletePort(port);
		ret = "delete port: " + port;
	} else {
		throw invalid_argument("Unknow command");
	}
	return ret;
}