/**
 * Copyright - Sébastien Piérard <sebastien.pierard@ulg.ac.be> - 2016
 * Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be>         - 2016
 * http://www.montefiore.ulg.ac.be/~blaugraud
 * http://www.telecom.ulg.ac.be/research/sbg
 *
 * LaBGen-P is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LaBGen-P is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <algorithm>
#include <stdexcept>

#include <opencv2/core/core.hpp>

/* ========================================================================== *
 * SummedAreaTables                                                           *
 * ========================================================================== */

/**
 * This class implements the method known as "Integral Image Representation"
 * or "Summed area tables" introduced by F. Crown at SIGGRAPH 1984.
 */
template <typename T>
class SummedAreaTables {
  private :

    int w;
    int h;
    T* sum;

  public :

    SummedAreaTables(const cv::Mat& mat);

    virtual ~SummedAreaTables();

    T getIntegral(int row, int col) const;

    T getIntegral(int min_row, int max_row, int min_col, int max_col) const;
};

#define _SUMMED_AREA_TABLES_TPP_
#include "SummedAreaTables.tpp"
#undef  _SUMMED_AREA_TABLES_TPP_
