/*
 * Copyright (C) 2014  Steffen Nüssle
 * tq - Twitch Query
 *
 * This file is part of tq.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <iostream>

#include <unistd.h>

#include "stream-opener.hpp"

extern char **environ;

static char *c_str_dup(const std::string &str)
{
    char *s = (char *) malloc(str.length() + 1);
    if (!s)
        std::exit(EXIT_FAILURE);
    
    return strcpy(s, str.c_str());
}

stream_opener::stream_opener(const config &config)
    : _opener(config.stream_opener()),
      _args(config.stream_opener_args())
{
}


void stream_opener::open(const std::string &stream)
{
    static const std::string base_url("www.twitch.tv/");
    
    if (_opener.empty())
        throw std::runtime_error("No stream opener specified");
    
    /* fork a child and start the stream-opener process */
    pid_t pid = fork();
    
    if (pid == (pid_t) -1)
        throw std::runtime_error("Forking new process failed.");
    
    if (pid > 0)
        return;
    
    close(STDIN_FILENO);
//     close(STDOUT_FILENO);
//     close(STDERR_FILENO);
    
    /* 'char *const argv[]' is kind of a funny interface ... */

    char *bin  = c_str_dup(_opener);
    char *url  = c_str_dup(base_url + stream);
    
    int argc = _args.size() + 3;
    char **argv = (char **) std::calloc(argc, sizeof(*argv));
    
    argv[0] = bin;
    argv[1] = url;
    
    for (std::size_t i = 0; i < _args.size(); ++i)
        argv[i + 2] = c_str_dup(_args[i]);

    execve(bin, argv, environ);
    
    std::exit(EXIT_FAILURE);
}
