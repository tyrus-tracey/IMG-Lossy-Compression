/*
    General helper functions not tied to a specific class.
*/

#pragma once
#include <string>
#include <vector>
#include "Node.h"
#include "symbolDistribution.h"

// Compare for lower-frequency node
struct CompareNodes {
    bool operator()(const Node node1, const Node node2)
    {
        return node1.getFrequency() > node2.getFrequency();
    }
};

// Takes a sorted string vector and creates nodes with associated frequencies
symbolDistribution::symbolDistribution(std::vector<std::string>& input)
{
    int freq;
    std::vector<std::string>::iterator iter = input.begin();
    std::string symbol;
    while (iter != input.end()) {
        symbol = *iter;
        freq = 0;
        while (iter != input.end() && symbol == *iter) {// second bounds-check required here
            freq++;                                     // otherwise the program will try to
            iter++;                                     // dereference input.end()
        }
        data.push_back(Node(symbol, freq));             // Create node
    }
    
}

std::vector<Node> symbolDistribution::getData()
{
    return data;
}

int symbolDistribution::getSum()
{
    std::vector<Node>::iterator iter = data.begin();
    int sum = 0;
    while (iter != data.end()) {
        sum += iter->getFrequency();
        iter++;
    }
    return sum;
}