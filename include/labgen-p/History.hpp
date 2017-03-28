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

#include <cstddef>
#include <cstdint>
#include <vector>

#include <opencv2/core/core.hpp>

#include "Utils.hpp"

namespace ns_labgen_p {
  namespace ns_internals {
    /* ====================================================================== *
     * HistoryMat                                                             *
     * ====================================================================== */

    class HistoryMat {
        friend bool operator< (const HistoryMat& lhs, const HistoryMat& rhs);
        friend bool operator<=(const HistoryMat& lhs, const HistoryMat& rhs);
        friend bool operator==(const HistoryMat& lhs, const HistoryMat& rhs);
        friend bool operator< (const HistoryMat& lhs, const uint32_t&   rhs);
        friend bool operator<=(const HistoryMat& lhs, const uint32_t&   rhs);
        friend bool operator==(const HistoryMat& lhs, const uint32_t&   rhs);
        friend bool operator< (const uint32_t&   lhs, const HistoryMat& rhs);
        friend bool operator<=(const uint32_t&   lhs, const HistoryMat& rhs);
        friend bool operator==(const uint32_t&   lhs, const HistoryMat& rhs);

      protected:

        uint8_t mat[3];
        uint32_t positives;

      public:

        HistoryMat(const unsigned char* mat, const uint32_t positives);

        HistoryMat(const HistoryMat& copy);

        HistoryMat& operator=(const HistoryMat& copy);

        uint8_t operator[](size_t i) const;
    };

    /* ====================================================================== *
     * History                                                                *
     * ====================================================================== */

    class History {
      public:

        typedef std::vector<HistoryMat>                             HistoryVec;

      protected:

        HistoryVec history;
        size_t buffer_size;

      public:

        explicit History(size_t buffer_size);

        HistoryVec& operator*();

        const HistoryVec& operator*() const;

        void insert(
          const int32_t* quantities_of_motion,
          const unsigned char* current_frame
        );

        void median(unsigned char* result, size_t size = ~0) const;

        bool empty() const;
    };

    /* ====================================================================== *
     * PatchesHistory                                                         *
     * ====================================================================== */

    class PatchesHistory {
      protected:

        typedef std::vector<History>                         PatchesHistoryVec;

      protected:

        PatchesHistoryVec p_history;
        Utils::ROIs rois;

      public:

        PatchesHistory(const Utils::ROIs& rois, size_t buffer_size);

        void insert(
          const cv::Mat& quantities_of_motion, const cv::Mat& current_frame
        );

        void median(cv::Mat& result, size_t size = ~0) const;

        bool empty() const;
    };

#define _NS_LABGEN_P_NS_INTERNALS_HISTORY_IPP_
#include "History.ipp"
#undef  _NS_LABGEN_P_NS_INTERNALS_HISTORY_IPP_
  } /* ns_internals */
} /* ns_labgen_p */
