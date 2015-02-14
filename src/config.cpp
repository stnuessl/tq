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

#include <string>
#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>

#include "config.hpp"

namespace opt = boost::program_options;

config::config(const std::string &path)
    : file(path),
      _int_len(11),
      _name_len(20),
      _game_len(40),
      _opener(),
      _args()
{
    opt::options_description desc;
    
    desc.add_options()
        ("printer.integer-length", opt::value(&_int_len))
        ("printer.name-length",    opt::value(&_name_len))
        ("printer.game-length",    opt::value(&_game_len))
        ("stream.opener",          opt::value(&_opener))
        ("stream.arg",             opt::value(&_args));
    
    try {
        opt::variables_map conf_var_map;
        std::ifstream file(path);
        
        auto parsed = opt::parse_config_file(file, desc);
        opt::store(parsed, conf_var_map);
        opt::notify(conf_var_map);
        
        file.close();
        
        /*
         * [stream]
         * #opener = /usr/bin/chromium
         * #arg = --incognito
         * 
         * opener  = /usr/bin/livestreamer
         * arg = best
         * arg = --no-version-check
         * #arg = --loglevel=debug
         * 
         */
        
        if (conf_var_map.empty()) {
            std::ofstream file(path);
            
            file << "[printer]\n"
                 << "integer-length = " << _int_len     << "\n"
                 << "name-length    = " << _name_len    << "\n"
                 << "game-length    = " << _game_len    << "\n\n"
                 << "[stream]\n"
                 << "#opener = /usr/bin/livestreamer\n"
                 << "#arg = best\n"
                 << "#arg = --no-version-check\n"
                 << "#arg = --loglevel=debug\n\n"
                 << "#opener = /usr/bin/chromium\n"
                 << "#arg = --incognito\n"
                 << "#arg = --start-maximized\n\n"
                 << "#opener = /usr/bin/firefox\n";
        }
        
    } catch (std::exception &e) {
        std::cerr << "** Warning: unable to parse config \"" << path << "\"- " 
                  << e.what() << "\n";
    }
}

unsigned int config::integer_length() const
{
    return _int_len;
}


unsigned int config::name_length() const
{
    return _name_len;
}


unsigned int config::game_length() const
{
    return _game_len;
}

const std::string &config::stream_opener() const
{
    return _opener;
}

const std::vector<std::string> &config::stream_opener_args() const
{
    return _args;
}