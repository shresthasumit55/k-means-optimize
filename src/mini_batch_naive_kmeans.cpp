//
// Created by shresthasumit55 on 10/22/19.
//

#include "mini_batch_naive_kmeans.h"
#include "general_functions.h"
#include <cassert>
#include <cstring>
#include <cmath>


int MiniBatchNaiveKmeans::runThread(int threadId, int maxIterations) {


    int startNdx = start(threadId);
    int endNdx = end(threadId);


    const int dataSize = x->n;
    const int dimensions = x->d;
    int *indexArray;

    indexArray = new int[dataSize];

    for (int i = 0; i < x->n; i++) {
        indexArray[i] = i;
    }

    // track the number of iterations the algorithm performs
    int iterations = 0;

    int *centerMembersCount = new int[k]{0};

    Dataset *oldCenters = new Dataset(k, x->d);

    while ((iterations < numberOfIterations)) {

        ++iterations;

        //Generating a new batch
        //shuffling the array
        for (int i=0;i<batchSize;i++){
            int j = i + (rand() % (dataSize-i));
            std::swap(indexArray[i],indexArray[j]);
        }



        for (int i = 0; i < batchSize; i++) {

            // look for the closest center to this example
            int closest = 0;
            double closestDist2 = std::numeric_limits<double>::max();
            for (int j = 0; j < k; ++j) {
                double d2 = pointCenterDist2(indexArray[i], j);
                if (d2 < closestDist2) {
                    closest = j;
                    closestDist2 = d2;
                }
            }
            if (assignment[indexArray[i]] != closest) {
                changeAssignment(indexArray[i], closest, threadId);
            }
        }

        verifyAssignment(iterations, startNdx, endNdx);

        synchronizeAllThreads();


        //updating the centers

        for (int i = 0; i < batchSize; i++) {

            int dataIdx = indexArray[i];
            int c = assignment[dataIdx];
            centerMembersCount[c] = centerMembersCount[c] + 1;
            double eta = (double)1 / centerMembersCount[c];
            for (int j = 0; j < dimensions; j++) {
                //(*centers)(c,j) = (1 - eta) * (*centers)(c,j) + eta * x->data[indexArray[i] + j];
                centers->data[c*dimensions + j] = (1 - eta) * centers->data[c * dimensions + j] + eta * x->data[dataIdx * dimensions + j];
            }

        }

        synchronizeAllThreads();

    }

    delete[] centerMembersCount;
    delete oldCenters;
    return iterations;

}

void MiniBatchNaiveKmeans::shuffleArray(int dataSize,int *indexArray){

    for (int i=0;i<batchSize;i++){
        int j = i + (rand() % (dataSize-i));
        std::swap(indexArray[i],indexArray[j]);
    }
}

