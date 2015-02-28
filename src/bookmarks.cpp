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

#include <algorithm>
#include <fstream>

#include "query.hpp"
#include "bookmarks.hpp"

bookmarks::bookmarks(const std::string &path)
    : file(path){
}

void bookmarks::add(const std::string &name)
{
    add(std::vector<std::string>({ name }));
}

void bookmarks::add(const std::vector<std::string> &names)
{
    string_ptr_set set;
    
    auto favs = read();
    
    for (auto &x : names)
        favs.emplace_back(x);
    
    write(favs, set);
}


void bookmarks::remove(const std::string &name)
{
    remove(std::vector<std::string> ({ name }));
}

void bookmarks::remove(const std::vector<std::string> &names)
{
    string_ptr_set set;
    
    for (auto &x : names)
        set.insert(&x);
    
    auto favs = read();
    
    write(favs, set);
}

void bookmarks::check(response_printer &printer, query &query)
{
    auto favs = read();
    
    auto response = query.streams(favs);
    
    printer.print_response(response);
}

std::ostream &operator<<(std::ostream &o, const bookmarks &bm)
{
    auto favs = bm.read();
    
    for (const auto &x : favs)
        o << x << "\n";
        
    return o;
}

bool bookmarks::comp::operator() (const std::string *a, 
                                  const std::string *b) const
{
    return *a == *b;
}

std::size_t bookmarks::hash::operator()  (const std::string *a) const
{
    static const std::hash<std::string> hash_func;
    
    return hash_func(*a);
}

std::vector<std::string> bookmarks::read() const
{
    std::vector<std::string> ret;
    
    std::ifstream reader(c_str(), std::ios::in);
    std::string line;
    
    while(std::getline(reader, line)) {
        
        if (line[0] == '#' || line[0] == ';')
            continue;
        
        auto begin = line.begin();
        auto end   = line.end();
        
        auto pred = [](char c) { return !std::isalnum(c); };
        
        line.erase(std::remove_if(begin, end, pred), line.end());
        
        ret.emplace_back(line);
    }
    
    return ret;
}

void bookmarks::write(const std::vector<std::string> &vec, 
                      string_ptr_set &set) const
{
    std::ofstream writer(c_str(), std::ios::out);
    
    for (auto &x : vec) {
        if (set.count(&x) == 0) {
            writer << x << "\n";
            set.insert(&x);
        }
    }
}

