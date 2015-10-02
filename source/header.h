#pragma once
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <stdexcept>
#include <vector>

using namespace std;

const string PROGRAM_NAME = "BNetToPrime";
const string VERSION = "1.0";

// Logic types
typedef char Value; ///< Three values - 0 for false, 1 for true, -1 for don't care
typedef vector<Value> Minterm; ///< A vector of values
typedef vector<Minterm> DNF; ///< A set of minterms representing a dnf

extern bool has_fin;
extern bool has_fout;
#define IF_HAS_FOUT(x) if (has_fout){ x } 
#define IF_HAS_FIN(x) if (has_fin){ x } 

#define REMOVE_LAST \
	if (has_fout) { \
		out.seekp(out.tellp() - static_cast<streampos>(1)); \
	} \
	else { \
		out << "\b \b"; \
	}

#define WHOLE(Container) Container.begin(), Container.end()
