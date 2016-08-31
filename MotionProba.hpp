#pragma once

#include <algorithm>
#include <stdexcept>

#include <opencv2/opencv.hpp>

#include "SummedAreaTables.h"

/* ========================================================================== *
 * CounterMotionProba                                                         *
 * ========================================================================== */

/*
 * Efficient version using using F. Crown summed area tables.
 */
class CounterMotionProba {
  public:

    typedef int32_t                                           ProbaMapEncoding;

  protected:

    int size;

  public:

    CounterMotionProba(int size) : size(size) {}

    void compute(cv::Mat& inputProbaMap, cv::Mat& outputProbaMap) const {
      int half = size / 2;

      if (half == 0)
        throw std::runtime_error("Size divided by 2 is zero!");

      ProbaMapEncoding* outputBuffer =
        reinterpret_cast<ProbaMapEncoding*>(outputProbaMap.data);

      SummedAreaTables<ProbaMapEncoding> sums(inputProbaMap);

      for (int y = 0; y < inputProbaMap.rows; ++y) {
        for (int x = 0; x < inputProbaMap.cols; ++x, ++outputBuffer) {
          /* Computing kernel ROI. */
          int xROI = std::max(x - half, 0);
          int yROI = std::max(y - half, 0);
          int widthROI = std::min(x + half, (inputProbaMap.cols - 1)) - std::max(x - half, 0) + 1;
          int heightROI = std::min(y + half, (inputProbaMap.rows - 1)) - std::max(y - half, 0) + 1;

          *outputBuffer = sums.getIntegral(yROI, yROI + heightROI - 1, xROI, xROI + widthROI - 1);
        }
      }
    }

    int getOpenCVEncoding() const {
      return CV_32SC1;
    }
};
