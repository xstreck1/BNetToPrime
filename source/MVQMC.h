#pragma once

#include "header.h"

// Post Algebra types
typedef int PVal;
typedef vector<PVal> PLit; ///< Post's algebra literal
typedef vector<PLit> PMin ; ///< Post's algebra minterm
typedef vector<PMin> PDNF; ///< Post's algebra disjunctive normal form

class MVQMC {
	/* Return a vector of combined values, if possible, otherwise return an empty vector. */
	static PMin combine(PMin & a, PMin & b) {
		PMin result;

		vector<bool> different(a.size());
		transform(WHOLE(a), begin(b), different.begin(), not_equal_to<vector<int> >()); // Get true if the vectors of values are equal
		const size_t diff_count = count(WHOLE(different), true);
		if (diff_count == 0) {
			throw runtime_error("Trying to merge duplicated vectors.");
		}
		if (diff_count == 1) {
			result = a;
			size_t diff_position = distance(different.begin(), find(WHOLE(different), true));
			set_difference(WHOLE(b[diff_position]), WHOLE(a[diff_position]), inserter(result[diff_position], result[diff_position].end()));
			sort(WHOLE(result[diff_position]));
		}

		return result;
	}

	/* Return a vector of combinations of the next order. Those who were not possible to merge are left in the reference to input. */
	static vector<PMin> next_merge(vector<PMin> & original) {
		vector<PMin> compacted;
		vector<bool> success(original.size(), false);

		for (vector<PMin>::iterator it1 = original.begin(); it1 != original.end(); it1++) {
			for (vector<PMin>::iterator it2 = it1 + 1; it2 != original.end(); it2++) {
				PMin combined = combine(*it1, *it2);
				if (!combined.empty()) {
					if (find(WHOLE(compacted), (combined)) == compacted.end())
						compacted.emplace_back(combined);
					success[distance(original.begin(), it1)] = true;
					success[distance(original.begin(), it2)] = true;
				}
			}
		}

		// Replace the original vector with the leftovers
		vector<PMin> leftovers;
		for (size_t i = 0; i < original.size(); i++)
			if (!success[i])
				leftovers.emplace_back(original[i]);
		original.resize(leftovers.size());
		copy(WHOLE(leftovers), original.begin());

		return compacted;
	}

public:
	static PDNF elementsToPDNF(vector<vector<int> > original) {
		PDNF result;
		result.reserve(original.size());

		size_t counter = 0;
		for (vector<vector<int> >::iterator ele_it = begin(original); ele_it != end(original); ele_it++) {
			cout << "\rConverting to PDNF: " << ++counter << "/" << original.size();
			PMin new_minterm;
			new_minterm.reserve(ele_it->size());

			for (vector<int>::iterator val_it = begin(*ele_it); val_it != end(*ele_it); val_it++) {
				PLit new_lit(1, *val_it);
				new_minterm.push_back(new_lit);
			}

			result.push_back(new_minterm);
		}

		return result;
	}

	/* Create representation of the vector with don't cares. */
	static PDNF compactize(PDNF current) {
		PDNF result;

		// Make compact
		PDNF compacted;
		int depth = 0;
		do {
			cout << "\rCompaction depth: " << ++depth << ", size of sample: " << current.size() << flush;
			compacted = next_merge(current);
			copy(WHOLE(current), inserter(result, result.end()));
			current = compacted;
		} while (!compacted.empty());

		return result;
	}
};