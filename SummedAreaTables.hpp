/**
 * Copyright - Sebastien Pierard
 * Modified by Benjamin Laugraud in 2016
 */
#pragma once

#include <algorithm>
#include <stdexcept>

#include <opencv2/opencv.hpp>

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
