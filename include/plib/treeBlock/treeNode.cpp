#include "treeNode.h"
// Transform a node from it pair <NODE_TYPE,OFFSET_TYPE> to it absolute representation (position) in DFUDS
uint16_t absolutePosition(treeNode &node){
    return 4*node.first + node.second;
}

/*give the next node in the dfuds.
for example, if your node is <1,1> it absolute position will be 5
then the next node will be <1,2> because it absolute position will be 6*/
inline void nextNode(treeNode &node){
    //this is like (second+1)%4
    node.second = (node.second+1) & 0x3;
    //this sum 1 <=> node.second=0, and will sum 0 in an ohter case
    node.first += !node.second;
}

/*give the previous node in the dfuds.
for example, if your node is <1,0> it absolute position will be 4
then the next node will be <0,3> because it absolute position will be 3*/
inline void prevNode(treeNode &node){
    //only subtract 1 if node.second=0
    node.first -= !node.second;
    //this is like (second-1)%4
    node.second = (node.second-1) & 0x3;
}