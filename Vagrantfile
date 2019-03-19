# -*- mode: ruby -*-
# vi: set ft=ruby :

# All Vagrant configuration is done below. The "2" in Vagrant.configure
# configures the configuration version (we support older styles for
# backwards compatibility). Please don't change it unless you know what
# you're doing.
Vagrant.configure("2") do |config|
  # The most common configuration options are documented and commented below.
  # For a complete reference, please see the online documentation at
  # https://docs.vagrantup.com.

  # Every Vagrant development environment requires a box. You can search for
  # boxes at https://vagrantcloud.com/search.
  config.vm.box = "bento/ubuntu-18.04"
  config.vm.network :private_network, ip: "192.168.33.10"
  config.vm.network "forwarded_port", guest: 80, host: 8080
  config.vm.network "forwarded_port", guest: 8181, host: 8181
  config.vm.synced_folder ".", "/vagrant", disabled: true
  config.vm.synced_folder "./shared", "/home/vagrant/shared", create:true,
  :owner => "root",
  :group => "root",
  :mount_options => ["dmode=775","fmode=664"]

  # Disable automatic box update checking. If you disable this, then
  # boxes will only be checked for updates when the user runs
  # `vagrant box outdated`. This is not recommended.
  # config.vm.box_check_update = false

  # Create a forwarded port mapping which allows access to a specific port
  # within the machine from a port on the host machine. In the example below,
  # accessing "localhost:8080" will access port 80 on the guest machine.
  # NOTE: This will enable public access to the opened port
  # config.vm.network "forwarded_port", guest: 80, host: 8080

  # Create a forwarded port mapping which allows access to a specific port
  # within the machine from a port on the host machine and only allow access
  # via 127.0.0.1 to disable public access
  # config.vm.network "forwarded_port", guest: 80, host: 8080, host_ip: "127.0.0.1"

  # Create a private network, which allows host-only access to the machine
  # using a specific IP.
  # config.vm.network "private_network", ip: "192.168.33.10"

  # Create a public network, which generally matched to bridged network.
  # Bridged networks make the machine appear as another physical device on
  # your network.
  # config.vm.network "public_network"

  # Share an additional folder to the guest VM. The first argument is
  # the path on the host to the actual folder. The second argument is
  # the path on the guest to mount the folder. And the optional third
  # argument is a set of non-required options.
  # Provider-specific configuration so you can fine-tune various
  # backing providers for Vagrant. These expose provider-specific options.
  # Example for VirtualBox:
  #
  config.vm.provider "virtualbox" do |vb|
  #   # Display the VirtualBox GUI when booting the machine
    vb.gui = false
    vb.customize ["modifyvm", :id, "--natdnshostresolver1", "on"]
    vb.customize ["modifyvm", :id, "--natdnsproxy1", "on"]
  #
  #   # Customize the amount of memory on the VM:
    vb.memory = "1024"
  end
  #
  # View the documentation for the provider you are using for more
  # information on available options.

  # Enable provisioning with a shell script. Additional provisioners such as
  # Puppet, Chef, Ansible, Salt, and Docker are also available. Please see the
  # documentation for more information about their specific syntax and use.
  config.vm.provision "shell", inline: <<-SHELL
  	 apt-cache depends clang
  	 apt-cache policy clang-6.0sudo apt-get install libgtest-dev

     apt-get update
     apt-get install -y apache2
     sudo apt -y install clang  
     sudo apt -y install gcc 
     sudo apt -y install build-essential < Y
     sudo apt-get install -y libgtest-dev
     sudo apt-get install -y cmake # install cmake
     cd /usr/src/gtest
     sudo cmake CMakeLists.txt
     sudo make
     sudo cp *.a /usr/local/lib
     git clone https://github.com/google/glog.git
     cd glog/
     ./autogen.sh && ./configure && make && make install
     sudo cmake CMakeLists.txt
     sudo make
     sudo make install prefix=/usr/local/
     sudo apt-get install -y autoconf automake libtool curl make g++ unzip
     cd ~/
     git clone https://github.com/protocolbuffers/protobuf.git
     cd protobuf
     git submodule update --init --recursive
     ./autogen.sh
     ./configure
     make
     make check
     export LD_LIBRARY_PATH=/usr/local/lib
     sudo make install
     sudo ldconfig
     cd ~/
     git clone
     sudo apt-get install -y pkg-config
     sudo apt-get install -y autoconf automake libtool make g++ unzip
     sudo apt-get install -y libgflags-dev libgtest-dev
     sudo apt-get install -y clang libc++-dev
      git clone https://github.com/grpc/grpc.git
      cd grpc
      git submodule update --init  #更新第三方源码
      make
      sudo make install prefix=usr/local/
      


   SHELL
end


