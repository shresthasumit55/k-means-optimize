/*
 *
 * Created by shresthasumit55 on 10/22/19.
 */

#include "mini_batch_kmeans.h"
#include "general_functions.h"
#include "hamerly_kmeans.h"
#include "general_functions.h"
#include <cmath>
#include <vector>
#include <map>
#include <fstream>

/*
 * Bounded mini batch k means using hamerly's bound.
 */


int MiniBatchKMeans::runThread(int threadId, int maxIterations) {

    if (totalMinibatchIterations == 0) {
        return 0;
    }

    int iterations = 0;
    int startNdx = start(threadId);
    int endNdx = end(threadId);

    const int dataSize = x->n;
    int *batchIndexArray;
    //holds the values for indexes of batches
    batchIndexArray = new int[dataSize];

    for (int i = 0; i < x->n; i++) {
        batchIndexArray[i] = i;
    }

    double *centerMembersCount = new double[k]{0};
    Dataset *oldCenters = new Dataset(k, x->d);

    //starting the outer iterations
    for (int run = 0; run < totalMinibatchIterations; run++) {

        //fisher yates algorithm to generate batches.
        for (int i = 0; i < batchSize; i++) {
            int j = i + (rand() % (dataSize - i));
            std::swap(batchIndexArray[i], batchIndexArray[j]);
        }

        converged = false;

        //this is the inner iteration which denotes number of times the same batch is run
        iterations = 0;
        float multiplier = pow(maxIterations, scalingFactor);

        while ((iterations < maxIterations) && (!converged)) {
            ++iterations;
            int dataIndex;

            update_s(threadId);

            for (int i = 0; i < batchSize; i++) {
                dataIndex = batchIndexArray[i];

                unsigned short closest = assignment[dataIndex];

                // if upper[i] is less than the greater of these two, then we can
                // ignore record i
                double upper_comparison_bound = std::max(s[closest], lower[dataIndex]);

                // first check: if u(x) <= s(c(x)) or u(x) <= lower(x), then ignore
                // x, because its closest center must still be closest
                if (upper[dataIndex] <= upper_comparison_bound) {
                    continue;
                }

                // otherwise, compute the real distance between this record and its
                // closest center, and update upper
                double u2 = pointCenterDist2(dataIndex, closest);
                upper[dataIndex] = sqrt(u2);

                // if (u(x) <= s(c(x))) or (u(x) <= lower(x)), then ignore x
                if (upper[dataIndex] <= upper_comparison_bound) {
                    continue;
                }

                // now update the lower bound by looking at all other centers
                double l2 = std::numeric_limits<double>::max(); // the squared lower bound
                for (int j = 0; j < k; ++j) {
                    if (j == closest) { continue; }

                    double dist2 = pointCenterDist2(dataIndex, j);

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
                lower[dataIndex] = sqrt(l2);

                // if the assignment for i has changed, then adjust the counts and
                // locations of each center's accumulated mass
                if (assignment[dataIndex] != closest) {
                    upper[dataIndex] = sqrt(u2);
                    changeAssignment(dataIndex, closest, threadId);
                }
            }

            verifyAssignment(iterations, startNdx, endNdx);

            synchronizeAllThreads();

            for (int i=0;i<k;i++){
                for (int j = 0; j < d; j++) {
                    oldCenters->data[i*d + j] = centers->data[i*d+j];
                }
            }

            for (int i = 0; i < batchSize; i++) {

                int dataIdx = batchIndexArray[i];
                int c = assignment[dataIdx];

                centerMembersCount[c] = centerMembersCount[c] + (double) 1 / (multiplier);
                double eta = (double) 1 / (centerMembersCount[c]);

                for (int j = 0; j < d; j++) {
                    centers->data[c * d + j] = (1 - eta) * centers->data[c * d + j] + eta * x->data[dataIdx * d + j];
                }

            }

            for (int i=0;i<k;i++){
                double tempSum = 0.0;
                for (int j = 0; j < d; j++) {
                    tempSum+= pow((oldCenters->data[i*d + j] - centers->data[i*d+j]),2);
                }
                centerMovement[i] = sqrt(tempSum);
            }

            if (!converged) {
                update_bounds(batchIndexArray);
            }

            synchronizeAllThreads();
        }

    }

    for (int i = startNdx; i < endNdx; ++i) {
        // look for the closest center to this example
        int closest = 0;
        double closestDist2 = std::numeric_limits<double>::max();
        for (int j = 0; j < k; ++j) {
            double d2 = pointCenterDist2(i, j);
            if (d2 < closestDist2) {
                closest = j;
                closestDist2 = d2;
            }
        }
        if (assignment[i] != closest) {
            changeAssignment(i, closest, threadId);
        }
    }

    delete oldCenters;

    delete[] centerMembersCount;

    delete[] batchIndexArray;

    return iterations;
}

void MiniBatchKMeans::update_bounds(int *indexArray) {
    double longest = centerMovement[0], secondLongest = (1 < k) ? centerMovement[1] : centerMovement[0];
    int furthestMovingCenter = 0;

    if (longest < secondLongest) {
        furthestMovingCenter = 1;
        std::swap(longest, secondLongest);
    }

    for (int j = 2; j < k; ++j) {
        if (longest < centerMovement[j]) {
            secondLongest = longest;
            longest = centerMovement[j];
            furthestMovingCenter = j;
        } else if (secondLongest < centerMovement[j]) {
            secondLongest = centerMovement[j];
        }
    }

    // update upper/lower bounds
    int dataIndex;
    for (int i = 0; i < batchSize; i++) {
        dataIndex = indexArray[i];
        // the upper bound increases by the amount that its center moved
        upper[dataIndex] += centerMovement[assignment[dataIndex]];

        // The lower bound decreases by the maximum amount that any center
        // moved, unless the furthest-moving center is the one it's assigned
        // to. In the latter case, the lower bound decreases by the amount
        // of the second-furthest-moving center.
        lower[dataIndex] -= (assignment[dataIndex] == furthestMovingCenter) ? secondLongest : longest;
    }
}

