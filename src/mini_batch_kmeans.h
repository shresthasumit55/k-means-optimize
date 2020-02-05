/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mini_batch_kmeans.h
 * Author: sumit shrestha
 *
 * Created on August 21, 2019, 8:23 PM
 */

#ifndef MINI_BATCH_KMEANS_H
#define MINI_BATCH_KMEANS_H

#include "triangle_inequality_base_kmeans.h"

class MiniBatchKMeans : public TriangleInequalityBaseKmeans {
public:

    MiniBatchKMeans(int a, int b) {
        numLowerBounds = 1;
        batchSize = a;
        totalMinibatchIterations = b;
    }

    virtual std::string getName() const { return "mini-batch"; }
    virtual ~MiniBatchKMeans() { free(); }

protected:
    int batchSize;
    int totalMinibatchIterations;

    void swap(int *val1, int *val2);

    void update_bounds(int *indexArray);

    virtual int runThread(int threadId, int maxIterations);
};

#endif /* MINI_BATCH_KMEANS_H */

