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
#include "tgbot/tgbot.h"

using namespace TgBot;
using namespace std;

int verbose = 1;
int64_t admin_id = 0;
int manager_port;

typedef struct {
	int port;
	uint64_t data;
} traffic;

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
	int lenth = recvfrom(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *) &sin, &sin_len);
	buf[lenth] = '\0';
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

vector<traffic> GetTrafficData()
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

string GetTrafficString()
{
	const vector<string> data_unit = {"B", "KB", "MB", "GB", "TB", "PB"};
	string ret;
	auto v = GetTrafficData();
	for(auto& a: v) {
		ret += to_string(a.port) + ": ";
		int time = 0;
		double traffic = a.data;
		while(traffic > 1024) {
			traffic /= 1024;
			time ++;
		}
		ret += to_string(traffic);
		ret += data_unit[time] + "\n";
	}
	return ret;
}

string HandleCmd(const string& msg)
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

int main(int argc, char* argv[])
{
	if(argc != 3) {
		printf("usage : %s <telegram bot key> <ss-manager port>\n", argv[0]);
		exit(1);
	}

	manager_port = atoi(argv[2]);

	Bot bot(argv[1]);

    bot.getEvents().onAnyMessage([&bot](Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
		try {
			if(admin_id == 0) {
				admin_id = message->from->id;
			} else if (message->from->id != admin_id) {
				throw invalid_argument("Not admin");
			}

	        bot.getApi().sendMessage(message->chat->id, HandleCmd(message->text));
		}catch(exception& e) {
			bot.getApi().sendMessage(message->chat->id, e.what());
		}
    });

	printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
	bot.getApi().deleteWebhook();

	TgLongPoll longPoll(bot);
	while (true) {
		printf("Long poll started\n");
		longPoll.start();
	}
	
	return 0;
}