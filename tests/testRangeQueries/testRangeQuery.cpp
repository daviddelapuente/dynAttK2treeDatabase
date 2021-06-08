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

	//nedges
	FILE *fpnEdges;

	//dynArr
	FILE *fpinsertdyn;
	FILE *fpsearchdyn;
	FILE *fpsizedyn;
	FILE *fprangedyn;

	//block
	FILE *fpinsertblock;
	FILE *fpsearchblock;
	FILE *fpsizeblock;
	FILE *fprangeblock;

	fpnEdges=fopen("./nEdges.txt", "w");

	fpinsertdyn=fopen("./resultsDynArr/insert.txt", "w");
	fpsearchdyn=fopen("./resultsDynArr/search.txt", "w");
	fpsizedyn=fopen("./resultsDynArr/size.txt", "w");
	fprangedyn=fopen("./resultsDynArr/range.txt", "w");

	fpinsertblock=fopen("./resultsBlock/insert.txt", "w");
	fpsearchblock=fopen("./resultsBlock/search.txt", "w");
	fpsizeblock=fopen("./resultsBlock/size.txt", "w");
	fprangeblock=fopen("./resultsBlock/range.txt", "w");

	if (fpnEdges == NULL ||fpinsertdyn==NULL || fpsearchdyn==NULL || fpsizedyn==NULL || fprangedyn==NULL ||fpinsertblock==NULL || fpsearchblock==NULL || fpsizeblock==NULL || fprangeblock==NULL){
		printf("Error opening file!\n");
		exit(1);
	}


	
	
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
	
	clock_t start, diffInsertDyn=0,diffSearchDyn=0,diffRangeDyn=0;
	clock_t diffInsertBlock=0,diffSearchBlock=0,diffRangeBlock=0;

	std::cout<<"begin test"<<std::endl;
	int edges[2];
	int nodeId;
	int i;
	std::vector<std::vector<int>> neigs1;
	std::vector<std::vector<int>> neigs2;
	int q1[2],q2[2];
	bool rangeAsigned=false;

	uint64_t msec;
	int sizeDyn;
	uint64_t sizeBlock;

	for(i=0;i<190000001;i++){


		//insert in dyn-arrr
		scanCode=scanf("%i\t%i\t%i\n", &nodeId ,&edges[0],&edges[1]);
		start = clock();
		t.insert(nodeId,edges);
        	diffInsertDyn += clock() - start;
		//search in dyn-arr
		start = clock();
		t.isNeighbor(edges[0],edges[1]);
		diffSearchDyn += clock() - start;

		//insert in block
		uint8_t * str;
		str=morton(edges[0],edges[1],23);
		start = clock();
		insertTrie(t2, str, 23, 22);
		diffInsertBlock += clock() - start;
		//search in block
		start = clock();
		isEdgeTrie(t2,str,23,22);
		diffSearchBlock += clock() - start;
		

		if (i%5000000 == 0 && i!=0) {

			if(!rangeAsigned){
				rangeAsigned=true;
				q1[0]=edges[0];
				q1[1]=edges[0];
				q2[0]=312972;
				q2[1]=314152;
			}




			printf(" nEdges=%i\n",i);
            		fprintf(fpnEdges,"%i,\n",i);
			
			//write for dyn
			msec = diffInsertDyn * 1000 / CLOCKS_PER_SEC;
			fprintf(fpinsertdyn,"%f,\n",(float)msec/i);
			msec = diffSearchDyn * 1000 / CLOCKS_PER_SEC;
			fprintf(fpsearchdyn,"%f,\n",(float)msec/i);
			
			//size in dyn-arr
			sizeDyn=t.numBytes();
            		fprintf(fpsizedyn,"%i,\n",sizeDyn);

			//rangequery in dyn-arr
			start = clock();
		   	neigs1=t.rangeQuery(q1,q2);
			diffRangeDyn += clock() - start;
			msec = diffRangeDyn * 1000;
			fprintf(fprangedyn,"%lu,\n",msec);


			



			//write for block
			msec = diffInsertBlock * 1000 / CLOCKS_PER_SEC;
			fprintf(fpinsertblock,"%f,\n",(float)msec/i);
			msec = diffSearchBlock * 1000 / CLOCKS_PER_SEC;
			fprintf(fpsearchblock,"%f,\n",(float)msec/i);

			//sizein block
			sizeBlock = sizeTrie(t2);
			fprintf(fpsizeblock,"%lu,\n",sizeBlock);

			//rangequery in block
			int pathLength=23;
                	int left=0;
                	int right=pow(2,pathLength)-1;
			start = clock();
			rangeQuery(t2,q1[0],q1[1],q2[0],q2[1],23,0,22,left,right,left,right,neigs2);
			diffRangeBlock += clock() - start;
			msec = diffRangeBlock* 1000;
			fprintf(fprangeblock,"%lu,\n",msec);
			neigs2.clear();



		}
	}

	//close files
	fclose(fpnEdges);
	
	fclose(fpinsertdyn);
	fclose(fpsearchdyn);
	fclose(fpsizedyn);
	fclose(fprangedyn);

	fclose(fpinsertblock);
	fclose(fpsearchblock);
	fclose(fpsizeblock);
	fclose(fprangeblock);

	return 0;
}
