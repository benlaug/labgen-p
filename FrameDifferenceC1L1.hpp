#pragma once

#include <opencv2/opencv.hpp>

/* ========================================================================== *
 * FrameDifferenceC1L1                                                        *
 * ========================================================================== */

class FrameDifferenceC1L1 {
  private:

    cv::Mat img_input_prev;

  public:

    FrameDifferenceC1L1() {}

    ~FrameDifferenceC1L1() {}

    void process(const cv::Mat& img_input, cv::Mat& proba_map);
};
