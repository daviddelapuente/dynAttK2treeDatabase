#include "../include/plib/tools.cpp"
#include "../include/plib/buildTree.cpp"
#include "../include/sequences/read.cpp"
#include "../include/sequences/kmer.cpp"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <malloc.h>

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

    fpnodesatt=fopen("./datasets/graph/graph.nodes.att", "r");
    fpnodestyp=fopen("./datasets/graph/graph.nodes.typ", "r");
    fpnodesval=fopen("./datasets/graph/graph.nodes.val", "r");

    fpedgesatt=fopen("./datasets/graph/graph.edges.att", "r");
    fpedgestyp=fopen("./datasets/graph/graph.edges.typ", "r");
    fpedgesval=fopen("./datasets/graph/graph.edges.val", "r");
    fpedgeslnk=fopen("./datasets/graph/graph.edges.lnk", "r");

    FILE *fpsize;
    FILE *fpinsert;
    FILE *fpqueries;

    fpsize=fopen("./resultsml100k2/size.txt", "w");
    fpinsert=fopen("./resultsml100k2/insertions.txt", "w");
    fpqueries=fopen("./resultsml100k2/queries.txt", "w");

    if (fpnodesatt == NULL || fpnodestyp == NULL|| fpnodesval == NULL ||fpedgesatt==NULL||fpedgestyp==NULL||fpedgeslnk==NULL||fpedgesval==NULL  || fpsize==NULL || fpinsert==NULL ||fpqueries==NULL){
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
	scanCode=fscanf(fpnodesatt,"\n", &aux); 
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
	scanCode=fscanf(fpedgesatt,"\n", &aux); 
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

    DynAttK2Tree tree(nodes, edgesAtt,nodeTypes,nodeN,nodeNumAtt,nodeAtt,edgeTypes,edgeN,edgeNumAtt,edgeAtt);


    std::vector <string> tokens={"","",""};

    printf("inserting links\n");

    clock_t start, diff=0;

    int lnkid, lnka, lnkb;

    for(int i=0;i<edgeN[0]+edgeN[1];i++){
	scanCode=fscanf(fpedgeslnk,"%i\t%i\t%i\n", &lnkid,&lnka,&lnkb);
	tokens[0]=to_string(lnkid);
	tokens[1]=to_string(lnka);
	tokens[2]=to_string(lnkb);
	start = clock();
	tree.insert2(2, tokens);
	diff += clock() - start;
    }


    fclose(fpedgeslnk);

    uint64_t msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(fpinsert,"insert links(ms) = %f \n",(float)msec);


    printf("inserting nodes\n");
    std::vector <string> tokens2={nodeTypes[0]};
    diff=0;
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
	start = clock();
	tree.insert2(0, tokens2);
	diff += clock() - start;
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
	start = clock();
	tree.insert2(0, tokens2);
	diff += clock() - start;
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
	start = clock();
	tree.insert2(0, tokens2);
	diff += clock() - start;
    }


    fclose(fpnodesval);

    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(fpinsert,"insert nodes val(ms) = %f \n",(float)msec);

    printf("inserting edges\n");
    std::vector <string> tokens3={edgeTypes[0]};
    diff=0;
    
    for(int i=0;i<edgeN[0];i++){
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
	start = clock();
	tree.insert2(1, tokens3);
	diff += clock() - start;
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
	start = clock();
	tree.insert2(1, tokens3);
	diff += clock() - start;
    }



    fclose(fpedgesval);
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(fpinsert,"insert nodes val(ms) = %f \n",(float)msec);

