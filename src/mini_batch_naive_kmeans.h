//
// Created by g5 on 10/22/19.
//

#ifndef FAST_KMEANS_MINI_BATCH_NAIVE_KMEANS_H
#define FAST_KMEANS_MINI_BATCH_NAIVE_KMEANS_H

#include "triangle_inequality_base_kmeans.h"

/* Author: Sumit Shrestha Khimbaja
 * MiniBatchNaiveKmeans implements mini batch k means algorithm with no distance bounds.
 */

class MiniBatchNaiveKmeans : public TriangleInequalityBaseKmeans {
public:
    MiniBatchNaiveKmeans(int size, int numIterations) {
        batchSize = size;
        numberOfIterations = numIterations;

        //std::cout<<"inside naive";
    }

    virtual std::string getName() const { return "minibatchnaive"; }

    virtual ~MiniBatchNaiveKmeans() { free(); }

protected:
    int batchSize;
    int numberOfIterations;

    virtual int runThread(int threadId, int maxIterations);

    void shuffleArray(int dataSize,int *indexArray);

};


#endif //FAST_KMEANS_MINI_BATCH_NAIVE_KMEANS_H
