#include <build.h>

const string build::compiler = "g++";
string build::standard;
vector<string> build::input;
vector<string> build::include;
vector<string> build::link_paths;
vector<string> build::link_names;
vector<string> build::flags;
string build::output;