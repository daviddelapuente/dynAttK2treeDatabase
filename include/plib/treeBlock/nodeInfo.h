#ifndef TREE1V2_NODEINFO_H
#define TREE1V2_NODEINFO_H

//struct to retrieve some data of the nodes
struct nodeInfo{
    //preorder: give the preorder of the node in the topology
    uint16_t preorder;
    //give the number of children the node have from 1 to 4
    uint16_t nChildren;
    //constructors
    nodeInfo() {};
    nodeInfo(uint16_t _preorder, uint16_t _nChildren){
        preorder = _preorder;
        nChildren = _nChildren;
    };
};


#endif //TREE1V2_NODEINFO_H
