# 499-zhihanz-usc-edu
name: Zhihan Zhang
---------------

email: zhihanz@usc.edu
# How to deploy and setup?
Using the Vagrantfile in root directory.
Run `vagrant up`
It will automaticly deploy a native virtual machine for you
**sometims it may not automatically install grpc for you**
run the command below to install grpc
```
git clone -b $(curl -L http://grpc.io/release) https://github.com/grpc/grpc
cd grpc
git submodule update --init
make
sudo make install prefix=/usr/local/
```
# How to compile?
## kvstore
compile:
In the root
```
cd kvstore
make // may need to make several times
```
In kvstore
make their unittests(kvstore tested in unittestkvhelper
```
cd utils
make // may need to make several times
```
## service
compile:
In the root
```
cd service
make serive ## (may need to make several times) build service rpc service without unittest
```
If you want to make them all 
```
cd service
make (may need to make several times)
cd utils
make 
```
## command
compile:
In the root
```
cd command
make (may need to make several times
```
# How to use?
## Registration:
If I want to regist with a name like Adam
In `root/command/` file
```
./command -regist=Adam
```
## How to Chirp(send message):
If I want to chirp some thing like Hellow World with a name like Adam
In `root/command/` file
```
./command -user=Adam -chirp="Hello World"
```
**If user do not exists, It will return a blank chirp with a hint like**
user name do not exists

## How to reply to other chirp:
If I want to chirp some thing like Hellow Adam with a name like Rick and reply this message to a given id like 6501687367827456
This ID is send to the user as a part of chirp reply
In `root/command/` file
```
./command -user=Morty -chirp="Hello Adam" -reply="6501687367827456"
```
## How to follow a given guy?
If your user name is Adam and want to follow Morty.
In `root/command/` file
```
./command -user=Adam -follow=Morty
```
## How to monitor a given people?
If you want to monitor Adam
In `root/command/` file
```
./command -user=Adam -monitor
```
## How to stream a given hashtag?
If you want to stream chirps with #fun
In `root/command/` file
```
./command -user=Adam -stream="fun"
```
## How to chirp with a given hashtag?
If you want to monitor Adam
In `root/command/` file
```
./command -user=Adam -chirp="lets meet up" -stream="fun"
```