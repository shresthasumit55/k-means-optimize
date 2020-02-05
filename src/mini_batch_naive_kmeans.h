//
// Created by g5 on 10/22/19.
//


#include "naive_kmeans.h"


#ifndef FAST_KMEANS_MINI_BATCH_NAIVE_KMEANS_H
#define FAST_KMEANS_MINI_BATCH_NAIVE_KMEANS_H


/* Author: Sumit Shrestha Khimbaja
 * MiniBatchNaiveKmeans implements mini batch k means algorithm with no distance bounds.
 */

class MiniBatchNaiveKmeans: public NaiveKmeans {
public:
    MiniBatchNaiveKmeans(int a) {
        batchSize = a;
    }
    virtual std::string getName() const { return "minibatch-naive"; }
    virtual ~MiniBatchNaiveKmeans() { free(); }
protected:
    int batchSize;
    virtual int runThread(int threadId, int maxIterations);
    void swap(int *val1, int *val2);

};


#endif //FAST_KMEANS_MINI_BATCH_NAIVE_KMEANS_H
