#include "formulae_resolver.h"
#include "MVQMC.h"

const string PROGRAM_NAME = "PrimeImplicantEnumerator";

vector<int> valuationToVals(const size_t valuation_id, map<string, bool> & valuation) {
	vector<int> result(valuation.size(), 0);
	int ele = 0;
	for (map<string, bool>::iterator it = begin(valuation); it != end(valuation); it++, ele++) {
		bool val = (valuation_id >> ele) % 2;
		result[ele] = val;
		it->second = val;
	}
	return result;
}

Json::Value PDNFToJSON(const PDNF & pdnf, const vector<string> & regulators) {
	Json::Value result;
	result.resize(0);

	for (PDNF::const_iterator it = begin(pdnf); it != end(pdnf); it++) {
		Json::Value implicant(Json::objectValue);

		for (size_t i = 0; i < regulators.size(); i++) {
			// If there are not both the values
			if ((*it)[i].size() != 2) {
				implicant[regulators[i]] = (*it)[i].front();
			}
		}

		result.append(implicant);
	}

	return result;
}

//
int main(int argc, char ** argv) {
#ifdef _MSC_VER
	//Set the output buffer size for visual studio
	setvbuf(stdout, 0, _IOLBF, 4096);
#endif

	try {
		FormulaeResolver::test();

		string filename;
		if (argc == 1) {
			filename = "input.bnet";
		}
		else {
			filename = argv[1];
		}

		Json::Value JsonResult;

		fstream fin(filename, ios::in);
		if (!fin) {
			throw invalid_argument(string("Failed to open the input file ") + filename);
		}
		string line;
		getline(fin, line); // Ignore the first line
		while (getline(fin, line)) {
			size_t comma_pos = line.find(',');
			string component = line.substr(0, comma_pos);
			string formula = FormulaeResolver::removeWhitespaces(line.substr(comma_pos + 1));
			vector<string> regulators = NetworkParser::getAllRegulators(formula);

			map<string, bool> valuation;
			cout << component << ":";
			for (vector<string>::iterator it = begin(regulators); it != end(regulators); it++) {
				cout << *it << ",";
				valuation.insert(make_pair(*it, false));
			} cout << endl;

			vector<vector<int> > true_elems;
			vector<vector<int> > false_elems;
			const size_t VALUATIONS_COUNT = pow(2, regulators.size());
			for (size_t valuation_id = 0; valuation_id < VALUATIONS_COUNT; valuation_id++) {
				vector<int> new_elem = valuationToVals(valuation_id, valuation);
				if (FormulaeResolver::resolve(valuation, formula)) {
					true_elems.push_back(new_elem);
				}
				else {
					false_elems.push_back(new_elem);
				}
				cout << "\rResolving: " << valuation_id + 1 << "/" << VALUATIONS_COUNT;
			}

			Json::Value JsonLine;
			JsonLine["0"] = PDNFToJSON(MVQMC::compactize(MVQMC::elementsToPDNF(false_elems)), regulators);
			JsonLine["1"] = PDNFToJSON(MVQMC::compactize(MVQMC::elementsToPDNF(true_elems)), regulators);
			JsonResult[component] = JsonLine;
		}

		string output_filename = "primes.json";
		fstream fout(output_filename, ios::out);
		if (!fout)
			throw runtime_error(string("Could not open '") + output_filename + "'.");

		Json::StyledWriter writer;
		string result = writer.write(JsonResult);
		fout << result << endl;
	}
	catch (exception & e) {
		cerr << PROGRAM_NAME << " encountered an exception and aborted." << endl
			<< "Exception message: \"" << e.what() << "\"" << endl;
		cin.get();
		return 1;
	}

	return 0;
}
