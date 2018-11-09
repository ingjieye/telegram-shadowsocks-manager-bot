#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <regex>
#include <algorithm>
#include <exception>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "tgbot/bot.h"

using namespace tgbot;
using namespace tgbot::types;
using namespace methods;
using namespace std;

int verbose = 1;
int64_t admin_id = 0;
int manager_port;

typedef struct {
	int port;
	uint64_t data;
} flow;

string SendToManager(string message)
{
	int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(manager_port);
	inet_aton("127.0.0.1" , &sin.sin_addr);

	socklen_t sin_len = sizeof(sin);
	char buf[500];
	sendto(socket_fd, message.data(), message.length(), 0, (struct sockaddr *) &sin, sin_len);
	recvfrom(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *) &sin, &sin_len);
	return string(buf);
}

void AddPort(string port, string passwd)
{
	string msg = "add: {\"server_port\": " + port + "\"password\":\"" + passwd + "\"}";
	string ret = SendToManager(msg);
	if(ret != "ok") throw runtime_error(ret);
}

void DeletePort(string port)
{
	string msg = "remove: {\"server_port\": " + port + "}";
	string ret = SendToManager(msg);
	if(ret != "ok") throw runtime_error(ret);
}

vector<flow> GetFlowData()
{
	vector<flow> ret;
	// stat: {"50001":33024961447,"50002":6140700730,"50003":2057743160,"50004":87410}
	string raw = SendToManager("ping");
	if(raw.find("stat: {") != 0) throw logic_error("ping response invalid");

	regex e_port("\"\\d+\"");
	regex e_flow(":\\d+");
	cmatch match_port, match_flow;
	regex_match(raw.data(), match_port, e_port);
	regex_match(raw.data(), match_flow, e_flow);
	if(match_port.size() != match_flow.size()) throw logic_error("ping response invalid");
	for(int i=0; i<match_port.size(); i++) {
		flow f;
		f.port = stoi(match_port[i]);
		f.data = stoll(match_flow[i]);
		ret.push_back(f);
	}
	return ret;
}

vector<string> data_unit = {"B", "KB", "MB", "GB", "TB", "PB"};

string GetFlowString()
{
	string ret;
	auto v = GetFlowData();
	for(auto& a: v) {
		ret += to_string(a.port) + ": ";
		int time = 0;
		double flow = a.data;
		while(flow > 1024) {
			flow /= 1024;
			time ++;
		}
		ret += to_string(flow);
		ret += data_unit[time] + "\n";
	}
	return ret;
}

string HandleCmd(const string& msg)
{
	string ret;
	if(msg.find("/flow") == 0) {
		ret = GetFlowString();
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
		if(count(msg.begin(), msg.end(), ' ') != 2) throw invalid_argument("to delete port: /del port password");
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

void EchoBack(const Message message, const Api& api)
{
	try {
		if(admin_id == 0) {
			admin_id = message.from->id;
		} else if (message.from->id != admin_id) {
			api.sendMessage(std::to_string(message.chat.id), "Not admin");
			return;
		}

		string msg = message.text->data();
		if(verbose) {
			cout << "RECEIVED : " << msg << "\n";
		}

		if(msg[0] == '/') {
			api.sendMessage(std::to_string(message.chat.id), HandleCmd(msg));
		}
	} catch(exception& e) {
		api.sendMessage(std::to_string(message.chat.id), string("ERROR: ") + e.what());
	}
}

int main(int argv, char** argc)
{
	if(argv != 3) {
		printf("usage : %s <telegram bot key> <ss-manager port>\n", argc[0]);
		exit(1);
	}

	manager_port = atoi(argc[2]);
	LongPollBot bot(argc[1]);

	bot.callback(EchoBack);

	bot.start();
	
	return 0;
}