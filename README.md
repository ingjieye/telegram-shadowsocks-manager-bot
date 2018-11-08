# Telegram Shadowsocks Manager Bot

A telegram bot to control your Shadowsocks server by communicate with ss-manager.

## Features

- [x] Get current flow.
- [x] Add / remove port.
- [ ] Flow graph

## Install

Clone the repository :

```bash
git clone --recurse-submodules https://github.com/ingjieye/telesh
```

Install dependencies :

```bash
sudo apt install libcurl4-openssl-dev gcrystal libgcrypt20-dev libjsoncpp-dev
```

Compile the xxtelebot static lib :

```bash
cd xxtelebot && mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE="Release" -DBUILD_SHARED_LIBS=OFF && make
```

Compile main program :

```
cd ../../ && make
```

## Usage

Launch server with 

```
$ ./server.out <telegram bot key> <ss-manager port>
```

The first one who sent the message to the bot will be the admin