/*
 * the-matrix: simple demo app for profiling with perf
 * Based on:
 *  - https://github.com/petersenna/rdtscbench
 *  - http://stackoverflow.com/questions/4305553/optimization-tips/9226080#9226080
 *
 * Released under GPL v2
 * Peter Senna Tschudin <peter.senna@gmail.com>
 * Peter Senna Tschudin <peter.senna@collabora.com>
 */

#include <memory.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define X_SIZE  10000
#define Y_SIZE  32
int matrix[X_SIZE][Y_SIZE];

void runt_measure (char *msg, void (*function)(struct timespec *ts, struct timespec *te), int reps);
void getnsec(struct timespec *ts);
void simple_clear (struct timespec *ts, struct timespec *te);
void reg_simple_clear (struct timespec *ts, struct timespec *te);
void pointer_clear (struct timespec *ts, struct timespec *te);
void memset_clear (struct timespec *ts, struct timespec *te);

/*
 * Works only on X86 using RDSTS asm instruction
 *
 *__inline__ void getcycles(struct timespec *ts)
 *{
 *	unsigned int a, d;
 *
 *	asm("cpuid");
 *
 *	asm volatile("rdtsc" : "=a" (a), "=d" (d));
 *
 *	ts->tv_nsec = (((uint64_t)a) | (((uint64_t)d) << 32));
 *}
 */

__inline__ void getnsec(struct timespec *ts)
{
	/* clock_gettime(CLOCK_MONOTONIC, ts); */
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, ts);
}

void inline simple_clear (struct timespec *ts, struct timespec *te)
{
	int x, y;

	getnsec ( ts );

	for (x = 0; x < X_SIZE; ++x)
		for (y = 0; y < Y_SIZE; ++y)
			matrix[x][y] = -1;

       	getnsec ( te );
}

void inline reg_simple_clear (struct timespec *ts, struct timespec *te)
{
	register int x, y;

	getnsec ( ts );

	for (x = 0; x < X_SIZE; ++x)
		for (y = 0; y < Y_SIZE; ++y)
			matrix[x][y] = -1;

       	getnsec ( te );
}

/* There is a similar example. It may be interesting to try to
 * compare the performance impact if you have time:
 *
 * - http://en.cppreference.com/w/cpp/algorithm/fill
 */
void inline pointer_clear (struct timespec *ts, struct timespec *te)
{
	register int index;
	register int *matrix_ptr;

	getnsec ( ts );

	matrix_ptr = &matrix[0][0];

	for (index = 0; index < X_SIZE * Y_SIZE; ++index){
		*matrix_ptr = -1;
		matrix_ptr++;
	}

	getnsec ( te );
}

/* memset_clear will only work on two-complement machines, or machines where -x
 * is (~x + 1). Valid values here are only 0 and -1. This is due matrix being
 * an multi-dimensional array, and memset not being aware of the structure of
 * matrix.
 */
void inline memset_clear (struct timespec *ts, struct timespec *te)
{
       	getnsec ( ts );

	memset (matrix, -1, sizeof(matrix));

       	getnsec ( te );

}


void inline runt_measure (char *msg, void (*function)(struct timespec *ts, struct timespec *te), int reps)
{
	struct timespec begin, end, diff;
	long double valuemean, acc = 1, power = 1;
	int i = 0;

	for (i = reps; i > 0; i--){
		function(&begin, &end);

		diff.tv_nsec = end.tv_nsec - begin.tv_nsec;

		acc *= (long)(double) diff.tv_nsec;
	}
	power /= reps;

	valuemean = powl (acc, power);
	printf ("%s : %6.0Lf nanoseconds\n", msg, valuemean);
}

int main (int argc, char *argv[])
{
	int reps;

	if (argc < 2) {
		printf ("Please specify the number of reps.\n");
		return -1;
	}

	reps = atoi(argv[1]);

	printf("Results will be a geometric mean of %d, results.\n", reps);

	runt_measure("simple_clear()    ", &simple_clear, reps);
	runt_measure("reg_simple_clear()", &reg_simple_clear, reps);
	runt_measure("pointer_clear()   ", &pointer_clear, reps);
	runt_measure("memset_clear()    ", &memset_clear, reps);
}
