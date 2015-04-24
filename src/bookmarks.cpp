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
#include <utility>

#include "query.hpp"
#include "bookmarks.hpp"

static void trim_spaces(std::string &str)
{
    /* remove all leading and trailing whitespaces */
    auto it = str.begin();
    
    while (it != str.cend() && std::isspace(*it))
        ++it;
    
    str.erase(str.begin(), it);
    
    while (!str.empty() && std::isspace(str.back()))
        str.pop_back();
}

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
    if (names.empty())
        return;
    
    string_ptr_set set;
    
    auto favs = read();
    
    favs.insert(favs.end(), names.begin(), names.end());

    write(favs, set);
}


void bookmarks::remove(const std::string &name)
{
    remove(std::vector<std::string> ({ name }));
}

void bookmarks::remove(const std::vector<std::string> &names)
{
    if (names.empty())
        return;
    
    string_ptr_set set;

    for (auto &x : names)
        set.insert(&x);
    
    auto favs = read();
    
    write(favs, set);
}

void bookmarks::check(response_printer &printer, query &query)
{
    auto favs = read();
    
    if (favs.empty())
        return;
    
    auto response = query.streams(favs);
    
    printer.print_response(response);
}

std::ostream &operator<<(std::ostream &o, const bookmarks &bm)
{
    auto favs = bm.read();
    
    o << "[ Bookmarks ]:\n";
    
    for (const auto &x : favs)
        o << "  " << x << "\n";
        
    return o;
}

bool bookmarks::comp::operator() (const std::string *a, 
                                  const std::string *b) const
{
    return *a == *b;
}

std::size_t bookmarks::hash::operator() (const std::string *a) const
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

        trim_spaces(line);
        
        if (!line.empty())
            ret.push_back(std::move(line));
    }
    
    return ret;
}

void bookmarks::write(std::vector<std::string> &vec, 
                      string_ptr_set &set) const
{
    std::ofstream writer(c_str(), std::ios::out);
    
    for (auto &x : vec) {
        trim_spaces(x);
        
        if (set.count(&x) == 0) {
            writer << x << "\n";
            set.insert(&x);
        }
    }
}

