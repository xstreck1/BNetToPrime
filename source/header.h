#pragma once
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

using namespace std;

const string PROGRAM_NAME = "PrimeImplicantEnumerator";

// Logic types
typedef char Value; ///< Three values - 0 for false, 1 for true, -1 for don't care
typedef vector<Value> Minterm; ///< A vector of values
typedef vector<Minterm> DNF; ///< A set of minterms representing a dnf

#define WHOLE(Container) Container.begin(), Container.end()
