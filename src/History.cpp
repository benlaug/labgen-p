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
#include <algorithm>

#include <labgen-p/History.hpp>

using namespace std;
using namespace cv;
using namespace ns_labgen_p::ns_internals;

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

History::History(size_t buffer_size) : history(), buffer_size(buffer_size) {
  history.reserve(buffer_size + 1);
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

void History::insert(
  const int32_t* quantities_of_motion, const unsigned char* current_frame
) {
  const int32_t* qt_buffer = quantities_of_motion;
  int32_t positives = *(qt_buffer);

  if (history.empty())
    history.push_back(HistoryMat(current_frame, positives));
  else {
    bool inserted = false;

    for (
      HistoryVec::iterator it = history.begin(), end = history.end();
      it != end;
      ++it
    ) {
      if (positives <= (*it)) {
        history.insert(it, HistoryMat(current_frame, positives));
        inserted = true;

        if (history.size() > buffer_size)
          history.erase(history.end() - 1);

        break;
      }
    }

    if ((history.size() < buffer_size) && !inserted)
      history.push_back(HistoryMat(current_frame, positives));
  }
}

/******************************************************************************/

void History::median(unsigned char* result, size_t size) const {
  if (history.size() == 1 || size == 1) {
    result[0] = history[0][0];
    result[1] = history[0][1];
    result[2] = history[0][2];
  }

  static vector<unsigned char> buffer_r(buffer_size);
  static vector<unsigned char> buffer_g(buffer_size);
  static vector<unsigned char> buffer_b(buffer_size);

  size_t _size = min(history.size(), size);

  for (size_t num = 0; num < _size; ++num) {
    buffer_r[num] = history[num][0];
    buffer_g[num] = history[num][1];
    buffer_b[num] = history[num][2];
  }

  size_t middle = _size / 2;

  if (_size & 1) {
    nth_element(buffer_r.begin(), buffer_r.begin() + middle, buffer_r.begin() + _size);
    nth_element(buffer_g.begin(), buffer_g.begin() + middle, buffer_g.begin() + _size);
    nth_element(buffer_b.begin(), buffer_b.begin() + middle, buffer_b.begin() + _size);

    result[0] = buffer_r[middle];
    result[1] = buffer_g[middle];
    result[2] = buffer_b[middle];
  }
  else {
    nth_element(buffer_r.begin(), buffer_r.begin() + (middle - 1), buffer_r.begin() + _size);
    nth_element(buffer_g.begin(), buffer_g.begin() + (middle - 1), buffer_g.begin() + _size);
    nth_element(buffer_b.begin(), buffer_b.begin() + (middle - 1), buffer_b.begin() + _size);

    nth_element(buffer_r.begin() + middle, buffer_r.begin() + middle, buffer_r.begin() + _size);
    nth_element(buffer_g.begin() + middle, buffer_g.begin() + middle, buffer_g.begin() + _size);
    nth_element(buffer_b.begin() + middle, buffer_b.begin() + middle, buffer_b.begin() + _size);

    result[0] = ((static_cast<int32_t>(buffer_r[middle - 1])) + (buffer_r[middle])) / 2;
    result[1] = ((static_cast<int32_t>(buffer_g[middle - 1])) + (buffer_g[middle])) / 2;
    result[2] = ((static_cast<int32_t>(buffer_b[middle - 1])) + (buffer_b[middle])) / 2;
  }
}

/******************************************************************************/

bool History::empty() const {
  return history.empty();
}

/* ========================================================================== *
 * PatchesHistory                                                             *
 * ========================================================================== */

PatchesHistory::PatchesHistory(const Utils::ROIs& rois, size_t buffer_size) :
p_history(), rois(rois) {
  p_history.reserve(rois.size());

  for (size_t i = 0; i < rois.size(); ++i)
    p_history.push_back(History(buffer_size));
}

/******************************************************************************/

void PatchesHistory::insert(
  const Mat& quantities_of_motion, const Mat& current_frame
) {
  int32_t* qt_buffer = reinterpret_cast<int32_t*>(quantities_of_motion.data);
  unsigned char* current_buffer = current_frame.data;

  for (int i = 0, j = 0; i < current_frame.total(); ++i, j += 3)
    p_history[i].insert(qt_buffer + i, current_buffer + j);
}

/******************************************************************************/

void PatchesHistory::median(Mat& result, size_t size) const {
  unsigned char* result_buffer = result.data;

  for (size_t i = 0, j = 0; i < rois.size(); ++i, j += 3)
    p_history[i].median(result_buffer + j, size);
}

/******************************************************************************/

bool PatchesHistory::empty() const {
 for (History h : p_history) {
   if (h.empty())
     return true;
 }

 return false;
}
