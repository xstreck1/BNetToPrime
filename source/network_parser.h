#pragma once

#include "header.h"

class NetworkParser {
public:
	// @return	true iff ch is possibly part of a valid name
	static char belongsToName(const char ch) {
		return isalnum(ch) || ch == '_' || ch == '-';
	}

	// Obtain unique, ordered names of the regulators from the formula
	static vector<string> getAllRegulators(const string & formula) {
		vector<string> result;
		int start_pos = -1;
		for (int i = 0; i < formula.size(); i++) {
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
		vector<string>::iterator end = unique(WHOLE(result));
		result.resize(distance(begin(result), end));
		if (count(WHOLE(result), "1")) {
			result.erase(find(WHOLE(result), "1"));
		}if (count(WHOLE(result), "0")) {
			result.erase(find(WHOLE(result), "0"));
		}
		return result;
	}
};