# tq - Twitch Query

tq is a simple and easy to use command-line tool to query information about
streams and channels on the [twitch.tv](http://www.twitch.tv) website.

## Overview

* [Why tq?](https://github.com/stnuessl/tq#why-tq)
* [Installation](https://github.com/stnuessl/tq#installation)
    * [Dependencies](https://github.com/stnuessl/tq#dependencies)
        * [Arch Linux](https://github.com/stnuessl/tq#arch-linux)
        * [Debian](https://github.com/stnuessl/tq#debian-jessie)
        * [Fedora](https://github.com/stnuessl/tq#fedora)
    * [Compiling tq](https://github.com/stnuessl/tq#compiling-tq)
* [Usage](https://github.com/stnuessl/tq#usage)
    * [Get help](https://github.com/stnuessl/tq#get-help)
    * [Query top played games](https://github.com/stnuessl/tq#query-top-played-games)
    * [Get descriptive output](https://github.com/stnuessl/tq#get-descriptive-output)
    * [Query featured streams](https://github.com/stnuessl/tq#query-featured-streams)
    * [Retrieve information about a channel / stream](https://github.com/stnuessl/tq#retrieve-information-about-a-channel--stream)
    * [Search for channels / streams / games](https://github.com/stnuessl/tq#search-for-channels--streams--games)
    * [Nota bene](https://github.com/stnuessl/tq#nota-bene)
* [Bugs and bug reports](https://github.com/stnuessl/tq#bugs-and-bug-reports)

## Why tq?

Just have a look at the [twitch.tv](http://www.twitch.tv) website. I find it way too slow 
and I always end up hitting the wrong buttons because the website is still 
rearranging stuff which is really annoying. Also, I prefer watching twitch using 
[livestreamer](https://livestreamer.readthedocs.org/en/latest/) and 
[vlc](http://www.videolan.org/vlc/). I wanted a tool which lets me track my 
prefered streams without using the website and a twitch account and therefore 
created tq.

## Installation

This section describes how to compile and install tq on a few selected Linux
distributions. 

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

* [libcurl4-openssl-dev](https://packages.debian.org/en/jessie/libcurl4-openssl-dev)
* [libjsoncpp-dev](https://packages.debian.org/en/jessie/libjsoncpp-dev)
* [libboost-program-options-dev](https://packages.debian.org/en/jessie/libboost-program-options-dev)
* [libboost-filesystem-dev](https://packages.debian.org/en/jessie/libboost-filesystem-dev)
* [libboost-system-dev](https://packages.debian.org/en/jessie/libboost-system-dev)

Run the following command to install them:

```
# apt-get install libcurl4-openssl-dev libjsoncpp-dev libboost-program-options-dev libboost-filesystem-dev libboost-system-dev
``` 

#### Fedora

If you are using Fedora you will need to install the following packages:

* [boost-devel](http://rpmfind.net/linux/rpm2html/search.php?query=boost-devel)
* [jsoncpp-devel](http://rpmfind.net/linux/rpm2html/search.php?query=jsoncpp-devel)
* [libcurl-devel](http://rpmfind.net/linux/rpm2html/search.php?query=libcurl-devel)

Run the following command to install them:

```
# yum install boost-devel jsoncpp-devel libcurl-devel
``` 

### Compiling tq

To compile tq you will need the following tools:

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

This section describes the basic usage of tq. However, not all commands are
listed here, so you should always have a look at the built in __--help__ command
which is also described in the following.

### Get help

Run just __tq__ or __tq --help__ to get a list of available commands and a
short description. Example:

```
    $ tq --help
```

Note that for most commands there are also short options available.

### Query top played games

Run the following command to get a list of the currently most played games

```
    $ tq --top
```

### Get descriptive output

If you do not know what the output of the program means you can 
add the __--descriptive flag to the program arguments to get additional 
information, e.g.:

```
    $ tq --top --descriptive
```
or just:
```
    $ tq -td
```

The __--informative__ flag works with all commands which print one line for each
returned object (games, channels, streams).

### Query featured streams

Twitch has always a list of featured streams on their website. You can get
this list by running:

```
    $ tq --featured
```

### Retrieve information about a channel / stream

To get information about a channel run:

```
    $ tq -C [channel-name]
```
Note that this does not tell you whether the channel's stream is online.

For informations about a stream, run:

```
    $ tq -S [stream-name]
```

However, to be able to retrieve information, the stream has to be online.

### Search for channels / streams / games

Sometimes it happens that you do not know the exact name of a channel, a stream or
even a game. tq can help you by issuing a search request to the server with a 
search string. In the Following are three examples to search for channels, streams 
and games respectively:

```
    $ tq -c [name]
    $ tq -s [name]
    $ tq -g [name]
```

Probably, the most useful command is searching for streams that features a game of
your choice. __tq -s__ automatically searchs for such streams too.

### Nota bene

All options except __--limit__ are able to take multiple arguments, e.g.

```
    $ tq -C channel1 channel2 channel3
```

will print out information about all three channels.

## Bugs and bug reports

You can leave a bug report on the [github project page](https://github.com/stnuessl/tq/issues), 
however don't expect to get them fixed soon. Make sure to leave a note on how to reproduce the bug and 
the output of the application (including the log file if applicable).
