#ifndef TREE1V2_TRIENODE_H
#define TREE1V2_TRIENODE_H
//this is a recursive data structure. the trieNode has 4 childrens that are accesed using the morton code
//from the level [0,L1) the block is a null pointer (cuz the data structure is not compressed)
//for leves in [L1,maxint), block is a pointer to a treeBlock (the block data structure)
struct trieNode{
    blockGlobalVars *bgv;
    void *block;
    trieNode **children;

    void init(blockGlobalVars* bgvv){
        children = (trieNode **)malloc(4*sizeof(trieNode*));
        children[0] = children[1] = children[2] = children[3] = NULL;
        block = NULL;
        bgv=bgvv;
    }

    void freeTrieNode();
};



void trieNode::freeTrieNode(){
    free((trieNode *)children);
    free(this);
}
#endif //TREE1V2_TRIENODE_H
