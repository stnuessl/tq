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

#ifndef _FAVORITES_HPP_
#define _FAVORITES_HPP_

#include <vector>
#include <set>
#include <ostream>

#include <boost/filesystem.hpp>

#include "response-handler.hpp"

namespace fs = boost::filesystem;

class bookmarks {
public:
    explicit bookmarks(const std::string &path);
    ~bookmarks();
    
    void add(const std::string &name);
    void remove(const std::string &name);
    void check(response_handler &handler);
    
    friend std::ostream &operator<<(std::ostream &o, const bookmarks &bm);
    
private:
    std::vector<std::string> read_bookmarks() const;
    void write_bookmarks(const std::vector<std::string> &vec,
                         std::set<std::string> &set) const;
    const fs::path _path;
};

#endif /* _FAVORITES_HPP_ */