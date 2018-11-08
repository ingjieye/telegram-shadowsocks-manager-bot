INC = -I./xxtelebot/include
LIBXXTELEBOT = ./xxtelebot/build/lib/libxxtelebot.a -lcurl -ljsoncpp -pthread -lgcrypt

CXXFLAGS = $(INC) -o2 -Wall

SRC_TG_SS_BOT = server.cpp
OBJ_TG_SS_BOT = $(SRC_TG_SS_BOT:.cpp=.o)
PRJ_TG_SS_BOT = server

.DEFAULT_GOAL := $(PRJ_TG_SS_BOT)

$(PRJ_TG_SS_BOT): $(OBJ_TG_SS_BOT)
	$(CXX) $^ -o $@.out $(LIBXXTELEBOT)

.PHONY: clean
clean:
	rm -rf *.o \
		*.out