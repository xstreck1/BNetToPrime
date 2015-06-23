#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Class able to resolve any logical function in propositional logic.
///
/// This is a static helper class able of resolving any preposition logic formula.
/// Formula construction:
///   -# \f$tt\f$ (true) and \f$ff\f$ (false) are formulas representing true and false respectively,
///	-# any variable is a formula,
///   -# for \f$\varphi\f$ formula is \f$!\varphi\f$ formula,
///   -# for \f$\psi, \varphi\f$ formulas are \f$(\psi|\varphi)\f$, \f$(\psi\&\varphi)\f$ formulas representing logical disjunction and conjunction respectively,
///   -# nothing else is a formula.
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "network_parser.h"

#define MY_TEST(expr, vals, res) if(FormulaeResolver::resolve(vals, expr)!=res){throw runtime_error(string(expr) + " does not yield the expected value\n");};

class FormulaeResolver {
public:
	typedef map<string, bool> Vals; ///< Valuation of atomic propositions
	typedef pair<string, bool> Val; ///< A single proposition valuation

	// @return	true iff the formula is true under the valuation (where the valuation are pairs (variable,value)) 
	static bool resolve(const Vals & valuation, string formula) {
		int start_pos = -1;
		int parity_count = 0;
		char current_op = '*';
		vector<string> subexpressions;
		for (int i = 0; i < formula.size(); i++) {
			if (formula[i] == ')') {
				parity_count--;
			}
			else if (formula[i] == '(') {
				parity_count++;
				if (start_pos == -1) {
					start_pos = i;
				}
			}
			else if (parity_count == 0) {
				if (start_pos == -1) {
					if (NetworkParser::belongsToName(formula[i]) || formula[i] == '!') {
						start_pos = i;
					}
				}
				else if (!(NetworkParser::belongsToName(formula[i]) || formula[i] == '!')) {
					subexpressions.push_back(formula.substr(start_pos, i - start_pos));
					start_pos = -1;
					if (formula[i] != current_op && current_op != '*') {
						throw runtime_error(string("Mismatched or uknown operator '") + formula[i] + "' in the (sub)expression '" + formula + "'.");
					}
					else {
						current_op = formula[i];
					}
				}
			}
		}
		if (start_pos != -1) {
			subexpressions.push_back(formula.substr(start_pos, formula.size() - start_pos));
		}
		if (parity_count != 0) {
			throw runtime_error(string("Wrong parenthesis parity in the (sub)expression '") + formula + "'.");
		}
		if (current_op == '*') {
			bool negate = false;
			bool result = false;
			if (formula[0] == '!') {
				formula = formula.substr(1);
				negate = true;
			}
			if (formula[0] == '(') {
				result = resolve(valuation, formula.substr(1, formula.size() - 2));
			}
			else {
				if (formula == "1") {
					result = true;
				}
				else if (formula == "0") {
					result = false;
				}
				else if (valuation.count(formula) == 0) {
					throw runtime_error(string("Variable '") + formula + "' not found in the interpretation.");
				}
				else {
					result = valuation.at(formula);
				}
			}
			return negate ? !result : result;
		}
		else if (current_op == '|') {
			bool result = false;
			for (vector<string>::iterator it = begin(subexpressions); it != end(subexpressions); it++) {
				if (it->front() == '!') {
					result |= !resolve(valuation, it->substr(1));
				}
				else {
					result |= resolve(valuation, *it);
				}
			}
			return result;
		}
		else if (current_op == '&') {
			bool result = true;
			for (vector<string>::iterator it = begin(subexpressions); it != end(subexpressions); it++) {
				if (it->front() == '!') {
					result &= !resolve(valuation, it->substr(1));
				}
				else {
					result &= resolve(valuation, *it);
				}
			}
			return result;
		}
		else {
			throw runtime_error (string("Unknown operator '") + current_op + "' in the (sub)expression '" + formula + "'.");
		}
	}

	// @return	new string made from the source by removing the leading and trailing white spaces
	static string trim(const string & source) {
		static const string WHITESPACES = " \n\r\t\v\f";
		const size_t front = source.find_first_not_of(WHITESPACES);
		return source.substr(front, source.find_last_not_of(WHITESPACES) - front + 1);
	}

	// @return new string made from the source by removing whitespaces
	static string removeWhitespaces(const string & source) {
		string result;
		for (int i = 0; i < source.size(); i++) {
			if (!isspace(source.at(i))) {
				result += source.at(i);
			}
		}
		return result;
	}

	static void test() {
		map<string, bool> vals;
		vals.insert(make_pair("A", true));
		vals.insert(make_pair("B", false));
		MY_TEST("A",vals,true);
		MY_TEST("A|B", vals, true);
		MY_TEST("!(A&B)", vals, true);
		MY_TEST("(!B&A)", vals, true);
		MY_TEST("(A&B&B&A)|A|B", vals, true);
		MY_TEST(removeWhitespaces("!(\n(A&B ) \t | (B&A)|!(A|B))"), vals, true);
	}
};