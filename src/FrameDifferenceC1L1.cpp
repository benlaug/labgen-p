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
#include <cmath>
#include <cstdlib>

#include <opencv2/imgproc/imgproc.hpp>

#include <labgen-p/FrameDifferenceC1L1.hpp>

using namespace std;
using namespace cv;
using namespace ns_labgen_p::ns_internals;

/* ========================================================================== *
 * FrameDifferenceC1L1                                                        *
 * ========================================================================== */

void FrameDifferenceC1L1::compute(const Mat& current_frame, Mat& motion_map) {
  if(current_frame.empty())
    return;

  Mat converted_input;

  if (current_frame.channels() != 1)
    cvtColor(current_frame, converted_input, CV_BGR2GRAY);
  else
    converted_input = current_frame;

  if (previous_frame.empty()) {
    converted_input.copyTo(previous_frame);
    return;
  }

  const unsigned char* current_buffer  = converted_input.data;
  const unsigned char* previous_buffer = previous_frame.data;

  int32_t* motion_map_buffer =
    reinterpret_cast<int32_t*>(motion_map.data);

  for (int i = 0; i < current_frame.total(); ++i) {
    *(motion_map_buffer++) = abs(
      static_cast<int32_t>(*(current_buffer++)) - *(previous_buffer++)
    );
  }

  converted_input.copyTo(previous_frame);
}
