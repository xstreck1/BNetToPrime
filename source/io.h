#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	Functions that help with input / output.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "header.h"

class IO {
public:
	static void printDNF(const DNF & dnf, const vector<string> & regulators, fstream & fout) {
		fout << "[";
		for (DNF::const_iterator it = dnf.begin(); it != dnf.end(); it++) {
			fout << "{";
			if (count(it->begin(), it->end(), -1) != it->size()) {
				for (size_t i = 0; i < regulators.size(); i++) {
					// If there are not both the values
					if ((*it)[i] != -1) {
						// Write the number (ASCII position 48 for 0, 49 for 1)
						fout << "\"" << regulators[i] << "\":" << (*it)[i] + 0 << ",";
					}
				}
				fout.seekp(fout.tellp() - static_cast<streampos>(1));
			}
			fout << "},";
		}
		if (dnf.size() != 0) {
			fout.seekp(fout.tellp() - static_cast<streampos>(1)); // Remove the last comma
		}
		fout << "]";
	}

	static void printHelp() {

	}

	// @return	true iff ch is possibly part of a valid name
	static char belongsToName(const char ch) {
		return isalnum(ch) || ch == '_' || ch == '-';
	}

	// Obtain unique, ordered names of the regulators from the formula, ingore the values 1 and 0
	static vector<string> getAllRegulators(const string & formula) {
		vector<string> result;
		int start_pos = -1;
		for (int i = 0; i < static_cast<int>(formula.size()); i++) {
			if (start_pos == -1 && belongsToName(formula[i])) {
				start_pos = i;
			}
			else if (start_pos != -1 && !belongsToName(formula[i])) {
				result.push_back(formula.substr(start_pos, i - start_pos));
				start_pos = -1;
			}
		}
		if (start_pos != -1) {
			result.push_back(formula.substr(start_pos, formula.size() - start_pos));
		}
		sort(WHOLE(result));
		vector<string>::iterator new_end = unique(WHOLE(result));
		result.resize(distance(result.begin(), new_end));
		
		// Remove 1 and 0 (true, false) values
		if (count(WHOLE(result), "1")) {
			result.erase(find(WHOLE(result), "1"));
		}
		if (count(WHOLE(result), "0")) {
			result.erase(find(WHOLE(result), "0"));
		}
		return result;
	}
};