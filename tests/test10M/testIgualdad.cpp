#include "../../include/plib/tools.cpp"
#include "../../include/plib/buildTree.cpp"
#include "../../include/sequences/read.cpp"
#include "../../include/sequences/kmer.cpp"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

char simpleItoa(int i){
	if (i==0){
		return '0';
	}else if(i==1){
		return '1';
	}else if(i==2){
		return '2';
	}else if(i==3){
		return '3';
	}else{
		return '9';
	}
}
void printIntVector(std::vector<int> v){
	for(int i=0;i<v.size();i++){
		printf("%i ",v[i]);
	}
	printf("\n");
}
void printStringVector(std::vector<string> v){
	for(int i=0;i<v.size();i++){
		std::cout<<v[i]<<" ";
	}
	std::cout<<std::endl;
}

void printIntMatrix(std::vector< std::vector<int> > m){
	for(int i=0;i<m.size();i++){
		for(int j=0;j<m[i].size();j++){
			printf("%i ",m[i][j]);		
		}
		printf("\n");
	}
}

int main(int argc, char* argv[]) {
    printf("Let's go\n");

    FILE *fpnodesatt;
    FILE *fpnodestyp;
    FILE *fpnodesval;


    FILE *fpedgesatt;
    FILE *fpedgestyp;
    FILE *fpedgesval;
    FILE *fpedgeslnk;

    fpnodesatt=fopen("../datasets/graph10M/graph.nodes.att", "r");
    fpnodestyp=fopen("../datasets/graph10M/graph.nodes.typ", "r");
    fpnodesval=fopen("../datasets/graph10M/graph.nodes.val", "r");

    fpedgesatt=fopen("../datasets/graph10M/graph.edges.att", "r");
    fpedgestyp=fopen("../datasets/graph10M/graph.edges.typ", "r");
    fpedgesval=fopen("../datasets/graph10M/graph.edges.val", "r");
    fpedgeslnk=fopen("../datasets/graph10M/graph.edges.lnk", "r");


    if (fpnodesatt == NULL || fpnodestyp == NULL|| fpnodesval == NULL ||fpedgesatt==NULL||fpedgestyp==NULL||fpedgeslnk==NULL||fpedgesval==NULL){
        printf("Error opening file!\n");
        exit(1);
    }


    int scanCode;









    int nNodeType,nodeNumAtt;
    scanCode=fscanf(fpnodesatt,"%i\t%i\n", &nNodeType,&nodeNumAtt);
    int aux;
    std::vector<int> nodes(nodeNumAtt,-1);
    std::vector< std::vector<int> > nodeAtt;

    for(int i=0;i<nNodeType;i++){
	std::vector<int> auxVector;
	for(int j=0;j<nodeNumAtt;j++){
	    if(j==nodeNumAtt-1){
	    	scanCode=fscanf(fpnodesatt,"%i\t", &aux);
	    }else{
	    	scanCode=fscanf(fpnodesatt,"%i", &aux);
	    }

	    if(aux==1){
		//dense
	    	nodes[j]=0;
		auxVector.push_back(1);
	    }else if(aux==2){
		//sparse
		nodes[j]=1;
		auxVector.push_back(2);
	    }else{
		auxVector.push_back(0);
	    }
	       
	}
	nodeAtt.push_back(auxVector);
	scanCode=fscanf(fpnodesatt,"\n"); 
    }

    fclose(fpnodesatt);

    std::vector <string> nodeTypes;
    std::vector <int> nodeN;

    string nodetypeplaceholder="nodeTypeX";
    char auxString[200];
    int nType;
    for(int i=0;i<nNodeType;i++){
	scanCode=fscanf(fpnodestyp,"%s\t%i\n", auxString,&nType);
	nodeN.push_back(nType);
	nodetypeplaceholder[8]=simpleItoa(i);
	nodeTypes.push_back(nodetypeplaceholder);
    }

    fclose(fpnodestyp);







    int nEdgeType,edgeNumAtt;
    scanCode=fscanf(fpedgesatt,"%i\t%i\n", &nEdgeType,&edgeNumAtt);
    std::vector<int> edgesAtt(edgeNumAtt,-1);
    std::vector< std::vector<int> > edgeAtt;

    for(int i=0;i<nEdgeType;i++){
	std::vector<int> auxVector;
	for(int j=0;j<edgeNumAtt;j++){
	    if(j==edgeNumAtt-1){
	    	scanCode=fscanf(fpedgesatt,"%i\t", &aux);
	    }else{
	    	scanCode=fscanf(fpedgesatt,"%i", &aux);
	    }

	    if(aux==1){
		//dense
	    	edgesAtt[j]=0;
		auxVector.push_back(1);
	    }else if(aux==2){
		//sparse
		edgesAtt[j]=1;
		auxVector.push_back(2);
	    }else{
		auxVector.push_back(0);
	    }
	       
	}
	edgeAtt.push_back(auxVector);
	scanCode=fscanf(fpedgesatt,"\n"); 
    }

    fclose(fpedgesatt);


    std::vector <string> edgeTypes;
    std::vector <int> edgeN;

    string edgetypeplaceholder="edgeTypeX";
    for(int i=0;i<nEdgeType;i++){
	scanCode=fscanf(fpedgestyp,"%s\t%i\n", auxString,&nType);
	edgeN.push_back(nType);
	edgetypeplaceholder[8]=simpleItoa(i);
	edgeTypes.push_back(edgetypeplaceholder);
    }

    fclose(fpedgestyp);



    printIntVector(nodes);    
    printIntVector(edgesAtt);
    printf("\n");

    printStringVector(nodeTypes);
    printIntVector(nodeN);
    printf("%i\n",nodeNumAtt);
    printIntMatrix(nodeAtt);
    printf("\n");

    printStringVector(edgeTypes);
    printIntVector(edgeN);
    printf("%i\n",edgeNumAtt);
    printIntMatrix(edgeAtt);
    printf("\n");





    DynAttK2Tree tree(nodes, edgesAtt,nodeTypes,nodeN,nodeNumAtt,nodeAtt,edgeTypes,edgeN,edgeNumAtt,edgeAtt);

    DynAttK2Tree tree2(nodes, edgesAtt,nodeTypes,nodeN,nodeNumAtt,nodeAtt,edgeTypes,edgeN,edgeNumAtt,edgeAtt);
    tree2.setEdgesBlockTreePath(24);


    std::vector <string> tokens={"","",""};

    printf("inserting links\n");



    int lnkid, lnka, lnkb;

    for(int i=0;i<edgeN[0]+edgeN[1];i++){
	if(i%1000000==0 && i!=0){
	    printf("links inserted = %i \n",i);
	}
	scanCode=fscanf(fpedgeslnk,"%i\t%i\t%i\n", &lnkid,&lnka,&lnkb);
	tokens[0]=to_string(lnkid);
	tokens[1]=to_string(lnka);
	tokens[2]=to_string(lnkb);

	tree.insert(2, tokens);
	tree2.insert2(2, tokens);

    }


    fclose(fpedgeslnk);






    printf("inserting nodes\n");
    std::vector <string> tokens2={nodeTypes[0]};

    for(int i=0;i<nodeN[0];i++){
	for(int j=0;j<nodeNumAtt+1;j++){
	    if(j==nodeNumAtt){
	    	scanCode=fscanf(fpnodesval,"%s\t", auxString);
	    }else{
	    	scanCode=fscanf(fpnodesval,"%s", auxString);
	    }

	    if(i==0){
		if(j>0){
		    tokens2.push_back(auxString);
		}
		
	    }else{
		if(j>0){
		    tokens2[j]=auxString;
		}
		
	    }
	}
	scanCode=fscanf(fpnodesval,"\n"); 

	tree.insert(0, tokens2);
	tree2.insert2(0, tokens2);

    }

    tokens2[0]=nodeTypes[1];
    for(int i=0;i<nodeN[1];i++){
	for(int j=0;j<nodeNumAtt+1;j++){
	    if(j==nodeNumAtt){
	    	scanCode=fscanf(fpnodesval,"%s\t", auxString);
	    }else{
	    	scanCode=fscanf(fpnodesval,"%s", auxString);
	    }

	    
	    if(j>0){
		tokens2[j]=auxString;
	    }
	}
	
	scanCode=fscanf(fpnodesval,"\n"); 

	tree.insert(0, tokens2);
	tree2.insert2(0, tokens2);
    }

    tokens2[0]=nodeTypes[2];
    for(int i=0;i<nodeN[2];i++){
	for(int j=0;j<nodeNumAtt+1;j++){
	    if(j==nodeNumAtt){
	    	scanCode=fscanf(fpnodesval,"%s\t", auxString);
	    }else{
	    	scanCode=fscanf(fpnodesval,"%s", auxString);
	    }

	    
	    if(j>0){
		tokens2[j]=auxString;
	    }
	}

	scanCode=fscanf(fpnodesval,"\n"); 

	tree.insert(0, tokens2);
	tree2.insert2(0, tokens2);
    }


    fclose(fpnodesval);






    printf("inserting edges\n");
    std::vector <string> tokens3={edgeTypes[0]};

    for(int i=0;i<edgeN[0];i++){
	
	if(i%1000000==0 && i!=0){
	    printf("links inserted = %i \n",i);
	}
	for(int j=0;j<edgeNumAtt+1;j++){
	    if(j==edgeNumAtt){
	    	scanCode=fscanf(fpedgesval,"%s\t", auxString);
	    }else{
	    	scanCode=fscanf(fpedgesval,"%s", auxString);
	    }

	    if(i==0){
		if(j>0){
		    tokens3.push_back(auxString);
		}
		
	    }else{
		if(j>0){
		    tokens3[j]=auxString;
		}
		
	    }
	}
	scanCode=fscanf(fpedgesval,"\n"); 

	tree.insert(1, tokens3);
	tree2.insert2(1, tokens3);

    }


    tokens3[0]=edgeTypes[1];
    for(int i=0;i<edgeN[1];i++){
	for(int j=0;j<edgeNumAtt+1;j++){
	    if(j==edgeNumAtt){
	    	scanCode=fscanf(fpedgesval,"%s\t", auxString);
	    }else{
	    	scanCode=fscanf(fpedgesval,"%s", auxString);
	    }

	    
	    if(j>0){
		tokens3[j]=auxString;
	    }
	}

	scanCode=fscanf(fpedgesval,"\n"); 

	tree.insert(1, tokens3);
	tree2.insert2(1, tokens3);
    }



    fclose(fpedgesval);











    printf("probando querie 4.1) get nodeatribute (id,label):\n");


    for(int i=0;i<=71566;i++){

	assert(tree.getNodeAttribute(i,0)==tree2.getNodeAttribute2(i,0));

    }


    for(int i=71567;i<=82247;i++){

	assert(tree.getNodeAttribute(i,1)==tree2.getNodeAttribute2(i,1));

    }


    for(int i=71567;i<=82247;i++){

	assert(tree.getNodeAttribute(i,2)==tree2.getNodeAttribute2(i,2));

    }


    for(int i=71567;i<=82247;i++){

	assert(tree.getNodeAttribute(i,3)==tree2.getNodeAttribute2(i,3));

    }

    for(int i=822478;i<=82267;i++){

	assert(tree.getNodeAttribute(i,4)==tree2.getNodeAttribute2(i,4));

    }








    printf("probando querie 4.2) get edgeatribute (id,label):\n");


    for(int i=0;i<=10000053;i++){
	if(i%2000000==0 && i!=0){
	    printf("rangequery = %i \n",i);
	}

	assert(tree.getEdgeAttribute(i,0)==tree2.getEdgeAttribute2(i,0));

    }


    for(int i=0;i<=10000053;i++){
	if(i%2000000==0 && i!=0){
	    printf("rangequery = %i \n",i);
	}

	assert(tree.getEdgeAttribute(i,1)==tree2.getEdgeAttribute2(i,1));

    }














    printf("probando querie 5.1) get node atribute value (type,att,value):\n");

    std::vector<int> V1;
    std::vector<int> V2;
    for(int year=1915;year<=2008;year++){
	
	V1=tree.getNodeAttributeValue("nodeType1",3,std::to_string(year));
	V2=tree2.getNodeAttributeValue2("nodeType1",3,std::to_string(year));

	assert(V1.size()==V2.size());
	for(int j=0;j<V1.size();j++){
	    assert(V1[j]==V2[j]);
	}
	V1.clear();
	V2.clear();
    }



    V1=tree.getNodeAttributeValue("nodeType2",4,"Action");
    V2=tree2.getNodeAttributeValue2("nodeType2",4,"Action");
    assert(V1.size()==V2.size());
    for(int j=0;j<V1.size();j++){
	assert(V1[j]==V2[j]);
    }
    V1.clear();
    V2.clear();

    V1=tree.getNodeAttributeValue("nodeType2",4,"Adventure");
    V2=tree2.getNodeAttributeValue2("nodeType2",4,"Adventure");
    assert(V1.size()==V2.size());
    for(int j=0;j<V1.size();j++){
	assert(V1[j]==V2[j]);
    }
    V1.clear();
    V2.clear();

    V1=tree.getNodeAttributeValue("nodeType2",4,"Animation");
    V2=tree2.getNodeAttributeValue2("nodeType2",4,"Animation");
    assert(V1.size()==V2.size());
    for(int j=0;j<V1.size();j++){
	assert(V1[j]==V2[j]);
    }
    V1.clear();
    V2.clear();










    printf("probando querie 5.2) get edge atribute value (type,att,value):\n");


    V1=tree.getEdgeAttributeValue("edgeType0",0,"0");
    V2=tree2.getEdgeAttributeValue2("edgeType0",0,"0");
    assert(V1.size()==V2.size());
    for(int j=0;j<V1.size();j++){
	assert(V1[j]==V2[j]);
    }
    V1.clear();
    V2.clear();

    V1=tree.getEdgeAttributeValue("edgeType0",0,"1");
    V2=tree2.getEdgeAttributeValue2("edgeType0",0,"1");
    assert(V1.size()==V2.size());
    for(int j=0;j<V1.size();j++){
	assert(V1[j]==V2[j]);
    }
    V1.clear();
    V2.clear();

    V1=tree.getEdgeAttributeValue("edgeType0",0,"2");
    V2=tree2.getEdgeAttributeValue2("edgeType0",0,"2");
    assert(V1.size()==V2.size());
    for(int j=0;j<V1.size();j++){
	assert(V1[j]==V2[j]);
    }
    V1.clear();
    V2.clear();

    V1=tree.getEdgeAttributeValue("edgeType0",0,"3");
    V2=tree2.getEdgeAttributeValue2("edgeType0",0,"3");
    assert(V1.size()==V2.size());
    for(int j=0;j<V1.size();j++){
	assert(V1[j]==V2[j]);
    }
    V1.clear();
    V2.clear();

    V1=tree.getEdgeAttributeValue("edgeType0",0,"4");
    V2=tree2.getEdgeAttributeValue2("edgeType0",0,"4");
    assert(V1.size()==V2.size());
    for(int j=0;j<V1.size();j++){
	assert(V1[j]==V2[j]);
    }
    V1.clear();
    V2.clear();



    printf("probando querie 6.1) get node neighbours (type,id):\n");


    for(int i=0;i<=71566;i++){

	V1=tree.getNeighbourNodes("nodeType1",i);
	V2=tree2.getNeighbourNodes2("nodeType1",i);
	assert(V1.size()==V2.size());
	for(int j=0;j<V1.size();j++){
	    assert(V1[j]==V2[j]);
	}
	V1.clear();
	V2.clear();
    }

    
    printf("felicidades, todos los tests pasaron\n");

}
