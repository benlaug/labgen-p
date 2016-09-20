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
#include <labgen-p/Utils.hpp>

using namespace std;
using namespace cv;
using namespace labgen_p;

/* ========================================================================== *
 * Utils                                                                      *
 * ========================================================================== */

string Utils::getMethod(string method) {
  size_t pos = method.rfind("/");
  pos = (pos == string::npos) ? 0 : pos + 1;

  return method.substr(pos, method.rfind(".") - pos);
}

/******************************************************************************/

Utils::ROIs Utils::getROIs(size_t height, size_t width, size_t segments) {
  if (segments == 0)
    return getROIs(height, width);

  ROIs rois;
  rois.reserve(segments * segments);

  size_t patchHeight = height / segments;
  size_t hReminder = height % segments;

  size_t patchWidth = width / segments;
  size_t wReminder = width % segments;

  /* Algorithm. */
  size_t hReminderTmp = hReminder;
  size_t previousY = 0;
  size_t previousHeight = 0;

  /*
   * If the modulo X of a dimension divided by the number of segments is
   * superior to 0, then it is distributed among the X first patches.
   */
  for (size_t i = 0; i < segments; ++i) {
    size_t wReminderTmp = wReminder;
    size_t previousX = 0;
    size_t previousWidth = 0;

    for (size_t j = 0; j < segments; ++j) {
      rois.push_back(
        Rect(
          (j == 0) ? 0 : (previousX + previousWidth),
          (i == 0) ? 0 : (previousY + previousHeight),
          patchWidth  + ((wReminderTmp > 0) ? 1 : 0),
          patchHeight + ((hReminderTmp > 0) ? 1 : 0)
        )
      );

      previousX = rois.back().x;
      previousWidth = rois.back().width;

      if (wReminderTmp > 0)
        --wReminderTmp;
    }

    previousY = rois.back().y;
    previousHeight = rois.back().height;

    if (hReminderTmp > 0)
      --hReminderTmp;
  }

  return rois;
}

/******************************************************************************/

Utils::ROIs Utils::getROIs(size_t height, size_t width) {
  ROIs rois;
  rois.reserve(height * width);

  for (size_t i = 0; i < height; ++i) {
    for (size_t j = 0; j < width; ++j)
      rois.push_back(Rect(j, i, 1, 1));
  }

  return rois;
}
