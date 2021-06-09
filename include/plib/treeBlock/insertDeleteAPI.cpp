
/*this function consume the string that represent the morton code, until the path it represent finished
finally call a function that insert the new path in a block*/
void insertar(treeBlock *root, uint8_t *str, uint64_t length, uint16_t level, uint16_t maxDepth){

    //curBlock is pointing to the root because we will start in that point. curBlockAux is a pointer we will use to decend the tree
    treeBlock *curBlock = root, *curBlockAux;
    //set a dummy treeNode, and an aux pointer to descend the tree
    treeNode curNode(0,0), curNodeAux;
    //dummy flag
    uint16_t curFlag = 0;

    //for every char of the string (the morton code) until we find the part were the path does not exist
    uint64_t i;
    for (i = 0; i < length; ++i) {

        //we get the child using the morton code char (str[i])
        curBlockAux = curBlock;
        curNodeAux = curBlock->child(curBlock, curNode, str[i], level, maxDepth, curFlag);

        //if the child function returned a treeNode with a -1, that means that the path does not exist. so we break here.
        if (curNodeAux.first == (NODE_TYPE)-1) {
            break;
        }
        else {
            //else, we update the curNode
            curNode = curNodeAux;
        }

        //if we are in a fronteir node
        if (curBlock->nPtrs > 0 && absolutePosition(curNode) == curFlag) {
            // we go to the block where the flag is pointing
            curBlock = curBlock->getPointer(curFlag);
            //and reset the treeNode (because we will start in a new block)
            curNode.first = 0;
            curNode.second = 0;
        }
    }

    /*
    the for cycle will stop at some i where the path does not exist
    so we have to inserts str[i..length-1] starting from the current node
    The new nodes inserted will descend from curNode*/
    curBlock->insert(curNode, &str[i], length-i, level, maxDepth, curFlag);
}


//this will create and insert the respective nodes in the trie and in the treeBlocks
void insertTrie(trieNode *t, uint8_t *str, uint64_t length, uint16_t maxDepth){
    blockGlobalVars * bgv=t->bgv;
    //this will consume the string until a children does not exist.
    //this will leave i as the position in str from where the path does not exist or if the path continue in a treeBlock
    uint64_t i = 0;
    while (t->children[str[i]]){
        t = t->children[str[i++]];
    }


    //if i<L1 we insert it uncompressed
    while (i < L1) {
        //create a new trieNode
        t->children[str[i]] = (trieNode *)malloc(sizeof(trieNode));
        t = t->children[str[i]];
        t->init(bgv);
        //go to next i
        i++;
    }


    //border case: we have that t (with i>L) is null (because that block was create recently)
    // this always pass when the previous while stop when i=L1
    treeBlock *p;
    p = (treeBlock *) t->block;
    if (t->block == NULL) {
        //we create the treeBlock
        t->block = malloc(sizeof(treeBlock));
        p = (treeBlock*)t->block;
        p->dfuds = (uint16_t *)calloc(2, sizeof(uint16_t));
        p->rootDepth = L1;
        p->nNodes = 1;
        p->ptr = NULL;
        p->nPtrs = 0;
        p->maxNodes = 4;
        p->bgv=bgv;
    }else{
        //if the block exist, we assign it to p
        p = (treeBlock *) t->block;
    }

    //we call insertar, that will insert the remaining string str[i...length(str)}
    //in treeBlocks
    insertar(p, &str[i], length-i, i, maxDepth);
}





//return true if the string str is a path in a block
bool isEdge(treeBlock *root, uint8_t *str, uint64_t length, uint16_t level, uint16_t maxDepth){

    //curBlock is pointing to the root because we will start in that point. curBlockAux is a pointer we will use to decend the tree
    treeBlock *curBlock = root, *curBlockAux;
    //set a dummy treeNode, and an aux pointer to descend the tree
    treeNode curNode(0,0), curNodeAux;
    //dummy flag
    uint16_t curFlag = 0;

    //for each char in the string
    uint64_t i;
    for (i = 0; i < length; ++i) {
        //we will get the child in function of the simbol str[i}
        curBlockAux = curBlock;
        curNodeAux = curBlock->child(curBlock, curNode, str[i], level, maxDepth, curFlag);

        //if the child is a -1, that means that the path does not exist, so we break
        if (curNodeAux.first == (NODE_TYPE)-1){
            break;
        }
        else {
            //if not, we continue
            curNode = curNodeAux;
        }

        //if we are in a frontier node, we need to go down to the corresponding block
        if (curBlock->nPtrs > 0 && absolutePosition(curNode) == curFlag) {
            // Goes down to a child block
            curBlock = curBlock->getPointer(curFlag);
            curNode.first = 0;
            curNode.second = 0;
        }
    }

    /*finally we return i==length that means: if we broke the previous cycle, the path does not exist*/
    return length == i;
}

//return true if the path represented with the str, is in the trie (and in the tree)
bool isEdgeTrie(trieNode *t, uint8_t *str, uint64_t length, uint16_t maxDepth) {

    //this will consume the string until a children does not exist.
    //this will leave i as the position in str from where the path does not exist or if the path continue in a treeBlock
    uint64_t i = 0;
    while (t->children[str[i]] && !t->block) {
        t = t->children[str[i++]];
    }

    /*instantiate p as the block of t (if it does not exist, then return false, but if exist, that means that exist in a treeBlock so we call
    isEdge to continue searching in the blocks)*/
    treeBlock *p= (treeBlock *) t->block;
    if (p) {
        return isEdge(p, &str[i], length - i, i, maxDepth);
    } else {
        return false;
    }
}


//return the size of the whole trie
uint64_t sizeTrie(trieNode *t){
    //base case: if the pointer is null return 0
    if (!t) {
        return 0;
    }

    //if we use up to 8 levels of pointers. the pointers can implemented with 16 bits
    //this is the size of the actual trie
    uint64_t totalSize = 4*sizeof(uint16_t);

    //if we are not a treeBlock
    if (!t->block) {
        //we sum the the size of the children tries
        totalSize += sizeTrie(t->children[0]);
        totalSize += sizeTrie(t->children[1]);
        totalSize += sizeTrie(t->children[2]);
        totalSize += sizeTrie(t->children[3]);
    }else{
        //if we are a treeBlock, we sum the size of the treeBlock
        totalSize += ((treeBlock *)t->block)->size();
    }
    //return the total sum
    return totalSize;
}


