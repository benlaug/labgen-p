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

#include <labgen-p/QuantitiesMotion.hpp>

using namespace std;
using namespace cv;
using namespace labgen_p;

/* ========================================================================== *
 * QuantitiesMotion                                                           *
 * ========================================================================== */

QuantitiesMotion::QuantitiesMotion(int size) : size(size) {}

/******************************************************************************/

void QuantitiesMotion::compute(
  Mat& motion_map, Mat& quantities_of_motion
) const {
  int half = size / 2;

  if (half == 0)
    throw logic_error("Size divided by 2 is zero!");

  QuantitiesMotionEncoding* output_buffer =
    reinterpret_cast<QuantitiesMotionEncoding*>(quantities_of_motion.data);

  SummedAreaTables<QuantitiesMotionEncoding> sums(motion_map);

  for (int y = 0; y < motion_map.rows; ++y) {
    for (int x = 0; x < motion_map.cols; ++x, ++output_buffer) {
      /* Computing kernel ROI. */
      int x_roi = max(x - half, 0);
      int y_roi = max(y - half, 0);
      int width_roi =
        min(x + half, (motion_map.cols - 1)) - max(x - half, 0) + 1;
      int height_roi =
        min(y + half, (motion_map.rows - 1)) - max(y - half, 0) + 1;

      *output_buffer = sums.getIntegral(
        y_roi, y_roi + height_roi - 1, x_roi, x_roi + width_roi - 1
      );
    }
  }
}

/******************************************************************************/

int QuantitiesMotion::getOpenCVEncoding() const {
  return CV_32SC1;
}
