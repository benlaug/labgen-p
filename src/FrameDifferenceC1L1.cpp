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
#include <cmath>
#include <cstdlib>

#include <opencv2/imgproc/imgproc.hpp>

#include <labgen-p/FrameDifferenceC1L1.hpp>

using namespace std;
using namespace cv;
using namespace labgen_p;

/* ========================================================================== *
 * FrameDifferenceC1L1                                                        *
 * ========================================================================== */

void FrameDifferenceC1L1::compute(const Mat& image, Mat& motion_map) {
  if(image.empty())
    return;

  Mat cinput;

  if (image.channels() != 1)
    cvtColor(image, cinput, CV_BGR2GRAY);
  else
    cinput = image;

  if (previous_image.empty()) {
    cinput.copyTo(previous_image);
    return;
  }

  const unsigned char* image_buffer      = cinput.data;
  const unsigned char* previous_buffer   = previous_image.data;

  int32_t* motion_map_buffer =
    reinterpret_cast<int32_t*>(motion_map.data);

  for (int i = 0; i < image.total(); ++i) {
    *(motion_map_buffer++) = abs(
      static_cast<int32_t>(*(image_buffer++)) - *(previous_buffer++)
    );
  }

  cinput.copyTo(previous_image);
}