void babToBin(uint16_t a){
    uint16_t n=a;
    uint16_t mask=0x8000;
    uint16_t mask2=0x7fff;
    int c=16;
    while (c) {
        if (n & mask) {
            printf("1");
        }else {
            printf("0");
        }
        n = n&mask2;
        mask >>=1;
        mask2 >>=1;
        c--;
    }
    printf("\n");

}

void printDfuds(uint16_t *A,int n){
    printf("dfuds");
    for(int i=0;i<n;i++){
        babToBin(A[i]);
    }
    printf("\n");
}

void f(uint16_t a){
    uint16_t n=a;
    uint16_t mask=0x8000;
    uint16_t mask2=0x7fff;
    int c=16;
    while (c) {
        if (n & mask) {
            printf("1");
        }else {
            printf("0");
        }
        n = n&mask2;
        mask >>=1;
        mask2 >>=1;
        c--;
    }

}


void printDfuds2(uint16_t *A,int n){
    printf("dfuds\n");
    for(int i=0;i<n;i++){
        printf(" ");
        f(A[i]);
    }
    printf("\n");
}


uint16_t table[4] = {0x0fff, 0xf0ff, 0xff0f, 0xfff0};

void deleteZeros(uint16_t * A,int n){
    int zeroIndex = 0;
    for (int actualIndex = 0; actualIndex < 4 * n; actualIndex++){
        uint16_t actualNumber = A[actualIndex / 4];
        actualNumber <<= (actualIndex % 4) * 4;
        actualNumber >>= 3 * 4;
        if (actualNumber != 0)
        {
            uint16_t aux = actualNumber;
            uint16_t zeroAux = A[zeroIndex / 4];
            zeroAux <<= (zeroIndex % 4) * 4;
            zeroAux >>= 3 * 4;

            A[actualIndex / 4] = A[actualIndex / 4] & table[actualIndex % 4] | (zeroAux << (4 * (3 - (actualIndex % 4))));
            A[zeroIndex / 4] = A[zeroIndex / 4] & table[zeroIndex % 4] | (aux << (4 * (3 - (zeroIndex % 4))));
            zeroIndex++;
        }
    }
}

void shift1(uint16_t *A, int n, int i, int j){
    int actualPos = 4 * i + j;
    for (int i2 = actualPos; i2 < (4 * n) - 1; i2++)
    {
        int newPos = i2 + 1;
        uint16_t newaux = A[newPos/4];
        newaux=newaux>>shiftT[newPos%4]&0x000f;
        newaux=newaux<<shiftT[i2%4];

        A[i2/4]=A[i2/4]&table[i2%4]|newaux;
    }
    A[n-1]= A[n-1]&0xfff0;
}

void deleteZeros2(uint16_t *A,int n){

    int c = 4 * n;
    int i = 0;
    while (c){
        uint16_t aux=A[i/4];
        aux=aux>>shiftT[i%4]&0x000f;
        if (aux == 0){
            shift1(A, n, i / 4, i % 4);
        }
        else{
            i++;
        }
        c--;
    }
}

void printTreePathStack(uint8_t *st,int index){
    printf("pathStackIndex = %u\n",index);
    printf("pathStack =\n");
    for(int i=0;i<index;i++){
        printf("%u ",st[i]);
    }
    printf("\n");
}

void printNodesInBlockStack(uint8_t *st,int index){
    printf("NodesInBlockStack index = %u\n",index);
    printf("printNodesInBlockStack =\n");
    for(int i=0;i<=index;i++){
        printf("%u ",st[i]);
    }
    printf("\n");
}

void blockInspectFrontiers(treeBlock *root){
    if(root==NULL){
        return;
    }
    if(root->nPtrs==0){
        return;
    }
    for(int k=0;k<root->nPtrs;k++){
        printf("%u ",((blockPtr *)root->ptr)[k].flag);
    }
    printf("\n");
    for(int k=0;k<root->nPtrs;k++){
        blockInspectFrontiers(((blockPtr *)root->ptr)[k].P);
    }



}

void trieInspectFrontiers(trieNode *t){
    if(t==NULL){
        return;
    }
    trieInspectFrontiers(t->children[0]);
    trieInspectFrontiers(t->children[1]);
    trieInspectFrontiers(t->children[2]);
    trieInspectFrontiers(t->children[3]);
    blockInspectFrontiers((treeBlock*)t->block);
}



void deleteZeros3(uint16_t *A,int n){
    int c = 4 * n;
    int i = 0;
    while (c){
        uint16_t aux=A[i/4];
        aux=aux>>shiftT[i%4]&0x000f;
        if (aux == 0){
            shift1(A, n, i / 4, i % 4);
        }
        else{
            i++;
        }
        c--;
    }
}




void unionBlocks(treeBlock * father, treeBlock * son,uint16_t flag,uint16_t flagIndex){
    //join dfuds
    //first grow the father

    father->grow(son->nNodes-1);

    for(int k=father->nNodes;k<father->maxNodes;k++){
        int i=k/4;
        int j=k%4;
        father->dfuds[i]=father->dfuds[i]&table[j];
    }



    //move all the nodes from flag+1 till the end, son->nNodes-1 Spaces
    for(int k=father->nNodes-1;k>flag;k--){
        //father->dfuds[k+son->nNodes-1]=father->dfuds[k];
        uint16_t iactual;
        uint16_t jactual;

        iactual=k/4;
        jactual=k%4;

        uint16_t actual=father->dfuds[iactual];
        actual=actual&maskInitT[jactual];
        actual=actual>>4*(3-jactual);

        uint16_t inew;
        uint16_t jnew;

        inew=(k+son->nNodes-1)/4;
        jnew=(k+son->nNodes-1)%4;

        uint16_t newD=father->dfuds[inew];
        newD=newD&table[jnew];
        actual=actual<<4*(3-jnew);
        newD=newD|actual;

        father->dfuds[inew]=newD;
    }

    father->nNodes+=son->nNodes-1;
    //insert the dfuds of the son in dfuds of the father
    for(int k=0;k<son->nNodes;k++){
        //father->dfuds[flag+k]=son->dfuds[k];
        uint16_t iDson;
        uint16_t jDson;

        iDson=k/4;
        jDson=k%4;

        uint16_t dson=son->dfuds[iDson];
        dson=dson&maskInitT[jDson];
        dson=dson>>4*(3-jDson);

        uint16_t iDfather;
        uint16_t jDfather;

        iDfather=(flag+k)/4;
        jDfather=(flag+k)%4;

        uint16_t dfather=father->dfuds[iDfather];
        dfather=dfather&table[jDfather];
        dson=dson<<4*(3-jDfather);
        dfather=dfather|dson;

        father->dfuds[iDfather]=dfather;
    }



    //join ptr
    //first need to find the index in father->ptr where flag=flag

    if(son->nPtrs>0){

        //if son has frontier
        //first grow the father frontier to absorb the son frontier
        uint16_t fatherPtrs=father->nPtrs;

        father->nPtrs+=son->nPtrs-1;
        father->ptr = realloc(father->ptr, sizeof(blockPtr)*(father->nPtrs));

        //move all the blockPtrs from the one that has flag=flag, sonNptrs spaces
        for(int k=(father->nPtrs-1);k>flagIndex;k--){
            ((blockPtr *)father->ptr)[k]=((blockPtr *)father->ptr)[k-(son->nPtrs-1)];
            ((blockPtr *)father->ptr)[k].flag+=son->nNodes-1;
        }


        //now insert son->ptr in father->ptr
        for(int k=0;k<son->nPtrs;k++){
            ((blockPtr *)father->ptr)[flagIndex+k]=((blockPtr *)son->ptr)[k];
            ((blockPtr *)father->ptr)[flagIndex+k].flag+=flag;
        }

        //we free son dfuds, but not the frointier, because the father will use it
        son->freeTreeBlockButKeepFrontier();
    }else{
        //if son has no frontier, we should delete flagIndex pointer and realloc

        for(int k=flagIndex; k<father->nPtrs-1;k++){
            ((blockPtr *)father->ptr)[k]=((blockPtr *)father->ptr)[k+1];
            ((blockPtr *)father->ptr)[k].flag+=son->nNodes-1;
        }

        father->nPtrs--;
        if(father->nPtrs==0){
            father->ptr=NULL;
        }else {
            father->ptr = realloc(father->ptr, sizeof(blockPtr) * (father->nPtrs));
        }

        son->freeTreeBlock();
    }

}



