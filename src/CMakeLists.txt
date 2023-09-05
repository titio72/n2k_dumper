#(C) 2023, Andrea Boni
#This file is part of n2k_dumper.
#n2k_dumper is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#n2k_dumper is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#You should have received a copy of the GNU General Public License
#along with n2k_dumper.  If not, see <http://www.gnu.org/licenses/>.

add_executable(n2k_dumper
  main.cpp
  Utils.cpp
  Log.cpp
  N2K.cpp
)

include_directories(../src)

target_link_libraries(n2k_dumper /home/aboni/Documents/PlatformIO/Projects/n2k_router_arduino/linux_lib/libnmea2000.a)
