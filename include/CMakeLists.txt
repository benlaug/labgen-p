# Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2016
# http://www.montefiore.ulg.ac.be/~blaugraud
# http://www.telecom.ulg.ac.be/research/sbg
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
  LaBGen-P_include
  *.hpp
  *.tpp
)

set(
  LaBGen-P_include
  ${LaBGen-P_include}
  PARENT_SCOPE
)
