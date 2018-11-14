# Telegram Shadowsocks Manager Bot

A telegram bot to control your Shadowsocks server by communicate with ss-manager.

## Features

- [x] Get current flow.
- [x] Add / remove port.

## TODO
- Flow graph
- Data persistence

## Install

Clone the repository :

```bash
git clone --recurse-submodules https://github.com/ingjieye/telesh
```

Install dependencies :

```bash
sudo apt install libcurl4-openssl-dev g++ make binutils cmake libssl-dev libboost-system-dev
```

Build :

```bash
mkdir build && cmake .. && make
```

## Usage

Launch server with 

```
$ ./server <telegram bot key> <ss-manager port>
```

The first one who sent the message to the bot will be the admin