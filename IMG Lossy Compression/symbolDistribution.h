/*
    General helper functions not tied to a specific class.
*/

#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include "Node.h"


// A record of nodes+frequencies to create Huffman trees
class symbolDistribution
{
public:
    symbolDistribution(std::vector<std::string>& input);
    int getSum();
    std::vector<Node> getData();
    
private:
    std::vector<Node> data;
};
