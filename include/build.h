#include <string>
#include <vector>
using std::string;
using std::vector;

struct build {
    static const string compiler;
    static string standard;
    static vector<string> input;
    static vector<string> include;
    static vector<string> link_paths;
    static vector<string> link_names;
    static vector<string> flags;
    static string output;
    
    static string get_cmd() {
        string cmd = compiler;
        
        if (standard.length() > 0) {
            cmd += " -std=" + standard;
        }
        
        for (string i : input) {
            cmd += " " + i;
        }
        
        for (string i : include) {
            cmd += " -I" + i;
        }
        
        for (string l : link_paths) {
            cmd += " -L" + l;
        }
        
        for (string l : link_names) {
            cmd += " -l" + l;
        }
        
        for (string f : flags) {
            cmd += " " + f;
        }
        
        if (output.length() > 0) {
            cmd += " -o" + output;
        }
        
        return cmd;
    }
};