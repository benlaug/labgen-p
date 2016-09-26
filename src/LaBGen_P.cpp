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
#include <stdexcept>

#include <labgen-p/LaBGen_P.hpp>
#include <labgen-p/Utils.hpp>

using namespace std;
using namespace cv;
using namespace ns_labgen_p;
using namespace ns_labgen_p::ns_internals;

/* ========================================================================== *
 * LaBGen_P                                                                   *
 * ========================================================================== */

LaBGen_P::LaBGen_P(size_t height, size_t width, int32_t s, int32_t n) :
height(height),
width(width),
s(s),
n(n),
motion_map(Mat(height, width, CV_32SC1)),
filter((min(height, width) / n) | 1),
history(Utils::getROIs(height, width), s),
first_frame(true) {
  quantities_of_motion = Mat(height, width, filter.getOpenCVEncoding());
}

/******************************************************************************/

void LaBGen_P::insert(const cv::Mat& current_frame) {
  /* Motion map computation by frame difference. */
  f_diff.compute(current_frame, motion_map);

  /* Initialization of background subtraction. */
  if (first_frame) {
    first_frame = false;
    return;
  }

  /* Filtering motion map to produce quantities of motion. */
  filter.compute(motion_map, quantities_of_motion);

  /* Insert the current frame along with the quantities of motion into the
   * history.
   */
  history.insert(quantities_of_motion, current_frame);
}

/******************************************************************************/

void LaBGen_P::generate_background(cv::Mat& background) const {
  if (history.empty()) {
    throw runtime_error(
      "Cannot generate the background with less than two inserted frames"
    );
  }

  history.median(background, s);
}

/******************************************************************************/

size_t LaBGen_P::get_height() const {
  return height;
}

/******************************************************************************/

size_t LaBGen_P::get_width() const {
  return width;
}

/******************************************************************************/

int32_t LaBGen_P::get_s() const {
  return s;
}

/******************************************************************************/

int32_t LaBGen_P::get_n() const {
  return n;
}

/******************************************************************************/

const Mat& LaBGen_P::get_motion_map() const {
  return motion_map;
}

/******************************************************************************/

const Mat& LaBGen_P::get_quantities_of_motion() const {
  return quantities_of_motion;
}