bool deleteBlockNodes(treeBlock *root, uint8_t str[], uint64_t length, uint16_t level,uint64_t maxDepth){
    //in the first part we insert nodes in the stack, so first we fill path stack
    //curBlock is pointing to the root because we will start in that point. curBlockAux is a pointer we will use to decend the tree
    treeBlock *curBlock = root, *curBlockAux;
    //set a dummy treeNode, and an aux pointer to descend the tree
    treeNode curNode(0,0), curNodeAux,dummyNode(0,0);
    //dummy flag
    uint16_t curFlag = 0;
    //set the first block and node in the respective stacks
    root->bgv->delTreeNodeStack[root->bgv->delTreeNodeIndex]=curNode;
    root->bgv->delBlockStack[root->bgv->delBlockNodeIndex]=curBlock;
    root->bgv->nodesInBlockStack[root->bgv->delBlockNodeIndex]=1;
    //if we descend from one block to another, this flag will store the position of the second block in the first block
    uint16_t lastFlag;

    //traverse the string (the path) until we reach the final node or we dont find a path
    uint64_t i;
    for (i = 0; i < length; ++i) {
        //put the char in the char stack
        root->bgv->delTreePathStack[root->bgv->delTreeNodeIndex]=str[i];
        //if we descend in a block, we want to store the last flag before it change
        lastFlag=curFlag;
        //we get the child using the morton code char (str[i])
        curNodeAux = curBlock->child(curBlock, curNode, str[i], level, maxDepth, curFlag);
        //if the child function returned a treeNode with a -1, that means that the path does not exist. so we break here.
        if (curNodeAux.first == (NODE_TYPE)-1) {
            //reset the indexes
            root->bgv->delTreeNodeIndex=0;
            root->bgv->delBlockNodeIndex=0;
            return false;
        }

        //we update the curNode
        curNode = curNodeAux;
        //if the block didnt changed
        if(root->bgv->changedBlock==0){
            root->bgv->delTreeNodeIndex++;
            root->bgv->delTreeNodeStack[root->bgv->delTreeNodeIndex]=curNode;
            root->bgv->nodesInBlockStack[root->bgv->delBlockNodeIndex]++;
        }else{
            //if the block changed (that means we are in a frontier)
            root->bgv->changedBlock=0;
            root->bgv->flagPathBlockStack[root->bgv->delBlockNodeIndex]=lastFlag;
            root->bgv->delBlockNodeIndex++;
            root->bgv->delBlockStack[root->bgv->delBlockNodeIndex]=curBlock;

            root->bgv->delTreeNodeIndex++;
            root->bgv->delTreeNodeStack[root->bgv->delTreeNodeIndex]=dummyNode;
            root->bgv->delTreePathStack[root->bgv->delTreeNodeIndex]=str[i];

            root->bgv->delTreeNodeIndex++;
            root->bgv->delTreeNodeStack[root->bgv->delTreeNodeIndex]=curNode;
            root->bgv->nodesInBlockStack[root->bgv->delBlockNodeIndex]=2;
        }
    }
    //substract one int the delTreeStackIndex because the last for cycle add an extra one
    root->bgv->nodesInBlockStack[root->bgv->delBlockNodeIndex]--;
    root->bgv->delTreeNodeIndex--;



    //now we traverse the stack backward deleting the nodes till a path fork
    //this variable will tell us if we need to delete the last block traversed
    bool deleteBlock=false;
    //first iter for each block in the stack root->bgv->delBlockStack
    for(int i=root->bgv->delBlockNodeIndex;i>=0;i--){
        treeBlock *actualBlock;
        if (deleteBlock){

            //in the actualBlock we should delete the last block
            deleteBlock=false;
            actualBlock->freeTreeBlock();
            actualBlock=NULL;
            actualBlock=(treeBlock*)root->bgv->delBlockStack[i];
            actualBlock->nPtrs--;
            if(actualBlock->nPtrs==0){
                actualBlock->ptr=NULL;
            }else{
                for(uint16_t k=root->bgv->flagPathBlockStack[i];k<actualBlock->nPtrs;k++){
                    ((blockPtr*)actualBlock->ptr)[k]= ((blockPtr *)actualBlock->ptr)[k+1];
                }
                actualBlock->ptr = realloc(actualBlock->ptr, sizeof(blockPtr)*(actualBlock->nPtrs));
            }
        }else{
            actualBlock=(treeBlock*)root->bgv->delBlockStack[i];
        }

        //we iter for each node in each block
        for(int j=0;j<root->bgv->nodesInBlockStack[i];j++){

            //get the actual char in the path (from bottom to top, dont forget it)
            uint8_t actualChar=root->bgv->delTreePathStack[root->bgv->delTreeNodeIndex];
            //get the actual node in the path
            treeNode actualNode=root->bgv->delTreeNodeStack[root->bgv->delTreeNodeIndex];

            //now we delete the node in the block
            uint16_t auxFirst=actualBlock->dfuds[actualNode.first];
            auxFirst=auxFirst & delNodeInBlockT[actualNode.second][actualChar];
            actualBlock->dfuds[actualNode.first]=auxFirst;
            //get the node we update to check if is 0
            uint16_t auxSecond=actualBlock->dfuds[actualNode.first];
            auxSecond=(auxSecond>>shiftT[actualNode.second]) & 0x000f;
            if(auxSecond==0){
                //actualize frontier (basicaly substract 1 from every flag in the frointier grater than the updated node
                //todo: talvez esto podria optimizarse
                for(uint16_t k=0;k<actualBlock->nPtrs;k++){
                    if(((blockPtr*)actualBlock->ptr)[k].flag>(actualNode.first*4+actualNode.second)){
                        ((blockPtr*)actualBlock->ptr)[k].flag--;
                    }
                }
                //substract 1 node from nNodes and delete the zeros
                actualBlock->nNodes--;
            }else if(!(actualNode.first==0 && actualNode.second==0 && i!=0)){
                //if auxFirst !=0 that means that the path was forked so we return false
                root->bgv->delTreeNodeIndex=0;
                root->bgv->delBlockNodeIndex=0;
                deleteZeros(actualBlock->dfuds,actualBlock->maxNodes/4);
                actualBlock->shrink2();
                return false;
            }
            root->bgv->delTreeNodeIndex--;
        }
        deleteZeros(actualBlock->dfuds,actualBlock->maxNodes/4);

        actualBlock->shrink2();

        if(actualBlock->nNodes==0){
            //is time to delete the block
            deleteBlock=true;
        }
    }

    root->bgv->delTreeNodeIndex=0;
    root->bgv->delBlockNodeIndex=0;
    return true;
}



