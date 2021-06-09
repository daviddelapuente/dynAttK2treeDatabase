#include <iostream>
#include "assert.h"
#include "../../include/plib/buildTree.cpp"

void printM(std::vector<std::vector<int> > M){
	for(int i=0;i<M.size();i++){
		for(int j=0;j<M[0].size();j++){
			printf("%i ",M[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char* argv[]) {
	//files

	int scanCode;

    	blockGlobalVars *bgv=(blockGlobalVars*) malloc(sizeof(blockGlobalVars));
    	bgv->init(4,S3,0.99);

    	//we init the trieNode that will contain all the edges
   	trieNode *t = (trieNode *) malloc(sizeof(trieNode));
    	t->init(bgv);
	


	std::cout<<"begin test"<<std::endl;
	int edges[2];
	int nodeId;
	int i;
	std::vector<std::vector<int>> neigs1;
	int q1[2],q2[2];
	bool rangeAsigned=false;

	for(i=0;i<100000;i++){
		scanCode=scanf("%i\t%i\t%i\n", &nodeId ,&edges[0],&edges[1]);
		//insert in block
		uint8_t * str;
		str=morton(edges[0],edges[1],23);
		insertTrie(t, str, 23, 22);
	}

	printf("aaa\n");		
	int pathLength=23;
        int left=0;
        int right=pow(2,pathLength)-1;
	rangeQuery(t,2,2,943,2624,23,0,22,left,right,left,right,neigs1);
	printM(neigs1);
	neigs1.clear();



	return 0;
}
