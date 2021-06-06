#ifndef TREE1V2_GLOBALCONSTANTS_H
#define TREE1V2_GLOBALCONSTANTS_H

//L1 and L2 are leves from where the tree change it forms.
//the nodes in the leves [0,L1) are trieNodes.
//nodes in [L1,l2] are TreeBlocks of a medium size
//nodes in (L2.maxint) are TreeBlocks of a larger size
//we make this, because we need to hava some equilibrium, the first nodes are not compress because
//the are accesed very often. And for the treeBlocks we have two diferent max sizes because the medium
//node are accessed more frequently and rank the child isnt cheap.
#define L1 8
#define L2 16


//these are the sizes for the diferent type of blocks
//S1 = for nodes in [0,L1)
//S2 = for nodes in [L1,L2]
//S3 = for nodes in (L2,maxint]
#define S1 64
#define S2 128
#define S3 1024

// position in the dfuds
#define NODE_TYPE uint16_t
// any postition in the dfuds, contains an uint16_t, that represent 4 nodes of 4 bits. the OFFSET_TYPE, represent wich of the 4 bits represent the node
#define OFFSET_TYPE uint8_t
//max unsigned int of 16 bits, is used to limit the dfuds
#define MAX_UINT_16 65535

#endif //TREE1V2_GLOBALCONSTANTS_H
