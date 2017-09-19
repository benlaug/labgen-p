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
#include <stdexcept>
#include <vector>

#include <boost/lexical_cast.hpp>

#include <labgen-p/ArgumentsHandler.hpp>

using namespace std;
using namespace boost;
using namespace boost::program_options;
using namespace ns_labgen_p;

/* ========================================================================== *
 * ArgumentsHandler                                                           *
 * ========================================================================== */

ArgumentsHandler::ArgumentsHandler(int argc, char** argv) :
opt_desc(
  "LaBGen-P - Copyright - Benjamin Laugraud <blaugraud@ulg.ac.be> - 2017\n"
  "http://www.montefiore.ulg.ac.be/~blaugraud\n"
  "http://www.telecom.ulg.ac.be/labgen\n\n"
  "Usage: ./LaBGen-P-cli [options]"
) {
  define_options();
  store(parse_command_line(argc, argv, opt_desc), vars_map);
  notify(vars_map);
}

/******************************************************************************/

bool ArgumentsHandler::ask_for_help() const {
  return vars_map.count("help");
}

/******************************************************************************/

void ArgumentsHandler::print_help(ostream& os) const {
  os << opt_desc << endl;
}

/******************************************************************************/

void ArgumentsHandler::parse_vars_map() {
  parse_input();
  parse_output();
  parse_default_params();
  parse_s_param();
  parse_n_param();
  parse_visualization();
  parse_split_vis();
  parse_record();
  parse_v_height();
  parse_v_width();
  parse_keep_ratio();
  parse_wait();
}

/******************************************************************************/

const string& ArgumentsHandler::get_input() const {
  return input;
}

/******************************************************************************/

const string& ArgumentsHandler::get_output() const {
  return output;
}

/******************************************************************************/

int32_t ArgumentsHandler::get_s_param() const {
  return s_param;
}

/******************************************************************************/

int32_t ArgumentsHandler::get_n_param() const {
  return n_param;
}

/******************************************************************************/

bool ArgumentsHandler::get_visualization() const {
  return visualization;
}

/******************************************************************************/

bool ArgumentsHandler::get_split_vis() const {
  return split_vis;
}

/******************************************************************************/

bool ArgumentsHandler::get_record() const {
  return record;
}

/******************************************************************************/

const string& ArgumentsHandler::get_record_path() const {
  return record_path;
}

/******************************************************************************/

int32_t ArgumentsHandler::get_record_fps() const {
  return record_fps;
}

/******************************************************************************/

int32_t ArgumentsHandler::get_v_height() const {
  return v_height;
}

/******************************************************************************/

int32_t ArgumentsHandler::get_v_width() const {
  return v_width;
}

/******************************************************************************/

bool ArgumentsHandler::get_keep_ratio() const {
  return keep_ratio;
}

/******************************************************************************/

int32_t ArgumentsHandler::get_wait() const {
  return wait;
}

/******************************************************************************/

void ArgumentsHandler::print_parameters(ostream& os) const {
  os << "   Input sequence: "      << input         << endl;
  os << "      Output path: "      << output        << endl;
  os << "                S: "      << s_param       << endl;
  os << "                N: "      << n_param       << endl;
  os << "    Visualization: "      << visualization << endl;
  if (visualization)
  os << "        Split vis: "      << split_vis     << endl;
  if (v_height > 0)
  os << "       Vis height: "      << v_height      << endl;
  if (v_width > 0)
  os << "        Vis width: "      << v_width       << endl;
  if (visualization)
  os << "Keep aspect ratio: "      << keep_ratio    << endl;
  os << "           Record: "      << record        << endl;
  if (record) {
  os << "      Record path: "      << record_path   << endl;
  os << "       Record fps: "      << record_fps    << endl;
  }
  if (visualization)
  os << "        Wait (ms): "      << wait          << endl;
  os << endl;
}

/******************************************************************************/

