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
 * along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <algorithm>
#include <cmath>
#include <cstdlib>

#include <labgen-p/FrameDifferenceC1L1.hpp>

/* ========================================================================== *
 * FrameDifferenceC1L1                                                        *
 * ========================================================================== */

void FrameDifferenceC1L1::process(const cv::Mat& img_input, cv::Mat& proba_map) {
  if(img_input.empty())
    return;

  cv::Mat cinput;

  if (img_input.channels() != 1)
    cv::cvtColor(img_input, cinput, CV_BGR2GRAY);
  else
    cinput = img_input;

  if(img_input_prev.empty()) {
    cinput.copyTo(img_input_prev);
    return;
  }

  const unsigned char* input      = cinput.data;
  const unsigned char* input_prev = img_input_prev.data;
              int32_t* proba      = reinterpret_cast<int32_t*>(proba_map.data);

  for (int i = 0; i < img_input.rows * img_input.cols; ++i)
    *(proba++) = abs((int32_t)(*(input++)) - *(input_prev++));

  cinput.copyTo(img_input_prev);
}
