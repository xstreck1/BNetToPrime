#include "formulae_resolver.h"
#include "implicant_enumerator.h"

#if __cplusplus < 199711L
#define X.begin() X.begin()
#define X.end() X.end()
#endif

// The number of the regulators is bounded by the nubmer of bits in the size_t 
size_t maxRegulatorsCount() {
	return sizeof(size_t) * 8;
}

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
		fstream fin(input_filename, ios::in);
		if (!fin) {
			throw invalid_argument(string("Failed to open the input file ") + input_filename);
		}
		fstream fout(output_filename, ios::out);
		if (!fout) {
			throw invalid_argument(string("Failed to open the output file ") + output_filename);
		}
		fout << "{";

		string line;
		getline(fin, line); // Ignore the first line
		// Read computed and write line by line
		while (getline(fin, line)) {
			// Parse the line
			size_t comma_pos = line.find(',');
			string component = line.substr(0, comma_pos);
			string formula = FormulaeResolver::removeWhitespaces(line.substr(comma_pos + 1));
			vector<string> regulators = IO::getAllRegulators(formula);
			const size_t VALUATIONS_COUNT = static_cast<size_t>(pow(2, regulators.size())); // How many valuations of the variables there are
			if (regulators.size() > maxRegulatorsCount()) {
				throw runtime_error("The component '" + component + "' has too many regulators.");
			}

			// Create the valuation map, also write the current line on the output 
			map<string, bool> valuation;
			cout << "RESOLVING: " << component << ". REGULATORS: [";
			for (vector<string>::iterator it = regulators.begin(); it != regulators.end(); it++) {
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
				cout << "\rResolving formula: " << valuation_id + 1 << "/" << VALUATIONS_COUNT;
			}

			// Compactize and write the output immediatelly
			fout << "\"" << component << "\":[";
			IO::printDNF(ImplicantEnumerator::compactize(regulators.size(), false_elems), regulators, fout);
			fout << ",";
			IO::printDNF(ImplicantEnumerator::compactize(regulators.size(), true_elems), regulators, fout);
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
