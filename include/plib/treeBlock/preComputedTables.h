#ifndef TREE1V2_PRECOMPUTEDTABLES_H
#define TREE1V2_PRECOMPUTEDTABLES_H

/*This file contains precomputed tables that help with the bit manipulation*/

/* this table is used to right Shitf >> bits for the OFFSET_TYPE
for example if you have this node with its 4 childs
0000 1111 0000 0000
you want to shift 8 times to the left (shiftT[1]) to recover the node
0000 0000 0000 1111 */
const uint16_t shiftT[4] = {12,8,4,0};

/*initial mask to obtain the binary code of a node according to its offset within a short int
 for example if you have a node 11001010100001101 and you want the fourth children, you want
 maskInitT[3]=0000000000001111*/
const uint16_t maskInitT[4] = {0xf000,0x0f00,0x00f0,0x000f};

/* number of childrens of a node. for example 9=1001 has 2 childrens because it has two 1 */
const uint8_t nChildrenT[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};


/*given a node and a symbol, this table says how many subtrees of the given node must be skipped to obtain the corresponding child subtree
for example, for 1100, and 2 = childSkipT[12,2], you have to skip 2 nodes to insert in 2, because you have two 1 in the left side*/


const int8_t childSkipT[16][4] = {
/*0000*/ { 0,  0,  0,  0},
/*0001*/ { 0,  0,  0,  0},
/*0010*/ { 0,  0,  0,  1},
/*0011*/ { 0,  0,  0,  1},
/*0100*/ { 0,  0,  1,  1},
/*0101*/ { 0,  0,  1,  1},
/*0110*/ { 0,  0,  1,  2},
/*0111*/ { 0,  0,  1,  2},
/*1000*/ { 0,  1,  1,  1},
/*1001*/ { 0,  1,  1,  1},
/*1010*/ { 0,  1,  1,  2},
/*1011*/ { 0,  1,  1,  2},
/*1100*/ { 0,  1,  2,  2},
/*1101*/ { 0,  1,  2,  2},
/*1110*/ { 0,  1,  2,  3},
/*1111*/ { 0,  1,  2,  3}
};


/*Given a node and a symbol, gives the rank of the symbol within the children of the node.
If the children does not exist, returns -1*/

const int8_t childT[16][4] = {
/*0000*/ {-1, -1, -1, -1},
/*0001*/ {-1, -1, -1,  1},
/*0010*/ {-1, -1,  1, -1},
/*0011*/ {-1, -1,  1,  2},
/*0100*/ {-1,  1, -1, -1},
/*0101*/ {-1,  1, -1,  2},
/*0110*/ {-1,  1,  2, -1},
/*0111*/ {-1,  1,  2,  3},
/*1000*/ { 1, -1, -1, -1},
/*1001*/ { 1, -1, -1,  2},
/*1010*/ { 1, -1,  2, -1},
/*1011*/ { 1, -1,  2,  3},
/*1100*/ { 1,  2, -1, -1},
/*1101*/ { 1,  2, -1,  3},
/*1110*/ { 1,  2,  3, -1},
/*1111*/ { 1,  2,  3,  4}
};

/* given a node in bits (example 1010) and a number(0,1,2,3), give us how the node will be after inserting in that postion
for example insertT[1010][3]=insertT[10][3]=1011 and insertT[1010][0]=insertT[10][0]=1010 */
const int8_t insertT[16][4] = {
/*0000*/ {0x8, 0x4, 0x2, 0x1},
/*0001*/ {0x9, 0x5, 0x3, 0x1},
/*0010*/ {0xa, 0x6, 0x2, 0x3},
/*0011*/ {0xb, 0x7, 0x3, 0x3},
/*0100*/ {0xc, 0x4, 0x6, 0x5},
/*0101*/ {0xd, 0x5, 0x7, 0x5},
/*0110*/ {0xe, 0x6, 0x6, 0x7},
/*0111*/ {0xf, 0x7, 0x7, 0x7},
/*1000*/ {0x8, 0xc, 0xa, 0x9},
/*1001*/ {0x9, 0xd, 0xb, 0x9},
/*1010*/ {0xa, 0xe, 0xa, 0xb},
/*1011*/ {0xb, 0xf, 0xb, 0xb},
/*1100*/ {0xc, 0xc, 0xe, 0xd},
/*1101*/ {0xd, 0xd, 0xf, 0xd},
/*1110*/ {0xe, 0xe, 0xe, 0xf},
/*1111*/ {0xf, 0xf, 0xf, 0xf}
};



const uint16_t delNodeInBlockT[4][4] = {
/*xfff*/ {0x7fff, 0xbfff, 0xdfff, 0xefff},
/*fxff*/ {0xf7ff, 0xfbff, 0xfdff, 0xfeff},
/*ffxf*/ {0xff7f, 0xffbf, 0xffdf, 0xffef},
/*fffx*/ {0xfff7, 0xfffb, 0xfffd, 0xfffe},
};


// given a symbol in {0, 1, 2, 3}, yields a unary node representing that symbol
const int8_t symbol2NodeT[4] = {0x8,0x4,0x2,0x1};

#endif //TREE1V2_PRECOMPUTEDTABLES_H
