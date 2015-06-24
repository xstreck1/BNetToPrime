#include "formulae_resolver.h"
#include "implicant_enumerator.h"



// @brief	The number of the regulators is bounded by the nubmer of bits in the size_t 
size_t maxRegulatorsCount() {
	return sizeof(size_t) * 8;
}

// @brief	from the id obtain the valation of the respective variables (e.g. for id==4 and variables=={A,B,C} we get (A=1,B=0,C=0), for id==1 we get (A=0,B=0,C=1))
Minterm valuationToVals(const size_t valuation_id, map<string, bool> & valuation) {
	Minterm result(valuation.size(), 0);
	int ele = 0;
	for (map<string, bool>::iterator it = valuation.begin(); it != valuation.end(); it++, ele++) {
		bool val = ((valuation_id >> (valuation.size() - 1 - ele)) % 2) == 1;
		result[ele] = val;
		it->second = val;
	}
	return result;
}

//
int main(int argc, char ** argv) {
	try {
#ifdef _MSC_VER //Set the output buffer size for visual studio
		setvbuf(stdout, 0, _IOLBF, 4096);
#endif
		// Parse the input
		string input_filename = "input.bnet";
		string output_filename = "output.json";
		if (argc > 1) {
			input_filename = argv[1];
			if (input_filename == "--help" || input_filename == "-h") {
				IO::printHelp();
				return 0;
			}
			if (argc > 2) {
				output_filename = argv[2];
			}
		}

		// Open the input and output files
        fstream fin(input_filename.c_str(), fstream::in);
		if (!fin) {
			throw invalid_argument(string("Failed to open the input file ") + input_filename);
		}
        fstream fout(output_filename.c_str(), fstream::out);
		if (!fout) {
			throw invalid_argument(string("Failed to open the output file ") + output_filename);
		}
		fout << "{";

		// Holds the input
		map<string, pair<vector<string>, string> > line_data;
		string line;
		getline(fin, line); // Ignore the first line
		// Read computed and write line by line
		while (getline(fin, line)) {
			// Parse the line
			size_t comma_pos = line.find(',');
			string component = line.substr(0, comma_pos);
			string formula = FormulaeResolver::removeWhitespaces(line.substr(comma_pos + 1));
			vector<string> regulators = IO::getAllRegulators(formula);
			line_data.insert(make_pair(component, make_pair(regulators, formula)));
		}
		fin.close();

		// Check if all components are present, if not, add self-regulation
		for (map<string, pair<vector<string>, string> >::iterator it = line_data.begin(); it != line_data.end(); it++) {
            for (vector<string>::const_iterator comp_it = it->second.first.begin(); comp_it != it->second.first.end(); comp_it++) {
				if (line_data.count(*comp_it) == 0) {
					vector<string> regulators;
					regulators.push_back(*comp_it);
					line_data.insert(make_pair(*comp_it, make_pair(regulators, *comp_it)));
				}
			}
		}

		// Compute and output data for each line
        for (map<string, pair<vector<string>, string> >::const_iterator it = line_data.begin(); it != line_data.end(); it++) {
			const string component = it->first;
			const vector<string> regulators = it->second.first;
			const string formula = it->second.second;

			const size_t VALUATIONS_COUNT = static_cast<size_t>(pow(2, regulators.size())); // How many valuations of the variables there are
			if (regulators.size() > maxRegulatorsCount()) {
				throw runtime_error("The component '" + component + "' has too many regulators.");
			}

			// Create the valuation map, also write the current line on the output 
			map<string, bool> valuation;
			cout << "\rTarget: '" << component << "'. Regulators: [";
            for (vector<string>::const_iterator it = regulators.begin(); it != regulators.end(); it++) {
				cout << *it << ",";
				valuation.insert(make_pair(*it, false));
			} 
			cout << "]" << endl;

			//  Resolve the formulas and push the values to the respective vectors
			DNF true_elems;
			DNF false_elems;
			for (size_t valuation_id = 0; valuation_id < VALUATIONS_COUNT; valuation_id++) {
				Minterm new_elem = valuationToVals(valuation_id, valuation);
				if (FormulaeResolver::resolve(valuation, formula)) {
					true_elems.push_back(new_elem);
				}
				else {
					false_elems.push_back(new_elem);
				}
				cout << "\rSolving formula: " << valuation_id + 1 << "/" << VALUATIONS_COUNT;
			}

			// Compactize and write the output immediatelly
			fout << "\"" << component << "\":[";
			cout << "\r\tComputing implicants for: '!(" << formula << ")'\n";
			ImplicantEnumerator::compactize(regulators.size(), false_elems);
			IO::printDNF(false_elems, regulators, fout);
			false_elems.clear();
			fout << ",";
			cout << "\r\tComputing implicants for: '" << formula << "'\n";
			ImplicantEnumerator::compactize(regulators.size(), true_elems);
			IO::printDNF(true_elems, regulators, fout);
			true_elems.clear();
			fout << "],";
		}

		fout.seekp(fout.tellp() - static_cast<streampos>(1)); // Remove the last comma
		fout << "}";
	}
	catch (exception & e) {
		cerr << PROGRAM_NAME << " encountered an exception and aborted." << endl;
		cerr << "Exception message: \"" << e.what() << "\"" << endl;
		return 1;
	}

	return 0;
}
