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

#include "query.hpp"
#include "response-printer.hpp"
#include "bookmarks.hpp"


bookmarks::bookmarks(const std::string &path)
    : file(path)
{
}

void bookmarks::add(const std::string &name)
{
    add(std::vector<std::string>({ name }));
}

void bookmarks::add(const std::vector<std::string> &names)
{
    std::set<std::string> set;
    
    auto favs = read_bookmarks();
    
    for (auto &x : names)
        favs.emplace_back(x);
    
    write_bookmarks(favs, set);
}


void bookmarks::remove(const std::string &name)
{
    remove(std::vector<std::string> ({ name }));
}

void bookmarks::remove(const std::vector<std::string> &names)
{
    std::set<std::string> set;
    
    for (auto &x : names)
        set.emplace(x);
    
    auto favs = read_bookmarks();
    
    write_bookmarks(favs, set);
}

void bookmarks::check(response_printer &handler, query &query)
{
    auto favs = read_bookmarks();
    
    for (const auto &f : favs) {
        query.set_name(f);
        
        auto r = query.get_response(query::TYPE_STREAMS);
        
        handler.print_response(r);
    }
}

std::ostream &operator<<(std::ostream &o, const bookmarks &bm)
{
    auto favs = bm.read_bookmarks();
    
    for (const auto &f : favs)
        o << "  " << f << "\n";
    
    return o;
}

std::vector<std::string> bookmarks::read_bookmarks() const
{
    std::vector<std::string> ret;
    
    std::ifstream reader(c_str(), std::ios::in);
    std::string line;
    
    while(std::getline(reader, line)) {
        
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
    std::ofstream writer(c_str(), std::ios::out);
    
    for (auto x : vec) {
        if (set.count(x) == 0) {
            writer << x << "\n";
            set.insert(x);
        }
    }
}

