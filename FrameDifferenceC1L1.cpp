#include "FrameDifferenceC1L1.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

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
