/**
 * Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2016
 * http://www.montefiore.ulg.ac.be/~blaugraud
 * http://www.telecom.ulg.ac.be/research/sbg
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
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/program_options.hpp>

#include <opencv2/opencv.hpp>

#include <labgen-p/FrameDifferenceC1L1.hpp>
#include <labgen-p/History.hpp>
#include <labgen-p/MotionProba.hpp>
#include <labgen-p/Utils.hpp>

using namespace cv;
using namespace std;
using namespace boost;
using namespace boost::program_options;

/******************************************************************************
 * Main program                                                               *
 ******************************************************************************/

int main(int argc, char** argv) {
  /***************************************************************************
   * Argument(s) handling.                                                   *
   ***************************************************************************/

  options_description optDesc(
    string("LaBGen-P - Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2016\n") +
    "http://www.montefiore.ulg.ac.be/~blaugraud\n"                                  +
    "http://www.telecom.ulg.ac.be/research/sbg\n\n"                                 +
    "Usage: LaBGen-P [options]"
  );

  optDesc.add_options()
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

  variables_map varsMap;
  store(parse_command_line(argc, argv, optDesc), varsMap);
  notify(varsMap);

  /* Help message. */
  if (varsMap.count("help")) {
    cout << optDesc << endl;
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

  int32_t sParam = 0;
  int32_t nParam = 0;

  /* "input" */
  if (!varsMap.count("input"))
    throw runtime_error("You must provide the path of the input sequence!");

  string sequence(varsMap["input"].as<string>());

  /* "output" */
  if (!varsMap.count("output"))
    throw runtime_error("You must provide the path of the output folder!");

  string output(varsMap["output"].as<string>());

  /* "default" */
  bool defaultSet = varsMap.count("default");

  if (defaultSet) {
    sParam = 19;
    nParam = 3;
  }

  /* Other parameters. */
  if (!defaultSet) {
    /* "s-parameter" */
    if (!varsMap.count("s-parameter"))
      throw runtime_error("You must provide the S parameter!");

    sParam = varsMap["s-parameter"].as<int32_t>();

    if (sParam < 1)
      throw runtime_error("The S parameter must be positive!");

    /* "n-parameter" */
    if (!varsMap.count("n-parameter"))
      throw runtime_error("You must provide the N parameter!");

    nParam = varsMap["n-parameter"].as<int32_t>();

    if (nParam < 1)
      throw runtime_error("The N parameter must be positive!");
  }

  /* "visualization" */
  bool visualization = varsMap.count("visualization");

  /* Display parameters to the user. */
  cout << "Input sequence: "      << sequence      << endl;
  cout << "   Output path: "      << output        << endl;
  cout << "             S: "      << sParam        << endl;
  cout << "             N: "      << nParam      << endl;
  cout << " Visualization: "      << visualization << endl;
  cout << endl;

  /***************************************************************************
   * Reading sequence.                                                       *
   ***************************************************************************/

  VideoCapture decoder(sequence);

  if (!decoder.isOpened())
    throw runtime_error("Cannot open the '" + sequence + "' sequence.");

  int32_t height     = decoder.get(CV_CAP_PROP_FRAME_HEIGHT);
  int32_t width      = decoder.get(CV_CAP_PROP_FRAME_WIDTH);

  cout << "Reading sequence " << sequence << "..." << endl;

  cout << "          height: " << height     << endl;
  cout << "           width: " << width      << endl;

  typedef vector<Mat>                                            FramesVec;
  vector<Mat> frames;
  frames.reserve(decoder.get(CV_CAP_PROP_FRAME_COUNT));

  Mat frame;

  while (decoder.read(frame))
    frames.push_back(frame.clone());

  decoder.release();
  cout << frames.size() << " frames read." << endl << endl;

  /***************************************************************************
   * Processing.                                                             *
   ***************************************************************************/

  cout << "Start processing..." << endl;

  /* Initialization of the background matrix. */
  Mat background = Mat(height, width, CV_8UC3);

  /* Initialization of the ROIs. */
  Utils::ROIs rois = Utils::getROIs(height, width); // Pixel-level.

  /* Initialization of the filter. */
  CounterMotionProba filter((min(height, width) / nParam) | 1);
  cout << "Size of the kernel: " << ((min(height, width) / nParam) | 1) << endl;

  /* Initialization of the maps matrices. */
  Mat motionScores;
  Mat quantitiesMotion;

  motionScores = Mat(height, width, CV_32SC1);
  quantitiesMotion = Mat(height, width, filter.getOpenCVEncoding());

  /* Initialization of the history structure. */
  std::shared_ptr<PatchesHistory> history = std::make_shared<PatchesHistory>(rois, sParam);

  /* Misc initializations. */
  std::shared_ptr<FrameDifferenceC1L1> fdiff;
  bool firstFrame = true;
  int numFrame = -1;

  /* Processing loop. */
  cout << endl << "Processing...";

  for (FramesVec::const_iterator it = frames.begin(), end = frames.end(); it != end; ++it) {
    ++numFrame;

    /* Algorithm instantiation. */
    if (firstFrame)
      fdiff = make_shared<FrameDifferenceC1L1>();

    /* Background subtraction. */
    fdiff->process((*it).clone(), motionScores);

    /* Visualization of the input frame and its probability map. */
    if (visualization)
      imshow("Input video", (*it));

    /* Skipping first frame. */
    if (firstFrame) {
      cout << "Skipping first frame..." << endl;

      ++it;
      firstFrame = false;

      continue;
    }

    /* Filtering probability map. */
    if (!motionScores.empty()) {
      filter.compute(motionScores, quantitiesMotion);

      if (visualization)
        imshow("Quantities of motion", quantitiesMotion);
    }

    /* Insert the current frame and its probability map into the history. */
    history->insert(quantitiesMotion, (*it));

    if (visualization) {
      history->median(background, sParam);

      imshow("Estimated background", background);
      cvWaitKey(1);
    }
  }

  /* Compute background and write it. */
  stringstream outputFile;
  outputFile << output << "/output_" << sParam << "_" << nParam << ".png";

  /* Compute background and write it. */
  history->median(background, sParam);

  cout << "Writing " << outputFile.str() << "..." << endl;
  imwrite(outputFile.str(), background);

  /* Cleaning. */
  if (visualization) {
    cout << endl << "Press any key to quit..." << endl;
    cvWaitKey(0);
    cvDestroyAllWindows();
  }

  /* Bye. */
  return EXIT_SUCCESS;
}
