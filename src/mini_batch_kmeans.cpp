/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "mini_batch_kmeans.h"
#include "general_functions.h"
#include "hamerly_kmeans.h"
#include "general_functions.h"
#include <cmath>
#include <vector>
#include <map>

int MiniBatchKMeans::runThread(int threadId, int maxIterations) {
    int iterations = 0;

    int startNdx = start(threadId);
    int endNdx = end(threadId);


    std::map<int,int> batchMap;
    std::map<int, int>::iterator it;
    int batchSize = 400;
    int max = x->n;

    batchMap.insert(std::pair<int, int>(0, 1));

    int i=0;
    srand(time(0));
    while (i<batchSize){
        int currentIndex = (rand() % max);
        it = batchMap.find(currentIndex);
        if (it == batchMap.end()){
            batchMap.insert(std::pair<int,int>(currentIndex,1));
            i++;
        }

    }

    while ((iterations < maxIterations) && ! converged) {
        ++iterations;

        // compute the inter-center distances, keeping only the closest distances
        update_s(threadId);
        synchronizeAllThreads();

        // loop over all records
        for (int i = startNdx; i < endNdx; ++i) {

            std::map<int, int>::iterator it;
            it=batchMap.find(i);
            if (it == batchMap.end()) {
                continue;
            }
            unsigned short closest = assignment[i];

            // if upper[i] is less than the greater of these two, then we can
            // ignore record i
            double upper_comparison_bound = std::max(s[closest], lower[i]);

            // first check: if u(x) <= s(c(x)) or u(x) <= lower(x), then ignore
            // x, because its closest center must still be closest
            if (upper[i] <= upper_comparison_bound) {
                continue;
            }

            // otherwise, compute the real distance between this record and its
            // closest center, and update upper
            double u2 = pointCenterDist2(i, closest);
            upper[i] = sqrt(u2);

            // if (u(x) <= s(c(x))) or (u(x) <= lower(x)), then ignore x
            if (upper[i] <= upper_comparison_bound) {
                continue;
            }

            // now update the lower bound by looking at all other centers
            double l2 = std::numeric_limits<double>::max(); // the squared lower bound
            for (int j = 0; j < k; ++j) {
                if (j == closest) { continue; }

                double dist2 = pointCenterDist2(i, j);

                if (dist2 < u2) {
                    // another center is closer than the current assignment

                    // change the lower bound to be the current upper bound
                    // (since the current upper bound is the distance to the
                    // now-second-closest known center)
                    l2 = u2;

                    // adjust the upper bound and the current assignment
                    u2 = dist2;
                    closest = j;
                } else if (dist2 < l2) {
                    // we must reduce the lower bound on the distance to the
                    // *second* closest center to x[i]
                    l2 = dist2;
                }
            }

            // we have been dealing in squared distances; need to convert
            lower[i] = sqrt(l2);

            // if the assignment for i has changed, then adjust the counts and
            // locations of each center's accumulated mass
            if (assignment[i] != closest) {
                upper[i] = sqrt(u2);
                changeAssignment(i, closest, threadId);
            }
        }

        verifyAssignment(iterations, startNdx, endNdx);

        // ELKAN 4, 5, AND 6
        // calculate the new center locations
        synchronizeAllThreads();
        if (threadId == 0) {
            int furthestMovingCenter = move_centers();
            converged = (0.0 == centerMovement[furthestMovingCenter]);
        }

        synchronizeAllThreads();

        if (! converged) {
            HamerlyKmeans::update_bounds(startNdx, endNdx);
        }

        synchronizeAllThreads();
    }

    return iterations;
}
