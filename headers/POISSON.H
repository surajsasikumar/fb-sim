
#ifndef POISSON_H
#define POISSON_H

#include <iostream>
#include <iomanip>
#include <cmath>
#include <unistd.h>
using namespace std;

// Safe factorial for 0..5
inline int fact(int x) {
	if (x <= 1) return 1;
	return x * fact(x - 1);
}

// Pick one goal count (0..5) based on largest Poisson probabilities,
// then randomize among the top 3 candidates for a bit of variety.
inline int goalCalc(float k[2][6], int x) {
	int picks[3] = {0, 0, 0};
	float tmp[6];
	for (int i = 0; i < 6; ++i) tmp[i] = k[x][i];

	// pick top 3
	for (int n = 0; n < 3; ++n) {
		int argmax = 0;
		for (int i = 1; i < 6; ++i) {
			if (tmp[i] > tmp[argmax]) argmax = i;
		}
		picks[n] = argmax;
		tmp[argmax] = -1.0f; // remove
	}

	int n = rand() % 3;
	return picks[n];
}

// Core Poisson — returns goals for team1 and team2 in a[0], a[1]
inline void poisson(float as1, float as2, float ds1, float ds2, float avg, int a[2]) {
	// λ = attack_strength * opponent_defense * league_avg_goals
	double lam1 = as1 * ds2 * avg;
	double lam2 = as2 * ds1 * avg;

	// clamp to a reasonable range to avoid over/underflows
	lam1 = max(0.05, min(5.0, lam1));
	lam2 = max(0.05, min(5.0, lam2));

	float k[2][6];
	for (int i = 0; i < 6; ++i) {
		k[0][i] = float(100.0 * (exp(-lam1) * pow(lam1, i) / fact(i)));
		k[1][i] = float(100.0 * (exp(-lam2) * pow(lam2, i) / fact(i)));
	}

	a[0] = goalCalc(k, 0);
	a[1] = goalCalc(k, 1);
}

#endif