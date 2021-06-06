#include <iostream>
#include "assert.h"
#include "../include/plib/buildTree.cpp"

void printM(std::vector<std::vector<int> > M){
	for(int i=0;i<M.size();i++){
		for(int j=0;j<M[0].size();j++){
			printf("%i ",M[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char* argv[]) {

	
	
	int scanCode;

	int n;
	int nEdges;
	 
	scanCode=scanf("%i\n", &nEdges);
	printf("%i\n",nEdges);
	scanCode=scanf("%i\n", &n);
	printf("%i\n",n);

	
	TreeDyn t(2,false);
	

    	blockGlobalVars *bgv=(blockGlobalVars*) malloc(sizeof(blockGlobalVars));
    	bgv->init(4,S3,0.99);

    	//we init the trieNode that will contain all the edges
   	trieNode *t2 = (trieNode *) malloc(sizeof(trieNode));
    	t2->init(bgv);
	
	clock_t start, diff=0;
	std::cout<<"begin test"<<std::endl;
	int edges[2];
	int nodeId;
	int i;
	std::vector<std::vector<int>> neigs1;
	std::vector<std::vector<int>> neigs2;
	int q1[2],q2[2];
	bool rangeAsigned=false;
	
	for(i=0;i<190000000;i++){
		scanCode=scanf("%i\t%i\t%i\n", &nodeId ,&edges[0],&edges[1]);
		t.insert(nodeId,edges);
		uint8_t * str;
		str=morton(edges[0],edges[1],23);            
		insertTrie(t2, str, 23, 22);
		

		if (i%5000000 == 0 && i!=0) {
			printf("nedges=%i\n",i);
			if(!rangeAsigned){
				rangeAsigned=true;
				q1[0]=edges[0];
				q1[1]=edges[0];
				q2[0]=312972;
				q2[1]=314152;
			}
		   	neigs1=t.rangeQuery(q1,q2);
			int pathLength=23;
                	int left=0;
                	int right=pow(2,pathLength)-1;
			rangeQuery(t2,q1[0],q1[1],q2[0],q2[1],23,0,22,left,right,left,right,neigs2);  

			assert(neigs1[0].size()==neigs2.size());
			for(int j=0;j<neigs1.size();j++){
				assert(neigs1[0][j]==neigs2[j][1]);
			}
			//printM(neigs1);
			//printf("---------------------------------------------------\n");
			//printM(neigs2);

			neigs2.clear();
			

		}
	}




	return 0;
}