bool deleteBlockNodes2(treeBlock *root, uint8_t str[], uint64_t length, uint16_t level,uint64_t maxDepth){
    //in the first part we insert nodes in the stack, so first we fill path stack
    //curBlock is pointing to the root because we will start in that point. curBlockAux is a pointer we will use to decend the tree
    treeBlock *curBlock = root, *curBlockAux;
    //set a dummy treeNode, and an aux pointer to descend the tree
    treeNode curNode(0,0), curNodeAux,dummyNode(0,0);
    //dummy flag
    uint16_t curFlag = 0;
    //set the first block and node in the respective stacks
    root->bgv->delTreeNodeStack[root->bgv->delTreeNodeIndex]=curNode;
    root->bgv->delBlockStack[root->bgv->delBlockNodeIndex]=curBlock;
    root->bgv->nodesInBlockStack[root->bgv->delBlockNodeIndex]=1;
    //if we descend from one block to another, this flag will store the position of the second block in the first block
    uint16_t lastFlag;

    //traverse the string (the path) until we reach the final node or we dont find a path
    uint64_t i;
    for (i = 0; i < length; ++i) {
        //put the char in the char stack
        root->bgv->delTreePathStack[root->bgv->delTreeNodeIndex]=str[i];
        //if we descend in a block, we want to store the last flag before it change
        lastFlag=curFlag;
        //we get the child using the morton code char (str[i])
        curNodeAux = curBlock->child(curBlock, curNode, str[i], level, maxDepth, curFlag);
        //if the child function returned a treeNode with a -1, that means that the path does not exist. so we break here.
        if (curNodeAux.first == (NODE_TYPE)-1) {
            //reset the indexes
            root->bgv->delTreeNodeIndex=0;
            root->bgv->delBlockNodeIndex=0;
            return false;
        }

        //we update the curNode
        curNode = curNodeAux;
        //if the block didnt changed
        if(root->bgv->changedBlock==0){
            root->bgv->delTreeNodeIndex++;
            root->bgv->delTreeNodeStack[root->bgv->delTreeNodeIndex]=curNode;
            root->bgv->nodesInBlockStack[root->bgv->delBlockNodeIndex]++;
        }else{
            //if the block changed (that means we are in a frontier)
            root->bgv->changedBlock=0;

            root->bgv->flagPathBlockStack[root->bgv->delBlockNodeIndex]=lastFlag;
            root->bgv->delBlockNodeIndex++;
            root->bgv->delBlockStack[root->bgv->delBlockNodeIndex]=curBlock;

            root->bgv->delTreeNodeIndex++;
            root->bgv->delTreeNodeStack[root->bgv->delTreeNodeIndex]=dummyNode;
            root->bgv->delTreePathStack[root->bgv->delTreeNodeIndex]=str[i];

            root->bgv->delTreeNodeIndex++;
            root->bgv->delTreeNodeStack[root->bgv->delTreeNodeIndex]=curNode;
            root->bgv->nodesInBlockStack[root->bgv->delBlockNodeIndex]=2;
        }
    }
    //substract one int the delTreeStackIndex because the last for cycle add an extra one
    root->bgv->nodesInBlockStack[root->bgv->delBlockNodeIndex]--;
    root->bgv->delTreeNodeIndex--;




    //now we traverse the stack backward deleting the nodes till a path fork
    //this variable will tell us if we need to delete the last block traversed
    bool deleteBlock=false;
    bool unionBlock=false;
    uint16_t beta=64;
    //first iter for each block in the stack root->bgv->delBlockStack
    for(int i=root->bgv->delBlockNodeIndex;i>=0;i--){

        treeBlock *actualBlock;
        if (deleteBlock){
            //in the actualBlock we should delete the last block
            deleteBlock=false;
            actualBlock->freeTreeBlock();
            actualBlock=NULL;
            actualBlock=(treeBlock*)root->bgv->delBlockStack[i];

            actualBlock->nPtrs--;
            if(actualBlock->nPtrs==0){
                actualBlock->ptr=NULL;
            }else{
                for(uint16_t k=root->bgv->flagPathBlockStack[i];k<actualBlock->nPtrs;k++){
                    ((blockPtr*)actualBlock->ptr)[k]= ((blockPtr *)actualBlock->ptr)[k+1];
                }
                actualBlock->ptr = realloc(actualBlock->ptr, sizeof(blockPtr)*(actualBlock->nPtrs));
            }



        }else if(unionBlock){
            unionBlock=false;
        }else{
            actualBlock=(treeBlock*)root->bgv->delBlockStack[i];
        }


        //we iter for each node in each block
        for(int j=0;j<root->bgv->nodesInBlockStack[i];j++){
            //get the actual char in the path (from bottom to top, dont forget it)
            uint8_t actualChar=root->bgv->delTreePathStack[root->bgv->delTreeNodeIndex];
            //get the actual node in the path
            treeNode actualNode=root->bgv->delTreeNodeStack[root->bgv->delTreeNodeIndex];
            //now we delete the node in the block
            uint16_t auxFirst=actualBlock->dfuds[actualNode.first];
            auxFirst=auxFirst & delNodeInBlockT[actualNode.second][actualChar];
            actualBlock->dfuds[actualNode.first]=auxFirst;

            //get the node we update to check if is 0
            uint16_t auxSecond=actualBlock->dfuds[actualNode.first];
            auxSecond=(auxSecond>>shiftT[actualNode.second]) & 0x000f;
            if(auxSecond==0){
                //actualize frontier (basicaly substract 1 from every flag in the frointier grater than the updated node
                //todo: talvez esto podria optimizarse
                for(uint16_t k=0;k<actualBlock->nPtrs;k++){
                    if(((blockPtr*)actualBlock->ptr)[k].flag>(actualNode.first*4+actualNode.second)){
                        ((blockPtr*)actualBlock->ptr)[k].flag--;
                    }
                }
                //substract 1 node from nNodes and delete the zeros
                actualBlock->nNodes--;
            }else if(!(actualNode.first==0 && actualNode.second==0 && i!=0)){
                //if auxFirst !=0 that means that the path was forked so we return false
                root->bgv->delTreeNodeIndex=0;
                root->bgv->delBlockNodeIndex=0;
                deleteZeros(actualBlock->dfuds,actualBlock->maxNodes/4);
                actualBlock->shrink2();
                if(i>0 && (actualBlock->nNodes+((treeBlock*)root->bgv->delBlockStack[i-1])->nNodes)<=beta){
                    unionBlocks((treeBlock*)root->bgv->delBlockStack[i-1],actualBlock, ((blockPtr*) ((treeBlock*)root->bgv->delBlockStack[i-1])->ptr)[root->bgv->flagPathBlockStack[i-1]].flag , root->bgv->flagPathBlockStack[i-1]);
                    i--;
                }

                while(i>0 && (((treeBlock*)root->bgv->delBlockStack[i])->nNodes+((treeBlock*)root->bgv->delBlockStack[i-1])->nNodes)<=beta){
                    unionBlocks((treeBlock*)root->bgv->delBlockStack[i-1],(treeBlock*)root->bgv->delBlockStack[i],((blockPtr*)((treeBlock*)root->bgv->delBlockStack[i-1])->ptr)[root->bgv->flagPathBlockStack[i-1]].flag , root->bgv->flagPathBlockStack[i-1]);
                    i--;
                }

                return false;
            }
            root->bgv->delTreeNodeIndex--;
        }

        deleteZeros(actualBlock->dfuds,actualBlock->maxNodes/4);
        actualBlock->shrink2();

        if(actualBlock->nNodes==0){
            //is time to delete the block
            deleteBlock=true;
        }
    }

    root->bgv->delTreeNodeIndex=0;
    root->bgv->delBlockNodeIndex=0;
    return true;
}