void ArgumentsHandler::define_options() {
  opt_desc.add_options()
    (
      "help",
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
    (
      "split-vis,l",
      "split the visualization items in separated windows"
    )
    (
      "height,h",
      value<int32_t>(),
      "height used to display an image to visualize"
    )
    (
      "width,w",
      value<int32_t>(),
      "width used to display an image to visualize"
    )
    (
      "keep-ratio,k",
      "keep aspect ratio of an image to visualize"
    )
    (
      "record,r",
      value<vector<string>>()->multitoken(),
      "record visualization in a video file by giving its path"
    )
    (
      "wait,t",
      value<int32_t>()->default_value(1),
      "time to wait (in ms) between the processing of two frames with "
      "visualization"
    )
  ;
}

/******************************************************************************/

void ArgumentsHandler::parse_input() {
  if (!vars_map.count("input"))
    throw logic_error("You must provide the path of the input sequence!");

  input = vars_map["input"].as<string>();
}

/******************************************************************************/

void ArgumentsHandler::parse_output() {
  if (!vars_map.count("output"))
    throw logic_error("You must provide the path of the output folder!");

  output = vars_map["output"].as<string>();
}

/******************************************************************************/

void ArgumentsHandler::parse_default_params() {
  default_set = vars_map.count("default");

  if (default_set) {
    s_param = 19;
    n_param = 3;
  }
}

/******************************************************************************/

void ArgumentsHandler::parse_s_param() {
  if (!default_set) {
    if (!vars_map.count("s-parameter"))
      throw logic_error("You must provide the S parameter!");

    s_param = vars_map["s-parameter"].as<int32_t>();

    if (s_param < 1)
      throw logic_error("The S parameter must be positive!");
  }
}

/******************************************************************************/

void ArgumentsHandler::parse_n_param() {
  if (!default_set) {
    if (!vars_map.count("n-parameter"))
      throw logic_error("You must provide the N parameter!");

    n_param = vars_map["n-parameter"].as<int32_t>();

    if (n_param < 1)
      throw logic_error("The N parameter must be positive!");
  }
}

/******************************************************************************/

void ArgumentsHandler::parse_visualization() {
  visualization = vars_map.count("visualization");
}

/******************************************************************************/

void ArgumentsHandler::parse_split_vis() {
  split_vis = vars_map.count("split-vis");

  if (split_vis && !visualization) {
    cerr << "/!\\ The split-vis option without visualization will be ignored!";
    cerr << endl << endl;

    split_vis = false;
  }
}

/******************************************************************************/

void ArgumentsHandler::parse_record() {
  record = vars_map.count("record");

  record_path = "";
  record_fps = 15;

  if (record) {
    if (split_vis) {
      cerr << "/!\\ The record option with split windows will be ignored!";
      cerr << endl << endl;

      record = false;
    }
    else {
      vector<string> record_args = vars_map["record"].as<vector<string>>();

      if ((record_args.size() < 1) || (record_args.size() > 2)) {
        throw logic_error(
          "One or two arguments must be provided with record: <path> [<fps>]"
        );
      }

      record_path = record_args[0];

      if (record_path.empty())
        throw logic_error("The record path cannot be empty!");

      if (record_args.size() > 1) {
        try {
          record_fps = lexical_cast<int32_t>(record_args[1]);
        }
        catch (bad_lexical_cast& e) {
          throw logic_error(
            "The number of fps for recording the video is not an integer!"
          );
        }

        if (record_fps < 1) {
          throw logic_error(
            "The number of fps for recording the video must be positive!"
          );
        }
      }
    }
  }
}

/******************************************************************************/

void ArgumentsHandler::parse_v_height() {
  v_height = 0;

  if (vars_map.count("height")) {
    if (!visualization && !record) {
      cerr << "/!\\ The height option without visualization or record will be ";
      cerr << "ignored!";
      cerr << endl << endl;
    }
    else if (split_vis) {
      cerr << "/!\\ The height option with split windows will be ignored!";
      cerr << endl << endl;
    }
    else {
      v_height = vars_map["height"].as<int32_t>();

      if (v_height < 1)
        throw logic_error("The height parameter must be positive!");
    }
  }
}

/******************************************************************************/

void ArgumentsHandler::parse_v_width() {
  v_width = 0;

  if (vars_map.count("width")) {
    if (!visualization && !record) {
      cerr << "/!\\ The width option without visualization or record will be ";
      cerr << "ignored!";
      cerr << endl << endl;
    }
    else if (split_vis) {
      cerr << "/!\\ The width option with split windows will be ignored!";
      cerr << endl << endl;
    }
    else {
      v_width = vars_map["width"].as<int32_t>();

      if (v_width < 1)
        throw logic_error("The width parameter must be positive!");
    }
  }
}

/******************************************************************************/

void ArgumentsHandler::parse_keep_ratio() {
  keep_ratio = vars_map.count("keep-ratio");

  if (keep_ratio) {
    if (!visualization && !record) {
      cerr << "/!\\ The keep-ratio option without visualization or record ";
      cerr << "will be ignored!";
      cerr << endl << endl;

      keep_ratio = false;
    }
    else if (split_vis) {
      cerr << "/!\\ The keep-ratio option with split windows will be ignored!";
      cerr << endl << endl;

      keep_ratio = false;
    }
    else if ((v_height < 1) || (v_width < 1)) {
      cerr << "/!\\ The keep-ratio option with no height or width defined ";
      cerr << "will be ignored!";
      cerr << endl << endl;

      keep_ratio = false;
    }
  }
}

/******************************************************************************/

void ArgumentsHandler::parse_wait() {
  wait = vars_map["wait"].as<int32_t>();

  if ((wait != 1) && !visualization) {
    cerr << "/!\\ The wait option without visualization will be ignored!";
    cerr << endl << endl;
  }

  if ((wait < 0) && visualization) {
    throw logic_error(
      "The wait parameter must be positive!"
    );
  }
}
