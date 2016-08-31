/**
 * Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2016
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
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <vector>

#include <opencv2/opencv.hpp>

#include "Utils.hpp"

#define CHANNELS                                                              3

/* ========================================================================== *
 * HistoryMat                                                                 *
 * ========================================================================== */

struct HistoryMat {
  uint8_t mat[3];
  uint32_t positives;

  /****************************************************************************/

  HistoryMat(const unsigned char* mat, const uint32_t positives) :
  positives(positives) {
    this->mat[0] = mat[0];
    this->mat[1] = mat[1];
    this->mat[2] = mat[2];
  }

  /****************************************************************************/

  HistoryMat(const HistoryMat& copy) :
  positives(copy.positives) {
    mat[0] = copy.mat[0];
    mat[1] = copy.mat[1];
    mat[2] = copy.mat[2];
  }

  /****************************************************************************/

  HistoryMat& operator=(const HistoryMat& copy) {
    mat[0] = copy.mat[0];
    mat[1] = copy.mat[1];
    mat[2] = copy.mat[2];

    positives = copy.positives;

    return *this;
  }
};

/******************************************************************************
 * Operator(s) overloading                                                    *
 ******************************************************************************/

inline bool operator<(const HistoryMat& lhs, const HistoryMat& rhs) {
  return lhs.positives < rhs.positives;
}

/******************************************************************************/

inline bool operator<=(const HistoryMat& lhs, const HistoryMat& rhs) {
  return lhs.positives <= rhs.positives;
}

/******************************************************************************/

inline bool operator==(const HistoryMat& lhs, const HistoryMat& rhs) {
  return lhs.positives == rhs.positives;
}

/******************************************************************************/

inline bool operator<(const HistoryMat& lhs, const uint32_t& rhs) {
  return lhs.positives < rhs;
}

/******************************************************************************/

inline bool operator<=(const HistoryMat& lhs, const uint32_t& rhs) {
  return lhs.positives <= rhs;
}

/******************************************************************************/

inline bool operator==(const HistoryMat& lhs, const uint32_t& rhs) {
  return lhs.positives == rhs;
}

/******************************************************************************/

inline bool operator<(const uint32_t& lhs, const HistoryMat& rhs) {
  return lhs < rhs.positives;
}

/******************************************************************************/

inline bool operator<=(const uint32_t& lhs, const HistoryMat& rhs) {
  return lhs <= rhs.positives;
}

/******************************************************************************/

inline bool operator==(const uint32_t& lhs, const HistoryMat& rhs) {
  return lhs == rhs.positives;
}

/* ========================================================================== *
 * History                                                                    *
 * ========================================================================== */

struct History {
  typedef std::vector<HistoryMat>                                   HistoryVec;

  /****************************************************************************/

  HistoryVec history;
  size_t bufferSize;

  /****************************************************************************/

  explicit History(size_t bufferSize) : history(), bufferSize(bufferSize) {
    history.reserve(bufferSize + 1);
  }

  /****************************************************************************/

  HistoryVec& operator*() { return history; }

  /****************************************************************************/

  const HistoryVec& operator*() const { return history; }

  /****************************************************************************/

  virtual void insert(const int32_t* probabilityMap, const unsigned char* frame) {
    const int32_t* mapBuffer = probabilityMap;
    int32_t positives = *(mapBuffer);

    if (history.empty())
      history.push_back(HistoryMat(frame, positives));
    else {
      bool inserted = false;

      for (
        HistoryVec::iterator it = history.begin(), end = history.end();
        it != end;
        ++it
      ) {
        if (positives <= (*it)) {
          history.insert(it, HistoryMat(frame, positives));
          inserted = true;

          if (history.size() > bufferSize)
            history.erase(history.end() - 1);

          break;
        }
      }

      if ((history.size() < bufferSize) && !inserted)
        history.push_back(HistoryMat(frame, positives));
    }
  }

  /****************************************************************************/

  virtual void median(unsigned char* result, size_t size = ~0) const {
    if (history.size() == 1 || size == 1) {
      result[0] = history[0].mat[0];
      result[1] = history[0].mat[1];
      result[2] = history[0].mat[2];
    }

    std::vector<unsigned char> bufferR(bufferSize);
    std::vector<unsigned char> bufferG(bufferSize);
    std::vector<unsigned char> bufferB(bufferSize);

    size_t _size = std::min(history.size(), size);

    for (size_t num = 0; num < _size; ++num) {
      bufferR[num] = history[num].mat[0];
      bufferG[num] = history[num].mat[1];
      bufferB[num] = history[num].mat[2];
    }

    size_t middle = _size / 2;

    if (_size & 1) {
      nth_element(bufferR.begin(), bufferR.begin() + middle, bufferR.begin() + _size);
      nth_element(bufferG.begin(), bufferG.begin() + middle, bufferG.begin() + _size);
      nth_element(bufferB.begin(), bufferB.begin() + middle, bufferB.begin() + _size);

      result[0] = bufferR[middle];
      result[1] = bufferG[middle];
      result[2] = bufferB[middle];
    }
    else {
      nth_element(bufferR.begin(), bufferR.begin() + (middle - 1), bufferR.begin() + _size);
      nth_element(bufferG.begin(), bufferG.begin() + (middle - 1), bufferG.begin() + _size);
      nth_element(bufferB.begin(), bufferB.begin() + (middle - 1), bufferB.begin() + _size);

      nth_element(bufferR.begin() + middle, bufferR.begin() + middle, bufferR.begin() + _size);
      nth_element(bufferG.begin() + middle, bufferG.begin() + middle, bufferG.begin() + _size);
      nth_element(bufferB.begin() + middle, bufferB.begin() + middle, bufferB.begin() + _size);

      result[0] = (((int)bufferR[middle - 1]) + ((int)bufferR[middle])) / 2;
      result[1] = (((int)bufferG[middle - 1]) + ((int)bufferG[middle])) / 2;
      result[2] = (((int)bufferB[middle - 1]) + ((int)bufferB[middle])) / 2;
    }
  }
};

/* ========================================================================== *
 * PatchesHistory                                                             *
 * ========================================================================== */

struct PatchesHistory {
  typedef std::vector<History>                 PatchesHistoryVec;

  /****************************************************************************/

  PatchesHistoryVec pHistory;
  const Utils::ROIs& rois;

  /****************************************************************************/

  PatchesHistory(const Utils::ROIs& rois, size_t bufferSize) :
    pHistory(), rois(rois) {

    pHistory.reserve(rois.size());

    for (size_t i = 0; i < rois.size(); ++i)
      pHistory.push_back(History(bufferSize));
  }

  /****************************************************************************/

  PatchesHistory(const Utils::ROIs& rois, std::vector<size_t> bufferSize) :
    pHistory(), rois(rois) {

    pHistory.reserve(rois.size());

    for (size_t i = 0; i < rois.size(); ++i)
      pHistory.push_back(History(bufferSize[i]));
  }

  /****************************************************************************/

  virtual void insert(const cv::Mat& probabilityMap, const cv::Mat& frame) {
    int32_t* probaData = reinterpret_cast<int32_t*>(probabilityMap.data);
    unsigned char* frameData = frame.data;

    for (int i = 0, j = 0; i < frame.rows * frame.cols; ++i, j += 3) {
      pHistory[i].insert(
        probaData + i,
        frameData + j
      );
    }
  }

  /****************************************************************************/

  virtual void median(cv::Mat& result, size_t size = ~0) const {
    unsigned char* data = result.data;

    for (size_t i = 0, j = 0; i < rois.size(); ++i, j += 3)
      pHistory[i].median(data + j, size);
  }
};
