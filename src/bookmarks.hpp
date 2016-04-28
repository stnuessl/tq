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

#ifndef _BOOKMARKS_HPP_
#define _BOOKMARKS_HPP_

#include <string>
#include <vector>
#include <unordered_set>
#include <ostream>

#include "file.hpp"

class bookmarks : public file {
public:
    explicit bookmarks(const std::string &path);
    
    void add(const std::string &name);
    void add(const std::vector<std::string> &names);
    void remove(const std::string &name);
    void remove(const std::vector<std::string> &names);
    
    std::vector<std::string> get() const;
    
    friend std::ostream &operator<<(std::ostream &o, const bookmarks &bm);
private:
    struct comp {
        bool operator() (const std::string *a, const std::string *b) const;
    };
    
    struct hash {
        std::size_t operator() (const std::string *a) const;
    };
    
    typedef std::unordered_set<const std::string *, hash, comp> string_ptr_set;
    
    std::vector<std::string> read() const;
    void write(std::vector<std::string> &vec, string_ptr_set &set) const;
};

#endif /* _BOOKMARKS_HPP_ */
