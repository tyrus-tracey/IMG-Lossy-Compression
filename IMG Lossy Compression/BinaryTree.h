#pragma once
#include "Node.h"
#include <vector>

// Compare for lower-frequency node
struct CompareNodes {
	bool operator()(const Node node1, const Node node2)
	{
		return node1.getFrequency() > node2.getFrequency();
	}
};

class BinaryTree
{
public:
	BinaryTree(std::vector<Node> distribution);
	~BinaryTree();
	int size() const;
	bool empty() const;
	Node* root();
	Node* position;
	void printTree();
	void deleteTree(Node* node);
	void generateCodes();
	double averageCodeLength();

private:
	void generateCode(Node* node);
	double getCodeLength(Node* node);
	void printNode(Node* node);

	int treeSize;
	Node* rootNode;
};

