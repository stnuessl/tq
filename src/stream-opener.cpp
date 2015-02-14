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
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "stream-opener.hpp"

extern char **environ;

stream_opener::stream_opener(const std::shared_ptr<const config> conf)
    : _config(conf)
{
}


void stream_opener::run(const std::string &stream)
{
    static const char path[] = "/tmp/tq-stream-opener.txt";
    const std::string url("www.twitch.tv/" + stream);
    
    auto opener = _config->stream_opener();
    auto args   = _config->stream_opener_args();
    
    if (opener.empty()) {
        std::string err_msg = "No stream opener specified: have a look at ";
        err_msg += "the configuration file \"";
        err_msg += _config->c_str();
        err_msg += "\".";
        
        throw std::runtime_error(err_msg);
    }
    
    int fd = open(path, O_WRONLY | O_CREAT, 0644);
    if (fd < 0) {
        std::string err_msg = "Failed to create \"";
        err_msg += path;
        err_msg += "\" output file - ";
        err_msg += std::strerror(errno);
        throw std::runtime_error(err_msg);
    }
    std::cout << "Redirecting stdout and stderr of \"" << opener 
              << "\" to file \"" << path << "\"." << std::endl;
              
    /* fork a child and start the stream-opener process */
    pid_t pid = fork();
    
    if (pid == (pid_t) -1) {
        std::string err_msg = "forking stream-opener process failed - ";
        err_msg += std::strerror(errno);
        
        throw std::runtime_error(err_msg);
    }
    
    if (pid > 0) {
        close(fd);
        return;
    }
    
    close(STDIN_FILENO);
    
    try {
        
        /* redirect output: do not interfere with the tq terminal output */
        if (dup2(fd, STDOUT_FILENO) < 0) {
            std::string err_msg("stream-opener: ");
            err_msg += "failed to redirect stdout of \"";
            err_msg += opener;
            err_msg += "\" - ";
            err_msg += std::strerror(errno);
            err_msg += ".";
            throw std::runtime_error(err_msg);
        }
        
        if (dup2(fd, STDERR_FILENO) < 0) {
            std::string err_msg("stream-opener: ");
            err_msg += "failed to redirect stderr of \"";
            err_msg += opener;
            err_msg += "\" - ";
            err_msg += std::strerror(errno);
            err_msg += ".";
            throw std::runtime_error(err_msg);
        }
        
        /* 'char *const argv[]' is kind of a funny interface ... */
        
        int argc = args.size() + 3;
        const char **argv = (const char **) std::calloc(argc, sizeof(*argv));
        if (!argv)
            throw std::runtime_error("stream-opener: out of memory.");
        
        argv[0] = opener.c_str();
        argv[1] = url.c_str();
        
        for (std::size_t i = 0; i < args.size(); ++i)
            argv[i + 2] = args[i].c_str();

        execve(opener.c_str(), (char ** const) argv, environ);
    
        std::string err_msg("stream-opener: failed to execute process \"");
        err_msg += opener;
        err_msg += "\" - ";
        err_msg += std::strerror(errno);
        err_msg += ".";
        
        throw std::runtime_error(err_msg);
        
    } catch (std::exception &e) {
        dprintf(fd, "%s\n", e.what());
        std::exit(EXIT_FAILURE);
    }
}
