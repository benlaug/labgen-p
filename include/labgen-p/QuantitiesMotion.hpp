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

#include <cstdint>

#include <opencv2/core/core.hpp>

#include "SummedAreaTables.hpp"

namespace labgen_p {
  /* ======================================================================== *
   * QuantitiesMotion                                                         *
   * ======================================================================== */

  class QuantitiesMotion {
    protected:

      typedef int32_t                                 QuantitiesMotionEncoding;

    protected:

      int size;

    public:

      QuantitiesMotion(int size);

      void compute(cv::Mat& motion_map, cv::Mat& quantities_of_motion) const;

      int getOpenCVEncoding() const;
  };
} /* _NS_labgen_p_ */
