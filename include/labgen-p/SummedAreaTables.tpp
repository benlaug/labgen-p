/**
 * Copyright - Sébastien Piérard <sebastien.pierard@ulg.ac.be> - 2016
 * Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be>         - 2016
 * http://www.montefiore.ulg.ac.be/~blaugraud
 * http://www.telecom.ulg.ac.be/labgen
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
 * along with LaBGen-P.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef    _LABGEN_P_SUMMED_AREA_TABLES_TPP_
#error "SummedAreaTables.hpp must be included instead of SummedAreaTables.tpp"
#else
/* ========================================================================== *
 * SummedAreaTables                                                           *
 * ========================================================================== */

template <typename T>
SummedAreaTables<T>::SummedAreaTables(const cv::Mat& mat) :
w(mat.cols),
h(mat.rows),
sum(new T[mat.total()]) {
  if (w == 0)
    throw std::logic_error("Image with zero width are not supported");

  if (h == 0)
    throw std::logic_error("Image with zero height are not supported");

  const T* buffer = reinterpret_cast<const T*>(mat.data);

  for (int row = 0; row < h; ++row) {
    for (int col = 0; col < w; ++col) {
      sum[row * w + col] =
        *(buffer++)                   +
        getIntegral(row - 1, col    ) +
        getIntegral(row    , col - 1) -
        getIntegral(row - 1, col - 1) ;
    }
  }
}

/******************************************************************************/

template <typename T>
SummedAreaTables<T>::~SummedAreaTables() {
  delete[] sum;
}

/******************************************************************************/

template <typename T>
inline T SummedAreaTables<T>::getIntegral(int row, int col) const {
  if (row < 0)
    return T();

  if (col < 0)
    return T();

  return sum[std::min(row, h - 1) * w + std::min(col, w - 1)];
}

/******************************************************************************/

template <typename T>
T SummedAreaTables<T>::getIntegral(
  int min_row, int max_row, int min_col, int max_col
) const {
  if (min_row > max_row)
    return T();

  if (min_col > max_col)
    return T();

  return
    getIntegral(max_row    , max_col    ) -
    getIntegral(min_row - 1, max_col    ) -
    getIntegral(max_row    , min_col - 1) +
    getIntegral(min_row - 1, min_col - 1) ;
}
#endif /* _LABGEN_P_SUMMED_AREA_TABLES_TPP_ */
