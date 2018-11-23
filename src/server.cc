#include <string>
#include <iostream>
#include <vector>
#include <regex>
#include <algorithm>
#include <exception>
#include <thread>
#include "tgbot/tgbot.h"
#include "manager.h"

using namespace std;

int64_t admin_id = 0;

void guard()
{
	for(;;) {
		sleep(5);
		
	}
}

int main(int argc, char* argv[])
{
	if(argc != 3) {
		printf("usage : %s <telegram bot key> <ss-manager port>\n", argv[0]);
		exit(1);
	}
	
	Manager ssmanager(atoi(argv[2]));

	TgBot::Bot bot(argv[1]);

	thread t1(guard);

    bot.getEvents().onAnyMessage([&bot, &ssmanager](TgBot::Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
		try {
			if(admin_id == 0) {
				admin_id = message->from->id;
			} else if (message->from->id != admin_id) {
				throw invalid_argument("Not admin");
			}

	        bot.getApi().sendMessage(message->chat->id, ssmanager.HandleCmd(message->text));
		}catch(exception& e) {
			bot.getApi().sendMessage(message->chat->id, e.what());
		}
    });

	printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
	bot.getApi().deleteWebhook();

	TgBot::TgLongPoll longPoll(bot);
	while (true) {
		printf("Long poll started\n");
		longPoll.start();
	}
	
	return 0;
}