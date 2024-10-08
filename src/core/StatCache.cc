/*
 *  OpenSCAD (www.openscad.org)
 *  Copyright (C) 2009-2011 Clifford Wolf <clifford@clifford.at> and
 *                          Marius Kintel <marius@kintel.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  As a special exception, you have permission to link this program
 *  with the CGAL library and distribute executables, as long as you
 *  follow the requirements of the GNU GPL in regard to all of the
 *  software in the executable aside from CGAL.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "core/StatCache.h"

#include <string>
#include <unordered_map>
#include <chrono>

namespace {

const float stale = 190;  // 190ms, maximum lifetime of a cache entry chosen to be shorter than the automatic reload poll time

double millis_clock()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

struct CacheEntry
{
  struct stat st;  // result from stat
  double timestamp; // the time stat was called
};

std::unordered_map<std::string, CacheEntry> statMap;

} // namespace

namespace StatCache {

int stat(const std::string& path, struct ::stat &st)
{
  auto iter = statMap.find(path);
  if (iter != statMap.end()) {                // Have we got an entry for this file?
    if (millis_clock() - iter->second.timestamp < stale) {
      st = iter->second.st;      // Not stale yet so return it
      return 0;
    }
    statMap.erase(iter);                // Remove stale entry
  }
  if (auto rv = ::stat(path.c_str(), &st)) return rv; // stat failed
  statMap[path] = {st, millis_clock()};
  return 0;
}

} // namespace StatCache
