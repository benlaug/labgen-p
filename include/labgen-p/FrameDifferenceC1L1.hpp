/**
 * Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2017
 * http://www.montefiore.ulg.ac.be/~blaugraud
 * http://www.telecom.ulg.ac.be/labgen
 *
 * This file is part of LaBGen-P.
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

#include <opencv2/core/core.hpp>

namespace ns_labgen_p {
  namespace ns_internals {
    /* =====================================================================  *
     * FrameDifferenceC1L1                                                    *
     * ====================================================================== */

    class FrameDifferenceC1L1 {
      private:

        cv::Mat previous_frame;

      public:

        FrameDifferenceC1L1() {}

        virtual ~FrameDifferenceC1L1() {}

        void compute(const cv::Mat& current_frame, cv::Mat& motion_map);
    };
  } /* ns_internals */
} /* ns_labgpen_p */
