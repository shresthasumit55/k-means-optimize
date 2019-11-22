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
    virtual std::string getName() const { return "minibatch-naive"; }
    virtual ~MiniBatchNaiveKmeans() { free(); }
protected:
    virtual int runThread(int threadId, int maxIterations);

};


#endif //FAST_KMEANS_MINI_BATCH_NAIVE_KMEANS_H
