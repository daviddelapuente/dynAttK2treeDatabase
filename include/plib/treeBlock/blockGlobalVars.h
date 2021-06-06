#ifndef TREE1V2_BLOCKGLOBALVARS_H
#define TREE1V2_BLOCKGLOBALVARS_H

//this is a structure that will contain global variables of the blocks
//this means that all the blocks of the same k2tree has the same values
//but blocks of diferents k2trees might have diferent values for the same variable
struct blockGlobalVars{

    //min and max size of a block
    uint16_t N1;
    uint16_t Nt;
    double alpha;
    // Global table that stores the sizes of the blocks, used for growing them
    uint16_t *sizeArray; // array of sizes

    //this variables are used in treeBlock::selectSubtree2
    //array of subtree info
    nodeInfo stackSS[4096];
    //this array contains all the subtrees
    subtreeInfo subtrees[4096];
    //array of deepth of every subtree
    uint16_t depthVector[4096];

    //var used in treeBlock::insert
    //dumyroot=tuple 0,0
    treeNode dummyRootBlockNode;

    //var used in  treeBlock::skipChildrenSubtree
    int8_t stack[100];

    //var used in treeBlock::child
    //null node
    treeNode NULL_NODE;

    //vars used in treeBlock::child and deleteTrie2 and deleteTrie and deleteBlockNodes2 and deleteBlockNodes
    int changedBlock;
    int deleting;

    //stack of treeNodes
    treeNode delTreeNodeStack[4096];
    uint8_t delTreePathStack[4096];
    int delTreeNodeIndex;

    //how many nodes of the path are in the block
    uint8_t nodesInBlockStack[4096];
    uint8_t flagPathBlockStack[4096];

    //stack of treeBlock
    //array of treeBlock (is void because, bgv uses treeBlock, and treeBlock use bgv.
    void * delBlockStack[4096];
    int delBlockNodeIndex;

    //this stack will contain the first L1 nodes from the path we want to delete
    //array of trieNode (is void because, bgv uses trieNode, and trieNode use bgv.
    void* delTrieNodeStack[4096];
    //this stack will contain the step the path take in each node (this means it has numbers from 0 to 3)
    //in other words delPathStack[i] contain the character for the path of node delTrieNodeStack[i] (corresponding to delTrieNodeStack[i+1])
    uint8_t delPathStack[4096];
    //this index will tell us in wich part of the stack we should insert.
    int delTrieNodeIndex;

    void init(uint16_t N1v,uint16_t Ntv, double alphav){
        N1=N1v;
        Nt=Ntv;
        alpha=alphav;

        sizeArray = (uint16_t *) malloc(sizeof(uint16_t)*(Ntv+1));
        for (int i = 0; i <= Ntv; ++i) {
            if (i > N1v) N1v = 4*(((uint16_t)ceil((double)N1v/alphav)+3)/4);
            sizeArray[i] = N1v;
        }

        dummyRootBlockNode.first=0;
        dummyRootBlockNode.second=0;

        NULL_NODE = treeNode((NODE_TYPE)-1, 0);

        changedBlock=false;
        deleting=false;

        delTreeNodeIndex=0;
        delBlockNodeIndex=0;

        delTrieNodeIndex=0;
    }
};

#endif //TREE1V2_BLOCKGLOBALVARS_H
