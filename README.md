# tq - Twitch Query

Project is currently in development.

## Overview

TODO

## Why tq?

Just have a look at the twitch.tv website. I find it too slow and I always hit
the wrong buttons because the website is still rearranging stuff which is really
annoying. Also, I prefer watching twitch using 
[livestreamer](http://livestreamer.readthedocs.org/en/latest/#) and vlc.
I wanted a tool which lets me track my prefered channels and players without
using the website and an twitch account and therefore created tq.

## Installation

This section describes how to install tq on your machine. However, I use for my
day to day tasks Arch Linux which is why you could run into trouble using a
different GNU/Linux distribution.

### Dependencies

Dependencies for tq currently are:

* [curl](http://www.curl.haxx.se/)
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp)

In the following you find a description on what packages need to be 
installed on some GNU/Linux distributions.

#### Arch Linux

On Arch Linux you will need the following packages:

* [curl](https://www.archlinux.org/packages/core/x86_64/curl/)
* [jsoncpp](https://www.archlinux.org/packages/extra/x86_64/jsoncpp/)

Run the following command to install them.

```
 # pacman -Syu curl jsoncpp
```

#### Debian

TODO

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
 $ cmake ..     
 $ make
 # make install
```

Note that the last command is run as user __root__.

## Usage

TODO

## Bugs and bug reports

You can leave a bug report on the [github project page](https://github.com/stnuessl/tq/issues), 
however don't expect to get them fixed soon. Make sure to leave a note on how to reproduce the bug and 
the output of the application (including the log file if applicable).