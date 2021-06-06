#ifndef TREE1V2_SUBTREEINFO_H
#define TREE1V2_SUBTREEINFO_H

//struct to retrieve some data of the subtrees of a node
struct subtreeInfo{
    //the preorder of a subtree
    uint16_t preorder;
    //the size of the subtree
    uint16_t subtreeSize;
    //constructors
    subtreeInfo() {};
    subtreeInfo(uint16_t _preorder, uint16_t _subtreeSize){
        preorder = _preorder;
        subtreeSize = _subtreeSize;
    };
};

#endif //TREE1V2_SUBTREEINFO_H
