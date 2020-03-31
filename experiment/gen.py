#!/usr/bin/env python3

import random
import sys
import argparse
import math

def scalePoints(p, scale):
    return [[pij * scale for pij in pi] for pi in p]

def translatePoints(p, translation):
    return [[pij + translation[j] for j, pij in enumerate(pi)] for pi in p]

def genCluster(n, d, generator):
    return [[generator() for _ in range(d)] for _ in range(n)]

def genUniform(n, d):
    return genCluster(n, d, lambda: random.uniform(-1, 1))

def genGaussian(n, d):
    return genCluster(n, d, lambda: random.gauss(0, 1))

def distribution(s):
    d = { 'gaussian': genGaussian, 'uniform': genUniform }
    if s not in d:
        raise argparse.ArgumentError()
    return d[s]

def genPoints(d=2, k=1, n=100, centerDistribution=genGaussian, clusterDistribution=genGaussian, scale=5):
    # transform the scale by dividing by the expected length of a d-dimensional
    # Gaussian random vector, so that the scale the user gives is 'natural'
    scale = scale / math.sqrt(d)
    centers = scalePoints(centerDistribution(k, d), scale)

    data = []
    for i in range(k):
        data.extend(translatePoints(clusterDistribution(n, d), centers[i]))

    return data

def main():
    ap = argparse.ArgumentParser(description='Generate a random dataset')
    ap.add_argument('--d', help='Data dimension', default=2, type=int)

    ap.add_argument('--k', help='Number of (base) clusters', default=1, type=int)
    ap.add_argument('--n', help='Number of points per cluster (not total)', default=100, type=int)

    ap.add_argument('--dc', help='Distribution of cluster centers', default='gaussian', type=distribution)
    ap.add_argument('--dp', help='Distribution of cluster points', default='gaussian', type=distribution)

    ap.add_argument('--scale', help='Natural scale of cluster center separation (will be divided by sqrt(tdimension))', default=5, type=float)

    args = ap.parse_args()

    data = genPoints(args.d, args.k, args.n, args.dc, args.dp, args.scale)

    for p in data:
        print(' '.join(map(str, p)))

if __name__ == '__main__':
    main()

