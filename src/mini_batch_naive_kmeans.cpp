//
// Created by shresthasumit55 on 10/22/19.
//

#include "mini_batch_naive_kmeans.h"
#include "general_functions.h"
#include <cassert>
#include <cstring>

int MiniBatchNaiveKmeans::runThread(int threadId, int maxIterations) {


    int startNdx = start(threadId);
    int endNdx = end(threadId);

    const int dataSize=x->n;
    int *indexArray;

    indexArray = new int[dataSize];

    for (int i=0;i<x->n;i++){
        indexArray[i] = i;
    }

    // track the number of iterations the algorithm performs
    int iterations = 0;

        while ((iterations < maxIterations) && (! converged)) {

            ++iterations;

            //Generating a new batch

            //will need to change random generation
            //srand(time(0));

            for (int i=dataSize-1;i>0;i--){
                int j = rand() % (i+1);
                swap(&indexArray[i],&indexArray[j]);
            }

            for (int i=0;i<batchSize;i++) {

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
            int *centerMembersCount = new int[k]{0};

            for (int i=0;i<batchSize;i++){

                int c = assignment[indexArray[i]];

                centerMembersCount[c] = centerMembersCount[c]+1;
                double eta = 1/centerMembersCount[c];
                for (int j=0;j<d;j++){
                    centers->data[c+j] = (1-eta) * (centers-> data[c + j]) + (eta * x->data[indexArray[i] + j]);
                }

            }

            }

            synchronizeAllThreads();

            if (threadId == 0) {
                int furthestMovingCenter = move_centers();
                converged = (0.0 == centerMovement[furthestMovingCenter]);
            }

            synchronizeAllThreads();
            return iterations;

}

void MiniBatchNaiveKmeans::swap(int *val1, int *val2){
    int temp = *val1;
    *val1 = *val2;
    *val2 = temp;
}