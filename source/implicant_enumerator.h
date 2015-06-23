#pragma once

#include "header.h"

// Post Algebra types
typedef char Value; ///< Three values - 0 for false, 1 for true, -1 for don't care
typedef vector<Value> Minterm ; ///< A vector of values
typedef vector<Minterm> DNF; ///< A set of minterms representing a dnf

class ImplicantEnumerator {
	static inline size_t countOnes(const Minterm & minterm) {
		return count(WHOLE(minterm), 1);
	}

	static inline size_t countDontCares(const Minterm & minterm) {
		return count(WHOLE(minterm), -1);
	}

	// @return	position where to merge if possible, otherwise -1
	static int canMerge(const Minterm & A, const Minterm & B) {
		int diff = -1;
		for (int i = 0; i < A.size(); i++) {
			if (A[i] != B[i]) {
				if (A[i] == -1 || B[i] == -1) {
					return -1;
				}
				else if (diff != -1) {
					return -1;
				}
				else {
					diff = i;
				}
			}
		}
		return diff;
	}

	static void nextLevel(const size_t var_count, const size_t depth, DNF & source, DNF & result) {
		vector<vector<DNF> >  distribution(depth+1, vector<DNF>(var_count - depth +1));
		DNF used;
		DNF merged;
		for (DNF::iterator it = begin(source); it != end(source); it++) {
			distribution[countDontCares(*it)][countOnes(*it)].push_back(*it);
		}

		for (vector<vector<DNF> >::const_iterator dc_level_it = begin(distribution); dc_level_it < end(distribution); dc_level_it++) {
			for (vector<DNF>::const_iterator one_level_it = begin(*dc_level_it); one_level_it + 1 < end(*dc_level_it); one_level_it++) {
				vector<DNF>::const_iterator one_upper_level_it = one_level_it + 1;
				for (DNF::const_iterator lower_minterm_it = begin(*one_level_it); lower_minterm_it < end(*one_level_it); lower_minterm_it++) {
					for (DNF::const_iterator upper_minterm_it = begin(*one_upper_level_it); upper_minterm_it < end(*one_upper_level_it); upper_minterm_it++) {
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
		set_difference(WHOLE(source), WHOLE(used), back_inserter(result));
		sort(WHOLE(merged));
		DNF::iterator end = unique(WHOLE(merged));
		source.resize(distance(begin(merged), end));
		copy(begin(merged), end, begin(source));
	}

public:
	/* Create representation of the vector with don't cares. */
	static DNF compactize(const size_t var_count, DNF merged) {
		DNF result;

		// Make compact
		for (int depth = 0; depth < var_count && !merged.empty(); depth++) {
			cout << "\rCompaction depth: " << depth << ", size of sample: " << merged.size() << flush;
			nextLevel(var_count, depth, merged, result);
		}

		return result;
	}
};