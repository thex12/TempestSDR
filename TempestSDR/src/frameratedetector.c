/*******************************************************************************
 * Copyright (c) 2014 Martin Marinov.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Public License v3.0
 * which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/gpl.html
 *
 * Contributors:
 *     Martin Marinov - initial API and implementation
 ******************************************************************************/

#include "frameratedetector.h"
#include "internaldefinitions.h"
#include <stdio.h>
#include <assert.h>

#define MIN_FRAMERATE (40)
#define MAX_FRAMERATE (90)

#define FRAMERATE_RUNS (10)


void frameratedetector_init(frameratedetector_t * frameratedetector) {
}

inline static double frameratedetector_fitvalue(float * data, int offset, int length) {
	double sum = 0.0;
	int i;
	for (i = 0; i < length; i++) {
		const float val1 = data[i];
		const float val2 = data[i+offset];
		const double difff = val1 - val2;
		sum += difff * difff;
	}
	return sum;
}

inline static int frameratedetector_estimatedirectlength(float * data, int length, int endlength, int startlength) {
	int bestlength = startlength;
	int l = startlength;
	float bestfitvalue = frameratedetector_fitvalue(data, l, length);
	l++;
	while (l < endlength) {
		const float fitvalue = frameratedetector_fitvalue(data, l, length);
		if (fitvalue < bestfitvalue) {
			bestfitvalue = fitvalue;
			bestlength = l;
		}
		l++;
	}

	return bestlength;
}

inline static int frameratedetector_estimateintlength(float * data, int length, int endlength, int startlength) {
	const int bestintestimate = frameratedetector_estimatedirectlength(data, length, endlength, startlength);
	return bestintestimate;
}

float frameratedetector_run(frameratedetector_t * frameratedetector, tsdr_lib_t * tsdr, float * data, int size, uint32_t samplerate) {
	const int maxlength = samplerate / (double) (MIN_FRAMERATE * tsdr->height);
	const int minlength = samplerate / (double) (MAX_FRAMERATE * tsdr->height);

	const int searchsize = 2*minlength + maxlength;
	const int lastindex = size - searchsize;

	assert (lastindex > 1);

	const int offsetstep = lastindex / FRAMERATE_RUNS;

	assert (offsetstep > 1);

	// estimate the length of a horizontal line in samples
	double crudelength = 0.0;
	int i;
	for (i = 0; i < FRAMERATE_RUNS; i++)
		crudelength += frameratedetector_estimateintlength(&data[i*offsetstep], minlength, maxlength, minlength) / (double) FRAMERATE_RUNS;

	const double fps = samplerate / (double) (crudelength * tsdr->height);

	printf("%f\n", fps); fflush(stdout);

	return fps;
}

void frameratedetector_free(frameratedetector_t * frameratedetector) {

}
