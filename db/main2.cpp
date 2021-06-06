#include "../include/plib/tools.cpp"
#include "../include/plib/buildTree.cpp"
#include "../include/sequences/read.cpp"
#include "../include/sequences/kmer.cpp"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <malloc.h>


int main(int argc, char* argv[]) {
    printf("Let's go\n");

    std::vector<int> nodes{0, 1, 1};
    std::vector<int> edgesAtt{0, 0, 0,1};

    std::vector <string> nodeTypes={"nodeType1","nodeType2","nodeType3"};
    std::vector <int> nodeN={6,3,3};
    int nodeNumAtt=3;
    std::vector< std::vector<int> > nodeAtt={{1,0,0},{0,2,0},{1,0,2}};

    std::vector <string> edgeTypes={"edgeType1","edgeType2","edgeType3"};
    std::vector <int> edgeN={8,3,4};
    int edgeNumAtt=4;
    std::vector< std::vector<int> > edgeAtt={{0,1,0,2},{0,0,1,0},{1,0,1,0}};


    DynAttK2Tree tree(nodes, edgesAtt,nodeTypes,nodeN,nodeNumAtt,nodeAtt,edgeTypes,edgeN,edgeNumAtt,edgeAtt);

    std::vector <string> tokens;

    printf("inserting links\n");

    tokens = {"0", "0", "6"};
    tree.insert(2, tokens);

    tokens = {"1", "0", "7"};
    tree.insert(2, tokens);

    tokens = {"2", "1", "8"};
    tree.insert(2, tokens);

    tokens = {"3", "2", "6"};
    tree.insert(2, tokens);

    tokens = {"4", "3", "7"};
    tree.insert(2, tokens);

    tokens = {"5", "4", "8"};
    tree.insert(2, tokens);

    tokens = {"6", "5", "6"};
    tree.insert(2, tokens);

    tokens = {"7", "5", "7"};
    tree.insert(2, tokens);

    tokens = {"8", "6", "11"};
    tree.insert(2, tokens);

    tokens = {"9", "7", "10"};
    tree.insert(2, tokens);

    tokens = {"10", "8", "9"};
    tree.insert(2, tokens);

    tokens = {"11", "0", "8"};
    tree.insert(2, tokens);

    tokens = {"12", "1", "7"};
    tree.insert(2, tokens);

    tokens = {"13", "2", "8"};
    tree.insert(2, tokens);

    tokens = {"14", "5", "8"};
    tree.insert(2, tokens);



    printf("inserting nodes\n");

    //tokens={"nodeType1","ESP","-","-"};
    tokens={"nodeType1","ESP","-","-"};
    tree.insert(0, tokens);
    
    tokens={"nodeType1","UK","-","-"};
    tree.insert(0, tokens);

    tokens={"nodeType1","IT","-","-"};
    tree.insert(0, tokens);

    tokens={"nodeType1","IT","-","-"};
    tree.insert(0, tokens);

    tokens={"nodeType1","ESP","-","-"};
    tree.insert(0, tokens);

    tokens={"nodeType1","FR","-","-"};
    tree.insert(0, tokens);


    tokens={"nodeType2","-","El señor de los anillos","-"};
    tree.insert(0, tokens);

    tokens={"nodeType2","-","Juraric Park","-"};
    tree.insert(0, tokens);

    tokens={"nodeType2","-","Los hijos del mar","-"};
    tree.insert(0, tokens);



    tokens={"nodeType3","ESP","-","Pedro Feijoo"};
    tree.insert(0, tokens);

    tokens={"nodeType3","USA","-","M. Crichton"};
    tree.insert(0, tokens);

    tokens={"nodeType3","UK","-","Tolkien"};
    tree.insert(0, tokens);


    printf("inserting edges\n");

    tokens={"edgeType1","-","9","-","05/07"};
    tree.insert(1, tokens);

    tokens={"edgeType1","-","6","-","03/01"};
    tree.insert(1, tokens);

    tokens={"edgeType1","-","8","-","04/02"};
    tree.insert(1, tokens);

    tokens={"edgeType1","-","9","-","05/07"};
    tree.insert(1, tokens);

    tokens={"edgeType1","-","5","-","06/08"};
    tree.insert(1, tokens);

    tokens={"edgeType1","-","6","-","07/09"};
    tree.insert(1, tokens);

    tokens={"edgeType1","-","9","-","08/03"};
    tree.insert(1, tokens);

    tokens={"edgeType1","-","6","-","05/02"};
    tree.insert(1, tokens);



    tokens={"edgeType2","-","-","85","-"};
    tree.insert(1, tokens);

    tokens={"edgeType2","-","-","86","-"};
    tree.insert(1, tokens);

    tokens={"edgeType2","-","-","85","-"};
    tree.insert(1, tokens);

	

    tokens={"edgeType3","NUEVO","-","85","-"};
    tree.insert(1, tokens);

    tokens={"edgeType3","NUEVO","-","89","-"};
    tree.insert(1, tokens);

    tokens={"edgeType3","SGDA","-","87","-"};
    tree.insert(1, tokens);

    tokens={"edgeType3","NUEVO","-","86","-"};
    tree.insert(1, tokens);

    printf("probando queries:\n");
    printf("probando querie 1.1) getnodetypes:\n");
    tree.printNodeTypes();
    printf("probando querie 1.2) getedgetypes:\n");
    tree.printEdgeTypes();

    printf("probando querie 2.1) scan node by type:\n");
    tree.printScanNodes("nodeType1");
    tree.printScanNodes("nodeType2");
    tree.printScanNodes("nodeType3");

    printf("probando querie 2.2) scan edge by type:\n");
    tree.printScanEdges("edgeType1");
    tree.printScanEdges("edgeType2");
    tree.printScanEdges("edgeType3");

    printf("probando querie 3.1) get node (x):\n");

    for(int i=0;i<12;i++){
    	tree.printNodeType(i);
    }

    printf("probando querie 3.2) get edge (x):\n");

    for(int i=0;i<15;i++){
    	tree.printEdgeType(i);
    }



    printf("probando querie 4.1) get nodeatribute (id,label):\n");
    tree.printNodeAttribute(7,1);
    tree.printNodeAttribute(0,0);
    tree.printNodeAttribute(4,0);
    tree.printNodeAttribute(11,0);

    printf("probando querie 4.2) get edgeatribute (id,label):\n");
    tree.printEdgeAttribute(1,3);


    printf("probando querie 5.1) get node atribute value (type,att,value):\n");
    tree.printNodeAttributeValue("nodeType2",0,"Juraric Park");

    printf("probando querie 5.2) get edge atribute value (type,att,value):\n");
    tree.printEdgeAttributeValue("edgeType3",0,"NUEVO");

    printf("probando querie 6.1) get node neighbours (type,id):\n");
    tree.printNeighbourNodes("nodeType2",0);
/*
    printf("probando querie 6.2) get edge neighbours (type,id):\n");
    tree.printNeighbourEdges("edgeType3",0);

*/
    
    /*
    printf("probando insert\n");

    tokens={"nodeType1","ESP","-","-"};
    tree.insert(0, tokens);

    tree.printScanNodes("nodeType1");
    tree.printNodeType(12);
    tree.printNodeAttribute(12,0);
    tree.printNodeAttributeValue("nodeType1",0,"ESP");

    tokens = {"15", "12", "6"};
    tree.insert(2, tokens);

    tree.printNeighbourEdges("edgeType3",12);
    */
}
