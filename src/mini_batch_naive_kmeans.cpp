//
// Created by shresthasumit55 on 10/22/19.
//

#include "mini_batch_naive_kmeans.h"
#include "general_functions.h"
#include <cassert>
#include <cstring>
#include <map>
#include <set>

int MiniBatchNaiveKmeans::runThread(int threadId, int maxIterations) {


    int startNdx = start(threadId);
    int endNdx = end(threadId);

    int batchSize = 400;

    // track the number of iterations the algorithm performs
    int iterations = 0;

        while ((iterations < maxIterations) && (! converged)) {

            ++iterations;

            //Generating a new batch
            std::set<int> miniBatch;
            std::map<int,int> centresMap;
            int max = x->n;
            int batchCount = 0;

            srand(time(0));
            while (batchCount<batchSize){
                int currentIndex = (rand() % max);
                miniBatch.insert(currentIndex);
                batchCount = miniBatch.size();
            }

            for (int dataIdx:miniBatch) {

                // look for the closest center to this example
                int closest = 0;
                double closestDist2 = std::numeric_limits<double>::max();
                for (int j = 0; j < k; ++j) {
                    double d2 = pointCenterDist2(dataIdx, j);
                    if (d2 < closestDist2) {
                        closest = j;
                        closestDist2 = d2;
                    }
                }
                if (assignment[dataIdx] != closest) {
                    changeAssignment(dataIdx, closest, threadId);
                }
            }

            verifyAssignment(iterations, startNdx, endNdx);


            for (int dataIdx: miniBatch) {

                int c = assignment[dataIdx];

                std::map<int, int>::iterator iter;
                iter=centresMap.find(c);
                int value = 1;
                if (iter == centresMap.end()) {
                    centresMap.insert(std::pair<int,int>(c,value));
                }else {
                    iter->second = iter->second + 1;
                    value = iter->second;
                }
                double eta = 1/value;
                for (int j=0;j<d;j++){
                    centers->data[c+j] = (1-eta) * (centers-> data[c + j]) + (eta * x->data[dataIdx + j]);
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