/*


    printf("probando queries:\n");
    printf("probando querie 1.1) getnodetypes:\n");
    tree.printNodeTypes();
    printf("probando querie 1.2) getedgetypes:\n");
    tree.printEdgeTypes();

    printf("probando querie 2.1) scan node by type:\n");
    tree.printScanNodes("nodeType0");
    tree.printScanNodes("nodeType1");
    tree.printScanNodes("nodeType2");

    printf("probando querie 2.2) scan edge by type:\n");
    tree.printScanEdges("edgeType0");
    tree.printScanEdges("edgeType1");

    printf("probando querie 3.1) get node (x):\n");

    
    tree.printNodeType(0);
    tree.printNodeType(942);
    tree.printNodeType(500);

    tree.printNodeType(943);
    tree.printNodeType(2624);
    tree.printNodeType(945);

    tree.printNodeType(2625);
    tree.printNodeType(2643);
    tree.printNodeType(2630);
    

    printf("probando querie 3.2) get edge (x):\n");


    tree.printEdgeType(0);
    tree.printEdgeType(99999);
    tree.printEdgeType(43874);

    tree.printEdgeType(100000);
    tree.printEdgeType(102892);
    tree.printEdgeType(101010);




*/
    printf("probando querie 4.1) get nodeatribute (id,label):\n");

    diff=0;
    for(int i=0;i<=942;i++){
	start = clock();
	tree.getNodeAttribute2(i,0);
	diff += clock() - start;
    }
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(fpqueries,"queries 4.1) get node atribute (user,0) (ms) = %f \n",(float)msec);

    diff=0;
    for(int i=0;i<=942;i++){
	start = clock();
	tree.getNodeAttribute2(i,1);
	diff += clock() - start;
    }
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(fpqueries,"queries 4.1) get node atribute (user,1) (ms) = %f \n",(float)msec);

    diff=0;
    for(int i=0;i<=942;i++){
	start = clock();
	tree.getNodeAttribute2(i,2);
	diff += clock() - start;
    }
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(fpqueries,"queries 4.1) get node atribute (user,2) (ms) = %f \n",(float)msec);

    diff=0;
    for(int i=0;i<=942;i++){
	start = clock();
	tree.getNodeAttribute2(i,3);
	diff += clock() - start;
    }
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(fpqueries,"queries 4.1) get node atribute (user,3) (ms) = %f \n",(float)msec);





    diff=0;
    for(int i=943;i<=2624;i++){
	start = clock();
	tree.getNodeAttribute2(i,4);
	diff += clock() - start;
    }
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(fpqueries,"queries 4.1) get node atribute (movie,4) (ms) = %f \n",(float)msec);

    diff=0;
    for(int i=943;i<=2624;i++){
	start = clock();
	tree.getNodeAttribute2(i,5);
	diff += clock() - start;
    }
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(fpqueries,"queries 4.1) get node atribute (movie,5) (ms) = %f \n",(float)msec);

    diff=0;
    for(int i=943;i<=2624;i++){
	start = clock();
	tree.getNodeAttribute2(i,6);
	diff += clock() - start;
    }
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(fpqueries,"queries 4.1) get node atribute (movie,6) (ms) = %f \n",(float)msec);



    diff=0;
    for(int i=2625;i<=2643;i++){
	start = clock();
	tree.getNodeAttribute2(i,7);
	diff += clock() - start;
    }
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(fpqueries,"queries 4.1) get node atribute (genre,7) (ms) = %f \n",(float)msec);


    fprintf(fpqueries,"\n");


    printf("probando querie 4.2) get edgeatribute (id,label):\n");

    diff=0;
    for(int i=0;i<=99999;i++){
	start = clock();
	tree.getEdgeAttribute2(i,0);
	diff += clock() - start;
    }
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(fpqueries,"queries 4.2) get edge atribute (rating,0) (ms) = %f \n",(float)msec);

    diff=0;
    for(int i=0;i<=99999;i++){
	start = clock();
	tree.getEdgeAttribute2(i,1);
	diff += clock() - start;
    }
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(fpqueries,"queries 4.2) get edge atribute (rating,1) (ms) = %f \n",(float)msec);

    fprintf(fpqueries,"\n");


    printf("probando querie 5.1) get node atribute value (type,att,value):\n");
    

    diff=0;
    start = clock();
    tree.getNodeAttributeValue2("nodeType0",0,"7");
    tree.getNodeAttributeValue2("nodeType0",0,"14");
    tree.getNodeAttributeValue2("nodeType0",0,"25");
    tree.getNodeAttributeValue2("nodeType0",0,"30");
    tree.getNodeAttributeValue2("nodeType0",0,"49");
    tree.getNodeAttributeValue2("nodeType0",0,"50");
    tree.getNodeAttributeValue2("nodeType0",0,"68");
    tree.getNodeAttributeValue2("nodeType0",0,"71");
    tree.getNodeAttributeValue2("nodeType0",0,"86");
    tree.getNodeAttributeValue2("nodeType0",0,"99");
    diff += clock() - start;

    msec = diff * 1000;
    fprintf(fpqueries,"queries 5.1) get node atribute value (user,0,x) (1000000*ms) = %f \n",(float)msec);


    diff=0;
    start = clock();
    tree.getNodeAttributeValue2("nodeType0",1,"M");
    tree.getNodeAttributeValue2("nodeType0",1,"M");
    diff += clock() - start;

    msec = diff * 1000;
    fprintf(fpqueries,"queries 5.1) get node atribute value (user,1,x) (1000000*ms) = %f \n",(float)msec);




    diff=0;
    start = clock();
    tree.getNodeAttributeValue2("nodeType0",2,"administrator");
    tree.getNodeAttributeValue2("nodeType0",2,"artist");
    tree.getNodeAttributeValue2("nodeType0",2,"doctor");
    tree.getNodeAttributeValue2("nodeType0",2,"educator");
    tree.getNodeAttributeValue2("nodeType0",2,"engineer");
    tree.getNodeAttributeValue2("nodeType0",2,"executive");
    tree.getNodeAttributeValue2("nodeType0",2,"healthcare");
    tree.getNodeAttributeValue2("nodeType0",2,"homemaker");
    tree.getNodeAttributeValue2("nodeType0",2,"lawyer");
    tree.getNodeAttributeValue2("nodeType0",2,"librarian");
    tree.getNodeAttributeValue2("nodeType0",2,"marketing");
    tree.getNodeAttributeValue2("nodeType0",2,"none");
    tree.getNodeAttributeValue2("nodeType0",2,"other");
    tree.getNodeAttributeValue2("nodeType0",2,"programmer");
    tree.getNodeAttributeValue2("nodeType0",2,"retired");
    tree.getNodeAttributeValue2("nodeType0",2,"salesman");
    tree.getNodeAttributeValue2("nodeType0",2,"scientist");
    tree.getNodeAttributeValue2("nodeType0",2,"student");
    tree.getNodeAttributeValue2("nodeType0",2,"technician");
    tree.getNodeAttributeValue2("nodeType0",2,"writer");
    diff += clock() - start;

    msec = diff * 1000;
    fprintf(fpqueries,"queries 5.1) get node atribute value (user,2,x) (1000000*ms) = %f \n",(float)msec);






    diff=0;
    start = clock();
    tree.getNodeAttributeValue2("nodeType2",7,"unknown");
    tree.getNodeAttributeValue2("nodeType2",7,"Action");
    tree.getNodeAttributeValue2("nodeType2",7,"Adventure");
    tree.getNodeAttributeValue2("nodeType2",7,"Animation");
    tree.getNodeAttributeValue2("nodeType2",7,"Children's");
    tree.getNodeAttributeValue2("nodeType2",7,"Comedy");
    tree.getNodeAttributeValue2("nodeType2",7,"Crime");
    tree.getNodeAttributeValue2("nodeType2",7,"Documentary");
    tree.getNodeAttributeValue2("nodeType2",7,"Drama");
    tree.getNodeAttributeValue2("nodeType2",7,"Fantasy");
    tree.getNodeAttributeValue2("nodeType2",7,"Film-Noir");
    tree.getNodeAttributeValue2("nodeType2",7,"Horror");
    tree.getNodeAttributeValue2("nodeType2",7,"Musical");
    tree.getNodeAttributeValue2("nodeType2",7,"Mystery");
    tree.getNodeAttributeValue2("nodeType2",7,"Romance");
    tree.getNodeAttributeValue2("nodeType2",7,"Sci-Fi");
    tree.getNodeAttributeValue2("nodeType2",7,"Thriller");
    tree.getNodeAttributeValue2("nodeType2",7,"War");
    tree.getNodeAttributeValue2("nodeType2",7,"Western");
    diff += clock() - start;

    msec = diff * 1000;
    fprintf(fpqueries,"queries 5.1) get node atribute value (movie,7,x) (1000000*ms) = %f \n",(float)msec);

    fprintf(fpqueries,"\n");

    printf("probando querie 5.2) get edge atribute value (type,att,value):\n");

    diff=0;
    start = clock();
    tree.getEdgeAttributeValue2("edgeType0",0,"0");
    tree.getEdgeAttributeValue2("edgeType0",0,"1");
    tree.getEdgeAttributeValue2("edgeType0",0,"2");
    tree.getEdgeAttributeValue2("edgeType0",0,"3");
    tree.getEdgeAttributeValue2("edgeType0",0,"4");
    diff += clock() - start;

    msec = diff * 1000/ CLOCKS_PER_SEC;
    fprintf(fpqueries,"queries 5.2) get edge atribute value (rating,0,x) (ms) = %f \n",(float)msec);





    fprintf(fpqueries,"\n");
    printf("probando querie 6.1) get node neighbours (type,id):\n");
    diff=0;

    for(int i=0;i<=942;i++){
    	start = clock();
	tree.getNeighbourNodes2("nodeType1",i);
    	diff += clock() - start;
    }
    
    msec = diff * 1000/ CLOCKS_PER_SEC;
    fprintf(fpqueries,"queries 6.1) get node neighbours (movie,i) (ms) = %f \n",(float)msec);

    


    fprintf(fpsize,"%i\n",tree.printSize());
    fclose(fpinsert);
    fclose(fpsize);
    fclose(fpqueries);
}
