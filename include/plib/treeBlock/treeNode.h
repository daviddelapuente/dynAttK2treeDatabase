#ifndef TREE1V2_TREENODE_H
#define TREE1V2_TREENODE_H

//A node within a block represented as a pair <index of the short int that contains the node, offset of the node within the short int>. The offset is 0, 1, 2, or 3
typedef std::pair <NODE_TYPE,OFFSET_TYPE> treeNode;

//function that returns the the absolute position of a node in the block.
//this absolute position is calculated first geting the NODE_TYPE, and then we have 4 consecutives posibilities
//and we use the OFFSET_TYPE to choose one.
uint16_t absolutePosition(treeNode &node);


#endif //TREE1V2_TREENODE_H