/*this will delete the path of the edge
 first we insert the trie nodes on the stack. and trigger deleteBlockNodes that will delete
 the nodes in a bottom up strategy*/
void deleteTrie(trieNode *t, uint8_t *str, uint64_t length, uint16_t maxDepth){

    //insert t in the stack
    t->bgv->delTrieNodeStack[t->bgv->delTrieNodeIndex]=t;

    //this will consume the string inserting the nodes of the path in the stack
    //if a node does not exist, that means that the path does not exist so we return
    uint64_t i ;
    for(i=0;i<L1;i++){
        if (t->children[str[i]]){
            //add the char to the char stack
            t->bgv->delPathStack[t->bgv->delTrieNodeIndex]=str[i];
            //we actualize t->bgv->delTrieNodeIndex because there is an offset of 1 between delPathStack and t->bgv->delTrieNodeStack
            t->bgv->delTrieNodeIndex++;
            //add the trieNode to the stack
            t->bgv->delTrieNodeStack[t->bgv->delTrieNodeIndex]=t->children[str[i]];
            //go to next node
            t=t->children[str[i]];
        }else{
            //the path does not exist, so there is nothing to delete
            t->bgv->delTrieNodeIndex=0;
            return;
        }
    }

    //if t->block==NULL that means that the path does not exist
    treeBlock *p;
    if (t->block == NULL) {
        //the path does not exist, so there is nothing to delete
        t->bgv->delTrieNodeIndex=0;
        return;
    }else{
        //if the block exist, we assign it to p
        p = (treeBlock *) t->block;
    }

    //we call deleteBlockNodes, that delete the remaining path str[i...length(str)] in treeBlocks
    t->bgv->deleting=true;
    bool continueDelete=deleteBlockNodes(p, &str[i], length-i, i, maxDepth);
    t->bgv->deleting=false;

    if(continueDelete){
        //if continueDelete is true, that means that we should check the t->bgv->delTrieNodeStack to continue deleting until the path is forked
        //get the last trieNode in the stack
        trieNode *taux=(trieNode*)t->bgv->delTrieNodeStack[t->bgv->delTrieNodeIndex];

        //the last trieNode has null childs but not null block, so if continueDelete is true, that means that the block is now null
        treeBlock *paux=(treeBlock *) taux->block;
        paux->freeTreeBlock();
        taux->block=NULL;

        //now go to the next trieNode and delete the last one
        t->bgv->delTrieNodeIndex--;
        taux=(trieNode*)t->bgv->delTrieNodeStack[t->bgv->delTrieNodeIndex];
        //delete all the trieNodes of the stack till the path fork
        while(t->bgv->delTrieNodeIndex>0){
            taux->children[t->bgv->delPathStack[t->bgv->delTrieNodeIndex]]=NULL;
            //this tell us if the path is not forked
            if( taux->children[0]==NULL && taux->children[1]==NULL && taux->children[2]==NULL && taux->children[3]==NULL ){
                t->bgv->delTrieNodeIndex--;
                taux->freeTrieNode();
                taux=(trieNode*)t->bgv->delTrieNodeStack[t->bgv->delTrieNodeIndex];
                taux->children[t->bgv->delPathStack[t->bgv->delTrieNodeIndex]]=NULL;
            }else{
                //the path is forked, so we stop
                t->bgv->delTrieNodeIndex=0;
                return;
            }
        }

        t->bgv->delTrieNodeIndex=0;
        return;

    }else{
        //if continueDelete is false, that means that this part of the path is still in use
        t->bgv->delTrieNodeIndex=0;
        return;
    }
    t->bgv->delTrieNodeIndex=0;
    return;

}


