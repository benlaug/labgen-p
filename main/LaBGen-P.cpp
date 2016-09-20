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
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/program_options.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <labgen-p/FrameDifferenceC1L1.hpp>
#include <labgen-p/History.hpp>
#include <labgen-p/QuantitiesMotion.hpp>
#include <labgen-p/Utils.hpp>

using namespace cv;
using namespace std;
using namespace boost;
using namespace boost::program_options;
using namespace labgen_p;

/******************************************************************************
 * Main program                                                               *
 ******************************************************************************/

int main(int argc, char** argv) {
  /****************************************************************************
   * Argument(s) handling.                                                    *
   ****************************************************************************/

  options_description opt_desc(
    "LaBGen-P - Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2016\n"
    "http://www.montefiore.ulg.ac.be/~blaugraud\n"
    "http://www.telecom.ulg.ac.be/labgen\n\n"
    "Usage: ./LaBGen-P [options]"
  );

  opt_desc.add_options()
    (
      "help,h",
      "print this help message"
    )
    (
      "input,i",
      value<string>(),
      "path to the input sequence"
    )
    (
      "output,o",
      value<string>(),
      "path to the output folder"
    )
    (
      "s-parameter,s",
      value<int32_t>()->multitoken(),
      "value of the S parameter"
    )
    (
      "n-parameter,n",
      value<int32_t>(),
      "value of the N parameter"
    )
    (
      "default,d",
      "use the default set of parameters"
    )
    (
      "visualization,v",
      "enable visualization"
    )
  ;

  variables_map vars_map;
  store(parse_command_line(argc, argv, opt_desc), vars_map);
  notify(vars_map);

  /* Help message. */
  if (vars_map.count("help")) {
    cout << opt_desc << endl;
    return EXIT_SUCCESS;
  }

  /*
   * Welcome message.
   */

  cout << "===========================================================" << endl;
  cout << "= LaBGen-P                                                =" << endl;
  cout << "===========================================================" << endl;
  cout << "= Copyright - Benjamin Laugraud - 2016                    =" << endl;
  cout << "= http://www.montefiore.ulg.ac.be/~blaugraud              =" << endl;
  cout << "= http://www.telecom.ulg.ac.be/research/sbg               =" << endl;
  cout << "===========================================================" << endl;
  cout << endl;

  /*
   * Extract parameters and sanity check.
   */

  int32_t s_param = 0;
  int32_t n_param = 0;

  /* "input" */
  if (!vars_map.count("input"))
    throw runtime_error("You must provide the path of the input sequence!");

  string sequence(vars_map["input"].as<string>());

  /* "output" */
  if (!vars_map.count("output"))
    throw runtime_error("You must provide the path of the output folder!");

  string output(vars_map["output"].as<string>());

  /* "default" */
  bool default_set = vars_map.count("default");

  if (default_set) {
    s_param = 19;
    n_param = 3;
  }

  /* Other parameters. */
  if (!default_set) {
    /* "s-parameter" */
    if (!vars_map.count("s-parameter"))
      throw runtime_error("You must provide the S parameter!");

    s_param = vars_map["s-parameter"].as<int32_t>();

    if (s_param < 1)
      throw runtime_error("The S parameter must be positive!");

    /* "n-parameter" */
    if (!vars_map.count("n-parameter"))
      throw runtime_error("You must provide the N parameter!");

    n_param = vars_map["n-parameter"].as<int32_t>();

    if (n_param < 1)
      throw runtime_error("The N parameter must be positive!");
  }

  /* "visualization" */
  bool visualization = vars_map.count("visualization");

  /* Display parameters to the user. */
  cout << "Input sequence: "      << sequence      << endl;
  cout << "   Output path: "      << output        << endl;
  cout << "             S: "      << s_param       << endl;
  cout << "             N: "      << n_param       << endl;
  cout << " Visualization: "      << visualization << endl;
  cout << endl;

  /****************************************************************************
   * Reading sequence.                                                        *
   ****************************************************************************/

  VideoCapture decoder(sequence);

  if (!decoder.isOpened())
    throw runtime_error("Cannot open the '" + sequence + "' sequence.");

  int32_t height = decoder.get(CV_CAP_PROP_FRAME_HEIGHT);
  int32_t width  = decoder.get(CV_CAP_PROP_FRAME_WIDTH);

  cout << "Reading sequence " << sequence << "..." << endl;

  cout << "          height: " << height     << endl;
  cout << "           width: " << width      << endl;

  typedef vector<Mat>                                                FramesVec;
  vector<Mat> frames;
  frames.reserve(decoder.get(CV_CAP_PROP_FRAME_COUNT));

  Mat frame;

  while (decoder.read(frame))
    frames.push_back(frame.clone());

  decoder.release();
  cout << frames.size() << " frames read." << endl << endl;

  /****************************************************************************
   * Processing.                                                              *
   ****************************************************************************/

  cout << "Start processing..." << endl;

  /* Initialization of the background matrix. */
  Mat background = Mat(height, width, CV_8UC3);

  /* Initialization of the ROIs. */
  Utils::ROIs rois = Utils::getROIs(height, width); // Pixel-level.

  /* Initialization of the filter. */
  QuantitiesMotion filter((min(height, width) / n_param) | 1);
  cout << "Size of the kernel: " << ((min(height, width) / n_param) | 1);
  cout << endl;

  /* Initialization of the maps matrices. */
  Mat motion_map;
  Mat quantities_of_motion;

  motion_map = Mat(height, width, CV_32SC1);
  quantities_of_motion = Mat(height, width, filter.getOpenCVEncoding());

  /* Initialization of the history structure. */
  std::shared_ptr<PatchesHistory> history =
    std::make_shared<PatchesHistory>(rois, s_param);

  /* Misc initializations. */
  std::shared_ptr<FrameDifferenceC1L1> f_diff;
  bool first_frame = true;

  /* Processing loop. */
  cout << endl << "Processing...";

  for (auto it = frames.begin(), end = frames.end(); it != end; ++it) {
    /* Algorithm instantiation. */
    if (first_frame)
      f_diff = std::make_shared<FrameDifferenceC1L1>();

    /* Background subtraction. */
    f_diff->compute(*it, motion_map);

    /* Visualization of the input frame. */
    if (visualization)
      imshow("Input video", *it);

    /* Skipping first frame. */
    if (first_frame) {
      cout << "Skipping first frame..." << endl;

      ++it;
      first_frame = false;

      continue;
    }

    /* Filtering motion map to produce quantities of motion. */
    if (!motion_map.empty()) {
      filter.compute(motion_map, quantities_of_motion);

      /* Visualization of the quantities of motion. */
      if (visualization)
        imshow("Quantities of motion", quantities_of_motion);
    }

    /* Insert the current frame along with the quantities of motion into the
     * history.
     */
    history->insert(quantities_of_motion, *it);

    /* Visualization of the estimated background. */
    if (visualization) {
      history->median(background, s_param);

      imshow("Estimated background", background);
      cvWaitKey(1);
    }
  }

  /* Compute background and write it. */
  stringstream output_file;
  output_file << output << "/output_" << s_param << "_" << n_param << ".png";

  /* Compute background and write it. */
  history->median(background, s_param);

  cout << "Writing " << output_file.str() << "..." << endl;
  imwrite(output_file.str(), background);

  /* Cleaning. */
  if (visualization) {
    cout << endl << "Press any key to quit..." << endl;
    cvWaitKey(0);
    cvDestroyAllWindows();
  }

  /* Bye. */
  return EXIT_SUCCESS;
}
