# tq - Twitch Query

Project is currently in development.

## Overview

TODO

## Why tq?

Just have a look at the [twitch.tv](www.twitch.tv) website. I find it way too slow 
and I always end up hitting the wrong buttons because the website is still 
rearranging stuff which is really annoying. Also, I prefer watching twitch using 
[livestreamer](http://livestreamer.readthedocs.org/en/latest/#) and vlc.
I wanted a tool which lets me track my prefered streams without
using the website and an twitch account and therefore created tq.

## Installation

This section describes how to install tq on your machine. However, I use for my
day to day tasks Arch Linux which is why you could run into trouble using a
different GNU/Linux distribution.

### Dependencies

Dependencies for tq currently are:

* [curl](http://www.curl.haxx.se/)
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
* [boost](http://www.boost.org/)

In the following you find a description on what packages need to be 
installed on some GNU/Linux distributions.

#### Arch Linux

On Arch Linux you will need the following packages:

* [curl](https://www.archlinux.org/packages/core/x86_64/curl/)
* [jsoncpp](https://www.archlinux.org/packages/extra/x86_64/jsoncpp/)
* [boost](https://www.archlinux.org/packages/extra/x86_64/boost/)

Run the following command to install them.

```
 # pacman -Syu curl jsoncpp boost
```

#### Debian (Jessie)

If using Debian you will need to install the following packages:

* [libcurl4-openssl-dev](https://packages.debian.org/de/jessie/libcurl4-openssl-dev)
* [libjsoncpp-dev](https://packages.debian.org/de/jessie/libjsoncpp-dev)
* [libboost-program-options-dev](https://packages.debian.org/de/jessie/libboost-program-options-dev)
* [libboost-filesystem-dev](https://packages.debian.org/de/jessie/libboost-filesystem-dev)
* [libboost-system-dev](https://packages.debian.org/de/jessie/libboost-system-dev)

Run the following command to install them:

```
# apt-get install libcurl4-openssl-dev libjsoncpp-dev libboost-program-options-dev libboost-filesystem-dev libboost-system-dev
``` 

#### Fedora

TODO

### Compiling tq

To compile tq you will need to following tools:

* git
* g++
* cmake

Please make sure that they are installed on your system.

Run the following commands to compile and install tq.

```
 $ git clone https://github.com/stnuessl/tq
 $ mkdir tq/build
 $ cd tq/build
 $ cmake ../
 $ make
 # make install
```

Note that the last command is run as user __root__.

## Usage

TODO / Run tq --help


## Bugs and bug reports

You can leave a bug report on the [github project page](https://github.com/stnuessl/tq/issues), 
however don't expect to get them fixed soon. Make sure to leave a note on how to reproduce the bug and 
the output of the application (including the log file if applicable).