void deleteTrie2(trieNode *t, uint8_t *str, uint64_t length, uint16_t maxDepth){

    //insert t in the stack
    t->bgv->delTrieNodeStack[t->bgv->delTrieNodeIndex]=t;

    //this will consume the string inserting the nodes of the path in the stack
    //if a node does not exist, that means that the path does not exist so we return
    uint64_t i ;
    for(i=0;i<L1;i++){
        if (t->children[str[i]]){
            //add the char to the char stack
            t->bgv->delPathStack[t->bgv->delTrieNodeIndex]=str[i];
            //we actualize t->bgv->delTrieNodeIndex because there is an offset of 1 between delPathStack and delTrieNodeStack
            t->bgv->delTrieNodeIndex++;
            //add the trieNode to the stack
            t->bgv->delTrieNodeStack[t->bgv->delTrieNodeIndex]=t->children[str[i]];
            //go to next node
            t=t->children[str[i]];
        }else{
            //the path does not exist, so there is nothing to delete
            t->bgv->delTrieNodeIndex=0;
            return;
        }
    }

    //if t->block==NULL that means that the path does not exist
    treeBlock *p;
    if (t->block == NULL) {
        //the path does not exist, so there is nothing to delete
        t->bgv->delTrieNodeIndex=0;
        return;
    }else{
        //if the block exist, we assign it to p
        p = (treeBlock *) t->block;
    }

    //we call deleteBlockNodes, that delete the remaining path str[i...length(str)] in treeBlocks
    t->bgv->deleting=true;
    bool continueDelete=deleteBlockNodes2(p, &str[i], length-i, i, maxDepth);
    t->bgv->deleting=false;
    if(continueDelete){
        //if continueDelete is true, that means that we should check the delTrieNodeStack to continue deleting until the path is forked
        //get the last trieNode in the stack
        trieNode *taux=(trieNode*)t->bgv->delTrieNodeStack[t->bgv->delTrieNodeIndex];

        //the last trieNode has null childs but not null block, so if continueDelete is true, that means that the block is now null
        treeBlock *paux=(treeBlock *) taux->block;
        paux->freeTreeBlock();
        taux->block=NULL;

        //now go to the next trieNode and delete the last one
        t->bgv->delTrieNodeIndex--;
        taux=(trieNode*)t->bgv->delTrieNodeStack[t->bgv->delTrieNodeIndex];

        //delete all the trieNodes of the stack till the path fork
        while(t->bgv->delTrieNodeIndex>0){
            taux->children[t->bgv->delPathStack[t->bgv->delTrieNodeIndex]]=NULL;
            //this tell us if the path is not forked
            if( taux->children[0]==NULL && taux->children[1]==NULL && taux->children[2]==NULL && taux->children[3]==NULL ){
                t->bgv->delTrieNodeIndex--;
                taux->freeTrieNode();
                taux=(trieNode*)t->bgv->delTrieNodeStack[t->bgv->delTrieNodeIndex];
                taux->children[t->bgv->delPathStack[t->bgv->delTrieNodeIndex]]=NULL;
            }else{
                //the path is forked, so we stop
                t->bgv->delTrieNodeIndex=0;
                return;
            }

        }
        t->bgv->delTrieNodeIndex=0;
        return;

    }else{
        //if continueDelete is false, that means that this part of the path is still in use
        t->bgv->delTrieNodeIndex=0;
        return;
    }
    t->bgv->delTrieNodeIndex=0;
    return;

}




//,int rleft,int rright,int cleft,int cright,treeNode curNode,uint16_t curFlag)
void getNeighboursBlock(treeBlock *root, int nodeid, uint64_t length, uint16_t level, uint16_t maxDepth,int rleft,int rright,int cleft,int cright,treeNode curNode,uint16_t curFlag,std::vector<int> &neigs) {


    treeBlock *curBlock = root;

    int rmid=(rleft+rright)/2;
    if(rmid==nodeid && rleft==rright){
        neigs.push_back(cright);
        return;
    }else if (nodeid<=rmid){

        treeNode leftAuxNode=curNode;
        treeBlock * leftBlockAux=curBlock;
        treeNode leftcurNodeAux;
        uint16_t leftCurFlag=curFlag;
        uint16_t leftLevel=level;

        leftcurNodeAux = leftBlockAux->child(leftBlockAux, leftAuxNode,0, leftLevel, maxDepth, leftCurFlag);


        if (leftcurNodeAux.first == (NODE_TYPE)-1){

        }else {
            leftAuxNode = leftcurNodeAux;

            if (leftBlockAux->nPtrs > 0 && absolutePosition(leftAuxNode) == leftCurFlag) {
                leftBlockAux = leftBlockAux->getPointer(leftCurFlag);
                leftAuxNode.first = 0;
                leftAuxNode.second = 0;
            }

            int cmid=(cleft+cright)/2;
            getNeighboursBlock(leftBlockAux,nodeid,length-1,leftLevel,maxDepth,rleft,rmid,cleft,cmid,leftAuxNode,leftCurFlag,neigs);
        }

        treeBlock * rightBlockAux=curBlock;
        treeNode rightAuxNode=curNode;
        treeNode rightcurNodeAux;
        uint16_t rightCurFlag=curFlag;
        uint16_t rightLevel=level;

        rightcurNodeAux = rightBlockAux->child(rightBlockAux, rightAuxNode,1, rightLevel, maxDepth, rightCurFlag);


        if (rightcurNodeAux.first == (NODE_TYPE)-1){

        }else {

            rightAuxNode = rightcurNodeAux;
            if (rightBlockAux->nPtrs > 0 && absolutePosition(rightAuxNode) == rightCurFlag) {
                rightBlockAux = rightBlockAux->getPointer(rightCurFlag);
                rightAuxNode.first = 0;
                rightAuxNode.second = 0;
            }

            int cmid=(cleft+cright)/2;
            getNeighboursBlock(rightBlockAux,nodeid,length-1,rightLevel,maxDepth,rleft,rmid,cmid+1,cright,rightAuxNode,rightCurFlag,neigs);
        }
        return;

    }else{
        treeNode leftAuxNode=curNode;
        treeBlock * leftBlockAux=curBlock;
        treeNode leftcurNodeAux;
        uint16_t leftCurFlag=curFlag;
        uint16_t leftLevel=level;

        leftcurNodeAux = leftBlockAux->child(leftBlockAux, leftAuxNode,2, leftLevel, maxDepth, leftCurFlag);


        if (leftcurNodeAux.first == (NODE_TYPE)-1){

        }else {
            leftAuxNode = leftcurNodeAux;

            if (leftBlockAux->nPtrs > 0 && absolutePosition(leftAuxNode) == leftCurFlag) {
                leftBlockAux = leftBlockAux->getPointer(leftCurFlag);
                leftAuxNode.first = 0;
                leftAuxNode.second = 0;
            }

            int cmid=(cleft+cright)/2;
            getNeighboursBlock(leftBlockAux,nodeid,length-1,leftLevel,maxDepth,rmid+1,rright,cleft,cmid,leftAuxNode,leftCurFlag,neigs);
        }


        treeBlock * rightBlockAux=curBlock;
        treeNode rightAuxNode=curNode;
        treeNode rightcurNodeAux;
        uint16_t rightCurFlag=curFlag;
        uint16_t rightLevel=level;

        rightcurNodeAux = rightBlockAux->child(rightBlockAux, rightAuxNode,3, rightLevel, maxDepth, rightCurFlag);


        if (rightcurNodeAux.first == (NODE_TYPE)-1){

        }else {
            rightAuxNode = rightcurNodeAux;

            if (rightBlockAux->nPtrs > 0 && absolutePosition(rightAuxNode) == rightCurFlag) {
                rightBlockAux = rightBlockAux->getPointer(rightCurFlag);
                rightAuxNode.first = 0;
                rightAuxNode.second = 0;
            }

            int cmid=(cleft+cright)/2;
            getNeighboursBlock(rightBlockAux,nodeid,length-1,rightLevel,maxDepth,rmid+1,rright,cmid+1,cright,rightAuxNode,rightCurFlag,neigs);
        }
        return;
    }

    return;


}


