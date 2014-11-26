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

#include <vector>
#include <set>
#include <string>
#include <fstream>

#include <boost/filesystem.hpp>

#include "response-handler.hpp"
#include "bookmarks.hpp"


bookmarks::bookmarks(const std::string &path)
    : _path(path)
{
}

bookmarks::~bookmarks()
{
}

void bookmarks::add(const std::string &name)
{
    std::set<std::string> set;
    
    auto favs = read_bookmarks();
    
    favs.emplace_back(name);
    
    write_bookmarks(favs, set);
}

void bookmarks::remove(const std::string &name)
{
    std::set<std::string> set ({ name });
    
    auto favs = read_bookmarks();
    
    write_bookmarks(favs, set);
}

void bookmarks::check(response_handler &handler)
{
    auto favs = read_bookmarks();
    query_builder builder(query_builder::TYPE_STREAMS);
    
    for (const auto &f : favs) {
        builder.set_query(f);
        
        auto response = builder.build().get_response();
        handler.handle_response(response, query_builder::TYPE_STREAMS);
    }
}

std::ostream &operator<<(std::ostream &o, const bookmarks &bm)
{
    auto favs = bm.read_bookmarks();
    
    for (const auto &f : favs)
        o << "  " << f << "\n";
    
    return o;
}

std::vector< std::string > bookmarks::read_bookmarks() const
{
    std::vector<std::string> ret;
    
    if (!fs::exists(_path)) {
        std::string f(_path.c_str());
        throw std::runtime_error("File \"" + f + "\" does not exist.");
    }
    
    if (!fs::is_regular_file(_path)) {
        std::string f(_path.c_str());
        throw std::runtime_error("File \"" + f + "\" is not a regular file.");
    }
    
    std::ifstream file(_path.c_str(), std::ios::in);
    std::string line;
    query_builder builder(query_builder::TYPE_STREAMS);
    
    while(std::getline(file, line)) {
        
        if (line[0] == '#' || line[0] == ';')
            continue;
        
        auto it  = line.begin();
        auto end = line.end();
        
        while (it != end && !std::isalnum(*it))
            it++;
        
        line.erase(line.begin(), it);
        
        while (!line.empty() && !std::isalnum(line.back()))
            line.pop_back();
        
        ret.emplace_back(line);
    }
    
    return ret;
}

void bookmarks::write_bookmarks(const std::vector<std::string> &vec, 
                                std::set< std::string > &set) const
{
    std::ofstream file(_path.c_str(), std::ios::out);
    
    for (auto x : vec) {
        if (set.count(x) == 0) {
            file << x << "\n";
            set.insert(x);
        }
    }
}

