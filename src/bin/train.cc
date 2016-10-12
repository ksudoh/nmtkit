#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <nmtkit/exception.h>
#include <nmtkit/vocabulary.h>

using namespace std;

namespace FS = boost::filesystem;
namespace PO = boost::program_options;
namespace PT = boost::property_tree;

namespace {

PO::variables_map parseArgs(int argc, char * argv[]) {
  PO::options_description opt_generic("Generic options");
  opt_generic.add_options()
    ("help", "Print this manual and exit.")
    ("config",
     PO::value<string>(),
     "(required) Location of the training configuration file.")
    ("output",
     PO::value<string>(),
     "(required) Location of the output directory.")
    ;

  PO::options_description opt;
  opt.add(opt_generic);

  // parse
  PO::variables_map args;
  PO::store(PO::parse_command_line(argc, argv, opt), args);
  PO::notify(args);

  // print usage
  if (args.count("help")) {
    cerr << "NMTKit trainer." << endl;
    cerr << "Author: Yusuke Oda (http://github.com/odashi/)" << endl;
    cerr << "Usage:" << endl;
    cerr << "  train --config CONFIG_FILE --output OUTPUT_DIRECTORY" << endl;
    cerr << "  train --help" << endl;
    cerr << opt << endl;
    exit(1);
  }

  // check required arguments
  const vector<string> required_args = {"config", "output"};
  bool ok = true;
  for (const string & arg : required_args) {
    if (!args.count(arg)) {
      cerr << "Missing argument: --" << arg << endl;
      ok = false;
    }
  }
  NMTKIT_CHECK(
      ok,
      "Some required arguments are missing.\n"
      "(--help to show all options)");

  return args;
}

void makeDirectory(const FS::path & dirpath) {
  NMTKIT_CHECK(
      !FS::exists(dirpath),
      "Directory or file already exists: " + dirpath.string());
  NMTKIT_CHECK(
      FS::create_directories(dirpath),
      "Could not create directory: " + dirpath.string());
}

void run(int argc, char * argv[]) try {
  // parse commandline args and config file.
  const auto args = ::parseArgs(argc, argv);
  PT::ptree config;
  PT::read_ini(args["config"].as<string>(), config);

  // create output directory.
  FS::path outdir(args["output"].as<string>());
  ::makeDirectory(outdir);

  // create vocabulary.
  NMTKit::Vocabulary src_vocab(
      config.get<string>("Corpus.train_source"),
      config.get<unsigned>("Model.source_vocabulary"));
  NMTKit::Vocabulary trg_vocab(
      config.get<string>("Corpus.train_target"),
      config.get<unsigned>("Model.target_vocabulary"));
  src_vocab.save((outdir / "source.vocab").string());
  trg_vocab.save((outdir / "target.vocab").string());

} catch (exception & ex) {
  cerr << ex.what() << endl;
  exit(1);
}

}  // namespace

int main(int argc, char * argv[]) {
  ::run(argc, argv);
  return 0;
}

