#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief	Class to enumerate prime implicants of a set of minterms.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "header.h"

class ImplicantEnumerator {
	//
	static inline size_t countOnes(const Minterm & minterm) {
		return count(WHOLE(minterm), 1);
	}

	// @return	unique index smaller than 2^|minterm| describing the position of the don't cares
	static size_t countDontCares(const Minterm & minterm) {
		size_t result = 0;
		for (size_t i = 0; i < minterm.size(); i++) {
			result <<= 1;
			if (minterm[i] == -1) {
				result++;
			}
		}
		return result;
	}

	// @return	position where to merge if possible, otherwise -1
	static int canMerge(const Minterm & A, const Minterm & B) {
		int diff = -1;
		for (size_t i = 0; i < A.size(); i++) {
			if (A[i] != B[i]) {
				if (diff != -1 || A[i] == -1 || B[i] == -1) {
					return -1;
				}
				else {
					diff = i;
				}
			}
		}
		return diff;
	}

	/***
	 * @brief	merges minterms by adding exactly one don't care. Unmerged are stored in the result.
	 * var_count[in]	the number of variables that are regulators
	 * depth[in]	current merge depts (number of already existing don't cares)
	 * to_merge[in,out]	minterms that are merged, after execution holds the new minterms
	 * result[out]	holds all the minterms that could not be merged
	 */
	static void nextLevel(const size_t var_count, const size_t depth, DNF & to_merge, DNF & result) {
		// Distribute the source values into an array indexed as [position of don't cares][number of ones]
		vector<vector<DNF> >  distribution(static_cast<int>(pow(2,var_count)), vector<DNF>(var_count - depth +1));
		cout << "\r\t\tMerge level: " << depth + 1 << ". Analyzing " << to_merge.size() << " minterms" << endl;
		for (DNF::const_iterator it = to_merge.cbegin(); it != to_merge.cend(); it++) {
			cout << "\rPreprocessing: " << distance(to_merge.cbegin(), it) << "/" << to_merge.size();
			distribution[countDontCares(*it)][countOnes(*it)].push_back(*it);
		}

		DNF used; // Which elements have been used for a merde
		DNF merged; // New, merged elements
		for (vector<vector<DNF> >::const_iterator dc_level_it = distribution.cbegin(); dc_level_it < distribution.cend(); dc_level_it++) {
			for (vector<DNF>::const_iterator one_level_it = dc_level_it->cbegin(); one_level_it + 1 < dc_level_it->cend(); one_level_it++) {
				cout << "\rMerging: " << distance(distribution.cbegin(), dc_level_it) << "/" << distribution.size();
				vector<DNF>::const_iterator one_upper_level_it = one_level_it + 1;
				for (DNF::const_iterator lower_minterm_it = one_level_it->cbegin(); lower_minterm_it < one_level_it->cend(); lower_minterm_it++) {
					for (DNF::const_iterator upper_minterm_it = one_upper_level_it->cbegin(); upper_minterm_it < one_upper_level_it->cend(); upper_minterm_it++) {
						int can_merge = canMerge(*lower_minterm_it, *upper_minterm_it);
						if (can_merge != -1) {
							merged.push_back(*lower_minterm_it);
							merged.back()[can_merge] = -1;
							used.push_back(*lower_minterm_it);
							used.push_back(*upper_minterm_it);
						}
					}
				}
			}
		}

		sort(WHOLE(used));
		set_difference(WHOLE(to_merge), WHOLE(used), back_inserter(result));
		sort(WHOLE(merged));
		DNF::iterator new_end = unique(WHOLE(merged));
		to_merge.resize(distance(merged.begin(), new_end));
		copy(merged.begin(), new_end, to_merge.begin());
	}

public:
	/* Create representation of the vector with don't cares. */
	static void compactize(const size_t var_count, DNF&  to_merge) {
		DNF result;
		// Make compact
		for (size_t depth = 0; !to_merge.empty(); depth++) {
			nextLevel(var_count, depth, to_merge, result);
		}
		to_merge = result;
	}
};