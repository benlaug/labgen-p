/**
 * Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2016
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
#pragma once

#include <algorithm>
#include <stdexcept>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "SummedAreaTables.hpp"

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
