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
#include <utility>
#include <fstream>

#include <boost/filesystem.hpp>

#include "file.hpp"

file::file(const std::string &path)
    : _path(path)
{
    if (!fs::exists(_path.parent_path())) {
        bool ok = fs::create_directories(_path.parent_path());
        if (!ok)
            throw std::runtime_error("Failed to create directories.");
    }
    
    if (!fs::exists(_path)) {
        /* just create the file */
        std::ofstream file(_path.c_str(), std::ios::out);
    }
    
    if (!fs::is_regular_file(_path)) {
        std::string f(_path.c_str());
        throw std::runtime_error("File \"" + f + "\" is not a regular file.");
    }
}

const char *file::c_str() const
{
    return _path.c_str();
}
