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
#include <algorithm>

#include <labgen-p/History.hpp>

using namespace std;
using namespace cv;
using namespace labgen_p;

/* ========================================================================== *
 * HistoryMat                                                                 *
 * ========================================================================== */

HistoryMat::HistoryMat(const unsigned char* mat, const uint32_t positives) :
positives(positives) {
  this->mat[0] = mat[0];
  this->mat[1] = mat[1];
  this->mat[2] = mat[2];
}

/******************************************************************************/

HistoryMat::HistoryMat(const HistoryMat& copy) :
positives(copy.positives) {
  mat[0] = copy.mat[0];
  mat[1] = copy.mat[1];
  mat[2] = copy.mat[2];
}

/******************************************************************************/

HistoryMat& HistoryMat::operator=(const HistoryMat& copy) {
  if (this != &copy) {
    mat[0] = copy.mat[0];
    mat[1] = copy.mat[1];
    mat[2] = copy.mat[2];

    positives = copy.positives;
  }

  return *this;
}

/******************************************************************************/

uint8_t HistoryMat::operator[](size_t i) const {
  return mat[i];
}

/* ========================================================================== *
 * History                                                                    *
 * ========================================================================== */

History::History(size_t bufferSize) : history(), bufferSize(bufferSize) {
  history.reserve(bufferSize + 1);
}

/******************************************************************************/

History::HistoryVec& History::operator*() {
  return history;
}

/******************************************************************************/

const History::HistoryVec& History::operator*() const {
  return history;
}

/******************************************************************************/

void History::insert(const int32_t* probabilityMap, const unsigned char* frame) {
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

/******************************************************************************/

void History::median(unsigned char* result, size_t size) const {
  if (history.size() == 1 || size == 1) {
    result[0] = history[0][0];
    result[1] = history[0][1];
    result[2] = history[0][2];
  }

  vector<unsigned char> bufferR(bufferSize);
  vector<unsigned char> bufferG(bufferSize);
  vector<unsigned char> bufferB(bufferSize);

  size_t _size = min(history.size(), size);

  for (size_t num = 0; num < _size; ++num) {
    bufferR[num] = history[num][0];
    bufferG[num] = history[num][1];
    bufferB[num] = history[num][2];
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

/* ========================================================================== *
 * PatchesHistory                                                             *
 * ========================================================================== */

PatchesHistory::PatchesHistory(const Utils::ROIs& rois, size_t bufferSize) :
pHistory(), rois(rois) {
  pHistory.reserve(rois.size());

  for (size_t i = 0; i < rois.size(); ++i)
    pHistory.push_back(History(bufferSize));
}

/******************************************************************************/

PatchesHistory::PatchesHistory(const Utils::ROIs& rois, vector<size_t> bufferSize) :
pHistory(), rois(rois) {
  pHistory.reserve(rois.size());

  for (size_t i = 0; i < rois.size(); ++i)
    pHistory.push_back(History(bufferSize[i]));
}

/******************************************************************************/

void PatchesHistory::insert(const Mat& probabilityMap, const Mat& frame) {
  int32_t* probaData = reinterpret_cast<int32_t*>(probabilityMap.data);
  unsigned char* frameData = frame.data;

  for (int i = 0, j = 0; i < frame.rows * frame.cols; ++i, j += 3) {
    pHistory[i].insert(
      probaData + i,
      frameData + j
    );
  }
}

/******************************************************************************/

void PatchesHistory::median(Mat& result, size_t size) const {
  unsigned char* data = result.data;

  for (size_t i = 0, j = 0; i < rois.size(); ++i, j += 3)
    pHistory[i].median(data + j, size);
}
