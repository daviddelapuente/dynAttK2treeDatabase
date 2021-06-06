#ifndef TREE1V2_TREEBLOCK_H
#define TREE1V2_TREEBLOCK_H

//this is the main structure. the more important attributes are:
//dfuds: is the bit array containing the topology
//ptr: is the array of pointers to the child blocks (the frontier)
struct treeBlock
{
    //attributes
    uint8_t rootDepth;  // depth of the root of the block
    uint16_t nNodes;  // number of nodes in the tree block
    uint16_t maxNodes; // maximum number of nodes in the block. Also, it is the size of the next array.
    uint16_t *dfuds;  // DFUDS of the tree block
    void /*blockPTR*/ *ptr;    // Pointers to child blocks (this is a pointer to a data structure containing the pointer to the block (please read  blockPtr below))
    uint16_t nPtrs;   // number of pointers to child blocks
    blockGlobalVars *bgv;

    //methods
    //inserts in a block
    void insert(treeNode, uint8_t[], uint64_t, uint16_t, uint64_t, uint16_t);
    //takes me to the position in dfuds, where is the child of the node with symbol
    treeNode skipChildrenSubtree(treeNode &, uint8_t, uint16_t &, uint16_t, uint16_t &);
    //return the next child in function of the symbol (this function is a wrapper of skipChildrenSubtree but identificate when a child is in the block or in a child block)
    treeNode child(treeBlock *&, treeNode &, uint8_t, uint16_t &, uint16_t, uint16_t &);
    // grow the dfuds and update maxNodes
    void grow(uint16_t extraNodes);
    // shrink dfuds and update maxNodes
    void shrink(uint16_t deletedNodes);
    void shrink2();
    //return a treeNode where the subtree begin (the end is in subTreeSize)
    treeNode selectSubtree2(uint16_t maxDepth, uint16_t & subTreeSize, uint16_t & depthSelectN);
    //return the pointer to a treeBlock pointer, in function of the current flag (fronteir node)
    struct treeBlock *getPointer(uint16_t);
    //this function calculate the size of the whole tree
    uint64_t size();

    //free the data structure
    void freeTreeBlock();
    void freeTreeBlockButKeepFrontier();
    ~treeBlock(){;};
};

#endif //TREE1V2_TREEBLOCK_H
