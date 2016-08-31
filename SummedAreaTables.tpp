/**
 * Copyright - Sebastien Pierard
 * Modified by Benjamin Laugraud in 2016
 */
#ifndef    _SUMMED_AREA_TABLES_TPP_
#error "SummedAreaTables.h must be included instead of SummedAreaTables.tpp"
#else

/* ========================================================================== *
 * SummedAreaTables                                                           *
 * ========================================================================== */

template <typename T>
SummedAreaTables<T>::SummedAreaTables(const cv::Mat& mat) :
w(mat.cols),
h(mat.rows),
sum(new T[mat.cols * mat.rows]) {
  if (w == 0) throw std::logic_error("Image with width == 0 are not supported");
  if (h == 0) throw std::logic_error("Image with height == 0 are not supported");

  const T* buffer = reinterpret_cast<const T*>(mat.data);

  for (int row = 0; row < h; ++row) {
    for (int col = 0; col < w; ++col) {
      sum [ row * w + col ] =
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
  if (row < 0) return T();
  if (col < 0) return T();

  return sum[std::min(row, h - 1) * w + std::min(col, w - 1)];
}

/******************************************************************************/

template <typename T>
T SummedAreaTables<T>::getIntegral(
  int min_row, int max_row, int min_col, int max_col
) const {
  if (min_row > max_row) return T();
  if (min_col > max_col) return T();

  return
    getIntegral(max_row    , max_col    ) -
    getIntegral(min_row - 1, max_col    ) -
    getIntegral(max_row    , min_col - 1) +
    getIntegral(min_row - 1, min_col - 1) ;
}

#endif /* _SUMMED_AREA_TABLES_TPP_ */