//neighbours api: the idea is to retrieve the neighbours of a node x
//the neighbours will be a linked list with all the nodes in the graph that has a 1 in the row x
//that means all the G[i[x]=1
void getNeighboursTrie(trieNode *t, int nodeid, uint64_t length,uint64_t level ,uint16_t maxDepth,int rleft,int rright, int cleft,int cright,std::vector<int> &neigs) {
    if (t==NULL){
        return;
    }else if(t->children[0]==NULL && t->children[1]==NULL && t->children[2]==0 && t->children[3]==0 && t->block==NULL){
        return;
    }else if(t->children[0]==NULL && t->children[1]==NULL && t->children[2]==0 && t->children[3]==0 && t->block!=NULL){
        treeNode curNode(0,0);
        int16_t curFlag=0;
        getNeighboursBlock((treeBlock*)t->block, nodeid, length, level, maxDepth,rleft,rright,cleft,cright,curNode,curFlag,neigs);
    }else{
        int rmid=(rleft+rright)/2;
        if (nodeid>rmid){
            int cmid=(cleft+cright)/2;
            getNeighboursTrie(t->children[2],nodeid,length-1,level+1,maxDepth,rmid+1,rright,cleft,cmid,neigs);
            getNeighboursTrie(t->children[3],nodeid,length-1,level+1,maxDepth,rmid+1,rright,cmid+1,cright,neigs);
            return;
        }else{
            int cmid=(cleft+cright)/2;
            getNeighboursTrie(t->children[0],nodeid,length-1,level+1,maxDepth,rleft,rmid,cleft,cmid,neigs);
            getNeighboursTrie(t->children[1],nodeid,length-1,level+1,maxDepth,rleft,rmid,cmid+1,cright,neigs);
            return;
        }
    }
}


//two intervals. A =[a1,a2] and B=[b1,b2]
//return true if A inter B is not empty
bool intersectIntervals(int a1,int a2,int b1,int b2){
    if(a2<b1 || b2<a1){
        return false;
    }else{
        return true;
    }
}

//two rectangles A with rows ar1 and ar2, columns ac1 and ac2
// B with rows br1, br2 and columns bc1 and bc2
// return true if A and B intersect
bool intersectRectangles(int ar1,int ar2, int ac1, int ac2, int br1, int br2, int bc1, int bc2){
    return intersectIntervals(ar1,ar2,br1,br2) && intersectIntervals(ac1,ac2,bc1,bc2);
}

