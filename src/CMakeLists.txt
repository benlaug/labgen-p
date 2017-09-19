# Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2017
# http://www.montefiore.ulg.ac.be/~blaugraud
# http://www.telecom.ulg.ac.be/labgen
#
# This file is part of LaBGen-P.
#
# LaBGen-P is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# LaBGen-P is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with LaBGen-P.  If not, see <http://www.gnu.org/licenses/>.
file(
  GLOB_RECURSE
  LaBGen-P_src
  *.cpp
)

# Shared library.
add_library(
  LaBGen-P_shared
  SHARED
  ${LaBGen-P_src}
  ${LaBGen-P_include}
)

set_target_properties(
  LaBGen-P_shared
  PROPERTIES
  OUTPUT_NAME
  LaBGen-P
)

target_link_libraries(
  LaBGen-P_shared
  ${Boost_LIBRARIES}
  ${OpenCV_LIBS}
)

# Static library.
add_library(
  LaBGen-P_static
  STATIC
  ${LaBGen-P_src}
  ${LaBGen-P_include}
)

set_target_properties(
  LaBGen-P_static
  PROPERTIES
  OUTPUT_NAME
  LaBGen-P
)

target_link_libraries(
  LaBGen-P_static
  ${Boost_LIBRARIES}
  ${OpenCV_LIBS}
)
