# 499-zhihanz-usc-edu
name: Zhihan Zhang
---------------

email: zhihanz@usc.edu
# How to deploy and setup?
Using the Vagrantfile in root directory.
Run `vagrant up`
It will automaticly deploy a native virtual machine for you
# How to compile?
## kvstore
compile:
In the root
```
cd kvstore
make
```
## service
compile:
In the root
```
cd service
make serive ## build service rpc service without unittest
```
If you want to make them all 
```
cd service
make
```
## command
compile:
In the root
```
cd command
make
```

