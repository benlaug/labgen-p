/**
 * Copyright - Benjamin Laugraud - 2016
 */
#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <cv.h>

/* ========================================================================== *
 * Utils                                                                      *
 * ========================================================================== */

struct Utils {
  typedef std::vector<cv::Rect>                                           ROIs;

  /****************************************************************************/

  static std::string getMethod(std::string method) {
    size_t pos = method.rfind("/");
    pos = (pos == std::string::npos) ? 0 : pos + 1;

    return method.substr(pos, method.rfind(".") - pos);
  }

  /****************************************************************************/

  /*
   * If the modulo X of a dimension divided by the number of segments is
   * superior to 0, then it is distributed among the X first patches.
   */
  static ROIs getROIs(size_t height, size_t width, size_t segments) {
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

    for (size_t i = 0; i < segments; ++i) {
      size_t wReminderTmp = wReminder;
      size_t previousX = 0;
      size_t previousWidth = 0;

      for (size_t j = 0; j < segments; ++j) {
        rois.push_back(
          cv::Rect(
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

  /****************************************************************************/

  /*
   * Pixel level.
   */
  static ROIs getROIs(size_t height, size_t width) {
    ROIs rois;
    rois.reserve(height * width);

    for (size_t i = 0; i < height; ++i) {
      for (size_t j = 0; j < width; ++j)
        rois.push_back(cv::Rect(j, i, 1, 1));
    }

    return rois;
  }
};
