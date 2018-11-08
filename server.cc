#include <string>
#include <iostream>
#include <thread>
#include <vector>

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
int BUFSIZE = 4096;
int MAXARG = 256;
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

int AddPort(int port, string passwd)
{
	int ret = -1;
	string msg = "add: {\"server_port\": " + to_string(port) + "\"password\":\"" + passwd + "\"}";
	if(SendToManager(msg) == "ok") ret = 0;
	return ret;
}

int RemovePort(int port)
{
	int ret = -1;
	string msg = "remove: {\"server_port\": " + to_string(port) + "}";
	if(SendToManager(msg) == "ok") ret = 0;
	return ret;
}

vector<flow> GetFlowData()
{
	vector<flow> ret;
	// stat: {"50001":33024961447,"50002":6140700730,"50003":2057743160,"50004":87410}
	string raw = SendToManager("ping");
	if(raw.find("stat: {") != 0) return ret;

	auto left = raw.find("{");
	auto right = raw.find("}");
	raw = raw.substr(left + 1, right - left - 1);
	//"50001":33024961447,"50002":6140700730,"50003":2057743160,"50004":87410

	size_t pos;
	while( !raw.empty() ) {
		pos = raw.find(',');
		if(pos == string::npos) pos = raw.length();
		string sub = raw.substr(0, pos);
		// "50001":33024961447
		auto sub_pos = sub.find(':');
		flow f;
		f.port = stoi(sub.substr(1, sub_pos - 2));
		f.data = stoll(sub.substr(sub_pos + 1));
		ret.push_back(f);
		if(pos == raw.length()) break;
		raw = raw.substr(pos + 1);
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

void EchoBack(const Message message, const Api& api)
{
	if(admin_id == 0) {
		admin_id = message.from->id;
	} else if (message.from->id != admin_id) {
		api.sendMessage(std::to_string(message.chat.id), "Not admin");
		return;
	}

	const char* msg = message.text->data();
	if(verbose) {
		cout << "RECEIVED : " << msg << "\n";
	}
	if(msg[0] == '/') {
		if(strcmp(msg, "/flow") == 0) {
			string ret = GetFlowString();
			cout << "GetFlowString() = " << ret;
			api.sendMessage(std::to_string(message.chat.id), ret);
		} else if (strcmp(msg, "ss") == 0) {

		}
		return;
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