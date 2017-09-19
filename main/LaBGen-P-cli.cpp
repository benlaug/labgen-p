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
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <labgen-p/ArgumentsHandler.hpp>
#include <labgen-p/LaBGen_P.hpp>
#include <labgen-p/GridWindow.hpp>
#include <labgen-p/TextProperties.hpp>
#include <labgen-p/Utils.hpp>

using namespace cv;
using namespace std;
using namespace ns_labgen_p;

/******************************************************************************
 * Main program                                                               *
 ******************************************************************************/

int main(int argc, char** argv) {
  /****************************************************************************
   * Argument(s) handling.                                                    *
   ****************************************************************************/

  /*
   * Extract parameters and sanity check.
   */

  ArgumentsHandler args_h(argc, argv);

  if (args_h.ask_for_help()) {
    args_h.print_help();
    return EXIT_SUCCESS;
  }

  /*
   * Welcome message.
   */

  cout << "===========================================================" << endl;
  cout << "= LaBGen-P                                                =" << endl;
  cout << "===========================================================" << endl;
  cout << "= Copyright - Benjamin Laugraud - 2017                    =" << endl;
  cout << "= http://www.montefiore.ulg.ac.be/~blaugraud              =" << endl;
  cout << "= http://www.telecom.ulg.ac.be/labgen                     =" << endl;
  cout << "===========================================================" << endl;
  cout << endl;

  args_h.parse_vars_map();
  args_h.print_parameters();

  /****************************************************************************
   * Reading sequence.                                                        *
   ****************************************************************************/

  VideoCapture decoder(args_h.get_input());

  if (!decoder.isOpened()) {
    throw runtime_error(
      "Cannot open the '" + args_h.get_input() + "' sequence."
    );
  }

  int32_t height = decoder.get(CV_CAP_PROP_FRAME_HEIGHT);
  int32_t width  = decoder.get(CV_CAP_PROP_FRAME_WIDTH);

  cout << "Reading sequence " << args_h.get_input() << "..." << endl;

  cout << "           height: " << height     << endl;
  cout << "            width: " << width      << endl;

  typedef vector<Mat>                                                FramesVec;
  vector<Mat> frames;
  frames.reserve(decoder.get(CV_CAP_PROP_FRAME_COUNT));

  Mat frame;

  while (decoder.read(frame))
    frames.push_back(frame.clone());

  decoder.release();
  cout << frames.size() << " frames read." << endl << endl;

  /****************************************************************************
   * Initialization of graphical components and video streams.                *
   ****************************************************************************/

  unique_ptr<Mat> motion_map_8u;
  unique_ptr<Mat> normalized_qom;

  unique_ptr<GridWindow> window;
  unique_ptr<VideoWriter> record_stream;

  if (args_h.get_visualization() || args_h.get_record()) {
    motion_map_8u  = unique_ptr<Mat>(new Mat(height, width, CV_8UC1));
    normalized_qom = unique_ptr<Mat>(new Mat(height, width, CV_8UC1));

    if (!args_h.get_split_vis()) {
      TextProperties::TextPropertiesPtr title_properties = nullptr;

      if (args_h.get_record()) {
        title_properties = make_shared<TextProperties>(
          TextProperties::Font::FONT_DUPLEX,
          0.8
        );
      }
      else
        title_properties = make_shared<TextProperties>();

      window = unique_ptr<GridWindow>(
        new GridWindow(
          "LaBGen-P",
          (args_h.get_v_height() > 0) ? args_h.get_v_height() : height,
          (args_h.get_v_width() > 0) ? args_h.get_v_width() : width,
          2,
          2,
          title_properties
        )
      );

      if (args_h.get_keep_ratio())
        window->keep_ratio();

      if (args_h.get_record()) {
        const Mat& buffer = window->get_buffer();

        record_stream = unique_ptr<VideoWriter>(
          new VideoWriter(
            args_h.get_record_path(),
            CV_FOURCC('M','J','P','G'),
            args_h.get_record_fps(),
            Size(buffer.cols, buffer.rows)
          )
        );
      }
    }
  }

  /****************************************************************************
   * Processing.                                                              *
   ****************************************************************************/

  cout << "Start processing..." << endl;

  /* Initialization of the background matrix. */
  Mat background = Mat(height, width, CV_8UC3);

  /* Initialization of the LaBGen-P algorithm. */
  LaBGen_P labgen_p(height, width, args_h.get_s_param(), args_h.get_n_param());

  /* Processing loop. */
  cout << endl << "Processing..." << endl;
  bool first_frame = true;

  for (auto it = frames.begin(), end = frames.end(); it != end; ++it) {
    labgen_p.insert(*it);

    /* Skipping first frame. */
    if (first_frame) {
      cout << "Skipping first frame..." << endl;

      ++it;
      first_frame = false;

      continue;
    }

    /* Visualization. */
    if (args_h.get_visualization() || args_h.get_record()) {
      labgen_p.generate_background(background);
      labgen_p.get_motion_map().convertTo(*motion_map_8u, CV_8U);

      Utils::normalize_mat(
        labgen_p.get_quantities_of_motion(),
        *normalized_qom,
        255.
      );

      if (args_h.get_split_vis()) {
        imshow("Input video", *it);
        imshow("LaBGen-P", background);
        imshow("Motion map", *motion_map_8u);
        imshow("Quantities of motion", *normalized_qom);
      }
      else {
        window->display(*it, 0);
        window->put_title("Input video", 0);

        window->display(background, 1);
        window->put_title("Background estimated by LaBGen-P", 1);

        window->display(*motion_map_8u, 2);
        window->put_title("Motion map", 2);

        window->display(*normalized_qom, 3);
        window->put_title("Quantities of motion", 3);

        if (args_h.get_visualization())
          window->refresh();

        if (args_h.get_record())
          *record_stream << window->get_buffer();
      }

      if (args_h.get_visualization())
        waitKey(args_h.get_wait());
    }
  }

  /* Compute background and write it. */
  stringstream output_file;
  output_file << args_h.get_output() << "/output_"
              << args_h.get_s_param() << "_"
              << args_h.get_n_param() << ".png";

  labgen_p.generate_background(background);

  cout << "Writing " << output_file.str() << "..." << endl;
  imwrite(output_file.str(), background);

  /* Cleaning. */
  if (args_h.get_visualization()) {
    //cout << endl << "Press any key in a graphical window to quit..." << endl;
    //waitKey(0);
    destroyAllWindows();

    if (args_h.get_record())
      record_stream->release();
  }

  /* Bye. */
  return EXIT_SUCCESS;
}