void rangeQueryBlock(treeBlock *root, int r1,int r2,int c1,int c2, uint64_t length, uint16_t level, uint16_t maxDepth,int rleft,int rright,int cleft,int cright,treeNode curNode,uint16_t curFlag,std::vector< std::vector<int> > &answer){

    if( rleft==rright && cleft==cright && r1==r2 && c1==c2 ){
        std::vector<int> graphNode;
        graphNode.push_back(r1);
        graphNode.push_back(c1);
        answer.push_back(graphNode);
    }else {
        treeBlock *curBlock = root;

        int rmid=(rleft+rright)/2;
        int cmid=(cleft+cright)/2;

        //ask in the first quadrant
        if(intersectRectangles(r1,r2,c1,c2,rleft,rmid,cleft,cmid)){
            treeNode upLeftAuxNode=curNode;
            treeBlock * upLeftBlockAux=curBlock;
            treeNode upLeftcurNodeAux;
            uint16_t upLeftCurFlag=curFlag;
            uint16_t upLeftLevel=level;

            upLeftcurNodeAux = upLeftBlockAux->child(upLeftBlockAux, upLeftAuxNode,0, upLeftLevel, maxDepth, upLeftCurFlag);


            if (upLeftcurNodeAux.first == (NODE_TYPE)-1){

            }else {
                upLeftAuxNode = upLeftcurNodeAux;

                if (upLeftBlockAux->nPtrs > 0 && absolutePosition(upLeftAuxNode) == upLeftCurFlag) {
                    upLeftBlockAux = upLeftBlockAux->getPointer(upLeftCurFlag);
                    upLeftAuxNode.first = 0;
                    upLeftAuxNode.second = 0;
                }

                int cmid=(cleft+cright)/2;
                rangeQueryBlock(upLeftBlockAux,std::max(r1,rleft),std::min(r2,rmid),std::max(c1,cleft),std::min(c2,cmid),length-1,upLeftLevel,maxDepth,rleft,rmid,cleft,cmid,upLeftAuxNode,upLeftCurFlag,answer);
            }
        }

        //ask in the second quadrant
        if(intersectRectangles(r1,r2,c1,c2,rleft,rmid,cmid+1,cright)){
            treeBlock * upRightBlockAux=curBlock;
            treeNode upRightAuxNode=curNode;
            treeNode upRightcurNodeAux;
            uint16_t upRightCurFlag=curFlag;
            uint16_t upRightLevel=level;

            upRightcurNodeAux = upRightBlockAux->child(upRightBlockAux, upRightAuxNode,1, upRightLevel, maxDepth, upRightCurFlag);


            if (upRightcurNodeAux.first == (NODE_TYPE)-1){

            }else {

                upRightAuxNode = upRightcurNodeAux;
                if (upRightBlockAux->nPtrs > 0 && absolutePosition(upRightAuxNode) == upRightCurFlag) {
                    upRightBlockAux = upRightBlockAux->getPointer(upRightCurFlag);
                    upRightAuxNode.first = 0;
                    upRightAuxNode.second = 0;
                }

                int cmid=(cleft+cright)/2;
                rangeQueryBlock(upRightBlockAux,std::max(r1,rleft),std::min(r2,rmid),std::max(c1,cmid+1),std::min(c2,cright),length-1,upRightLevel,maxDepth,rleft,rmid,cmid+1,cright,upRightAuxNode,upRightCurFlag,answer);
            }

        }

        //ask in the third quadrant
        if(intersectRectangles(r1,r2,c1,c2,rmid+1,rright,cleft,cmid)){
            treeNode downLeftAuxNode=curNode;
            treeBlock * downLeftBlockAux=curBlock;
            treeNode downLeftcurNodeAux;
            uint16_t downLeftCurFlag=curFlag;
            uint16_t downLeftLevel=level;

            downLeftcurNodeAux = downLeftBlockAux->child(downLeftBlockAux, downLeftAuxNode,2, downLeftLevel, maxDepth, downLeftCurFlag);


            if (downLeftcurNodeAux.first == (NODE_TYPE)-1){

            }else {
                downLeftAuxNode = downLeftcurNodeAux;

                if (downLeftBlockAux->nPtrs > 0 && absolutePosition(downLeftAuxNode) == downLeftCurFlag) {
                    downLeftBlockAux = downLeftBlockAux->getPointer(downLeftCurFlag);
                    downLeftAuxNode.first = 0;
                    downLeftAuxNode.second = 0;
                }

                int cmid=(cleft+cright)/2;
                rangeQueryBlock(downLeftBlockAux,std::max(r1,rmid+1),std::min(r2,rright),std::max(c1,cleft),std::min(c2,cmid),length-1,downLeftLevel,maxDepth,rmid+1,rright,cleft,cmid,downLeftAuxNode,downLeftCurFlag,answer);
            }
        }

        //ask the fourth quadrant
        if(intersectRectangles(r1,r2,c1,c2,rmid+1,rright,cmid+1,cright)){
            treeBlock * downRightBlockAux=curBlock;
            treeNode downRightAuxNode=curNode;
            treeNode downRightcurNodeAux;
            uint16_t downRightCurFlag=curFlag;
            uint16_t downRightLevel=level;

            downRightcurNodeAux = downRightBlockAux->child(downRightBlockAux, downRightAuxNode,3, downRightLevel, maxDepth, downRightCurFlag);


            if (downRightcurNodeAux.first == (NODE_TYPE)-1){

            }else {
                downRightAuxNode = downRightcurNodeAux;

                if (downRightBlockAux->nPtrs > 0 && absolutePosition(downRightAuxNode) == downRightCurFlag) {
                    downRightBlockAux = downRightBlockAux->getPointer(downRightCurFlag);
                    downRightAuxNode.first = 0;
                    downRightAuxNode.second = 0;
                }

                int cmid=(cleft+cright)/2;
                rangeQueryBlock(downRightBlockAux,std::max(r1,rmid+1),std::min(r2,rright),std::max(c1,cmid+1),std::min(c2,cright),length-1,downRightLevel,maxDepth,rmid+1,rright,cmid+1,cright,downRightAuxNode,downRightCurFlag,answer);
            }
        }

    }



    return;
}

void rangeQuery(trieNode *t, int r1,int r2,int c1,int c2, uint64_t length,uint64_t level ,uint16_t maxDepth,int rleft,int rright, int cleft,int cright,std::vector< std::vector<int> > &answer){

    if (t==NULL){
        return;
    }else if(t->children[0]==NULL && t->children[1]==NULL && t->children[2]==0 && t->children[3]==0 && t->block==NULL){
        return;
    }else if(t->children[0]==NULL && t->children[1]==NULL && t->children[2]==0 && t->children[3]==0 && t->block!=NULL){
        treeNode curNode(0,0);
        int16_t curFlag=0;
        rangeQueryBlock((treeBlock*)t->block, r1,r2,c1,c2, length, level, maxDepth,rleft,rright,cleft,cright,curNode,curFlag,answer);
    }else{
        int rmid=(rleft+rright)/2;
        int cmid=(cleft+cright)/2;

        //ask if the trie has a 1 in the first quadrant
        if( t->children[0]!=NULL && intersectRectangles(r1,r2,c1,c2,rleft,rmid,cleft,cmid) ){
            //add nodes to ansert
            rangeQuery(t->children[0],std::max(r1,rleft),std::min(r2,rmid),std::max(c1,cleft),std::min(c2,cmid),length-1,level+1,maxDepth,rleft,rmid,cleft,cmid,answer);
        }

        //ask if the trie has a 1 in the second quadrant
        if( t->children[1]!=NULL && intersectRectangles(r1,r2,c1,c2,rleft,rmid,cmid+1,cright) ){
            //add nodes to ansert
            rangeQuery(t->children[1],std::max(r1,rleft),std::min(r2,rmid),std::max(c1,cmid+1),std::min(c2,cright),length-1,level+1,maxDepth,rleft,rmid,cmid+1,cright,answer);
        }

        //ask if the trie has a 1 in the third quadrant
        if( t->children[2]!=NULL && intersectRectangles(r1,r2,c1,c2,rmid+1,rright,cleft,cmid) ){
            //add nodes to ansert
            rangeQuery(t->children[2],std::max(r1,rmid+1),std::min(r2,rright),std::max(c1,cleft),std::min(c2,cmid),length-1,level+1,maxDepth,rmid+1,rright,cleft,cmid,answer);
        }

        //ask if the trie has a 1 in the fourht quadrant
        if( t->children[3]!=NULL && intersectRectangles(r1,r2,c1,c2,rmid+1,rright,cmid+1,cright) ){
            //add nodes to ansert
            rangeQuery(t->children[3],std::max(r1,rmid+1),std::min(r2,rright),std::max(c1,cmid+1),std::min(c2,cright),length-1,level+1,maxDepth,rmid+1,rright,cmid+1,cright,answer);
        }
    }

}
