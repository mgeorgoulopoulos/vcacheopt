/*
 * test.cpp - vcacheopt test program
 * Copyright 2009 Michael Georgoulpoulos <mgeorgoulopoulos at gmail>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "vcacheopt.h"
#include <chrono>
#include <stdio.h>
#include <stdlib.h>

#define PLANE_SIZE 100
#define RANDOM_SWAPS 0

// plane index buffer generator
std::vector<int> GeneratePlane(int n);

int main(int , char **)
{
	// generate the plane
	std::vector<int> pl = GeneratePlane(PLANE_SIZE);
	int tri_count = 2 * (PLANE_SIZE-1) * (PLANE_SIZE-1);

	printf("Mesh triangle count: %d\n", tri_count);

	VertexCache vertex_cache;
	int misses = vertex_cache.GetCacheMissCount(&pl[0], tri_count);

	printf("*** Before optimization ***\n");
	printf("Cache misses\t: %d\n", misses);
	printf("ACMR\t\t: %f\n", (float)misses / (float)tri_count);

	VertexCacheOptimizer vco;
	printf("Optimizing ... \n");
	const auto startTime = std::chrono::high_resolution_clock::now();
	VertexCacheOptimizer::Result res = vco.Optimize(&pl[0], tri_count);
	auto endTime = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double, std::milli> duration = endTime - startTime;

	if (res)
	{
		printf("Error\n");
		return 0;
	}

	printf("Optimized in %f seconds (%d ns/triangle)\n",
		duration.count() / 1000.0f, (static_cast<int>(duration.count()) * 1000) / tri_count);

	misses = vertex_cache.GetCacheMissCount(&pl[0], tri_count);

	printf("*** After optimization ***\n");
	printf("Cache misses\t: %d\n", misses);
	printf("ACMR\t\t: %f\n", (float)misses / (float)tri_count);

	return 0;
}

std::vector<int> GeneratePlane(int n)
{
	std::vector<int> ret;

	for (int i=0; i<n-1; i++)
	{
		for (int j=0; j<n-1; j++)
		{
			int top_left = i + n * j;
			int top_right = top_left + 1;
			int bottom_left = top_left + n;
			int bottom_right = bottom_left + 1;

			ret.push_back(top_left);
			ret.push_back(top_right);
			ret.push_back(bottom_left);

			ret.push_back(top_right);
			ret.push_back(bottom_right);
			ret.push_back(bottom_left);
		}
	}

	int tri_count = (int)ret.size() / 3;

	// swap some triangles
	for (int i=0; i<RANDOM_SWAPS; i++)
	{
		int swap_a = rand() % tri_count;
		int swap_b = rand() % tri_count;

		for (int j=0; j<3; j++)
		{
			int tmp = ret[3 * swap_a + j];
			ret[3 * swap_a + j] = ret[3 * swap_b + j];
			ret[3 * swap_b + j] = tmp;
		}
	}

	return ret;
}


/*
 * Cross platform timer routines
 * Thanks to John Tsiombikas
 */