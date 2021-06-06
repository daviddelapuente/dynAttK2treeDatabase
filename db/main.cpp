#include "../include/plib/tools.cpp"
#include "../include/plib/buildTree.cpp"
#include "../include/sequences/read.cpp"
#include "../include/sequences/kmer.cpp"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <malloc.h>

/*void unityTestsK2TreeDynamicNoMulti(){
    TreeDyn tree(2);
    int edges[2] = {2,5};
    tree.insert(0,edges);
    edges[0]=15;edges[1]=7;
    tree.insert(0,edges);
    tree.insert(0,edges);
    tree.printTL();
}*/

void unityTestsK2TreeDynamic(){
    TreeDyn tree(2,true);
    int edges[2] = {2,5};
    tree.insert(0,edges);
    edges[0]=15;edges[1]=7;
    tree.insert(1,edges);
    edges[0]=2;edges[1]=15;
    tree.insert(2,edges);
    edges[0]=2;edges[1]=8;
    tree.insert(3,edges);
    edges[0]=2;edges[1]=0;
    tree.insert(4,edges);
    edges[0]=2;edges[1]=5;
    tree.insert(5,edges);
    edges[0]=0;edges[1]=0;
    tree.insert(6,edges);
    edges[0]=8;edges[1]=14;
    tree.insert(7,edges);
    edges[0]=13;edges[1]=4;
    tree.insert(8,edges);
    edges[0]=4;edges[1]=8;
    tree.insert(9,edges);
    edges[0]=15;edges[1]=8;
    tree.insert(10,edges);
    tree.printTL();
    int q3[2] = {13,13}, q4[2] = {0,15};
    std::vector<std::vector<int>> vecinos = tree.rangeQuery(q3,q4);
    printf("Find vecinos\n");
    for (int j = 0; j < vecinos.size();j++)
        for (int i = 0; i< vecinos[j].size(); i++)
            printf("%d ",vecinos[j][i]);
    printf("\n");

    if (tree.isNeighbor(2,0) & tree.isNeighbor(2,15))
        printf("Ok!!!\n");
    int q1[2] = {0,15},q2[2] = {0,15};
    std::vector<std::vector<int>> v = tree.rangeQuery(q1,q2);
    for (int i = 0; i < v.size(); i++) {
        printf("line %d\n",i);
        for (int j = 0; j < v[i].size(); j++)
            printf("Valores %d ", v[i][j]);
        printf("\n");
    }
    int q5[2] = {0,15},q6[2] = {8,8};
    v = tree.rangeQuery(q5,q6);
    for (int i = 0; i < v.size(); i++) {
        printf("line %d\n",i);
        for (int j = 0; j < v[i].size(); j++)
            printf("Valores %d ", v[i][j]);
        printf("\n");
    }
}


int main(int argc, char* argv[]) {
    printf("Let's go\n");
    if (argc < 2) {
        fprintf(stderr, "USAGE: %s <baseName>\n", argv[0]);
        return (-1);
    }


    DynAttK2Tree tree(argv[1]);


    printf("probando queries:\n");
    printf("probando querie 1.1) getnodetypes:\n");
    tree.printNodeTypes();

    printf("probando querie 1.2) getedgetypes:\n");
    tree.printEdgeTypes();
    
    printf("probando querie 2.1) scan node by type:\n");
    tree.printScanNodes("BOOK");
    tree.printScanNodes("USER");
    tree.printScanNodes("WRITER");

    printf("probando querie 2.2) scan edge by type:\n");
    tree.printScanEdges("BUY");
    tree.printScanEdges("ESC");
    tree.printScanEdges("RATE");

    printf("probando querie 3.1) get node (x):\n");

    for(int i=0;i<12;i++){
    	tree.printNodeType(i);
    }

    printf("probando querie 3.2) get edge (x):\n");

    for(int i=0;i<15;i++){
    	tree.printEdgeType(i);
    }

    printf("probando querie 4.1) get nodeatribute (x,label):\n");
    tree.printNodeAttribute(7,1);

    printf("probando querie 4.2) get edgeatribute (x,label):\n");
    tree.printEdgeAttribute(1,3);


    printf("probando querie 5.1) get node atribute value (type,att,value):\n");
    tree.printNodeAttributeValue("USER",0,"IT");

    printf("probando querie 5.2) get edge atribute value (type,att,value):\n");
    tree.printEdgeAttributeValue("BUY",0,"NUEVO");

    printf("probando querie 6.1) get node neighbours (type,id):\n");
    tree.printNeighbourNodes("BOOK",0);

    printf("probando querie 6.2) get edge neighbours (type,id):\n");
    tree.printNeighbourEdges("BUY",0);


    printf("end\n");
    

    //Build all the structure
    /*if (!atoi(argv[2])) {
        int nodes = numNodes(argv[1]);
        printf("Num nodes %d\n", nodes);
        int edges = numEdges(argv[1]);
        printf("Num edges %d\n", edges);
        int matrixSize = getSizeMat(nodes);
        printf("MatrizSize %d\n", matrixSize);
        attk2treeStruct *tree = constructFirstApproach(argv[1], argv[3], matrixSize, nodes, edges);
    }else
        reloadAttk2Tree(argv[1],argv[3]);*/
    //Process queries
    //processQueries(argv[2], tree);

    /*
    printf("Pruebas de añadido sobre DynAttk2Tree\n");
    std::vector<int> nodes{1, 0, 1};
    std::vector<int> edgesAtt{1, 0, 0};
    DynAttK2Tree tree(nodes, edgesAtt);
    std::vector <string> tokens({"test", "Coherence", "-", "Regardless"});
    tree.insert(0, tokens);
    tokens = {"test", "Coherence0.2", "-", "Regardless0.2"};
    tree.insert(0, tokens);
    std::vector <string> tokens2({"test2", "Coherence2", "-", "Regardless2"});
    tree.insert(0, tokens2);
    tokens2 = {"test1", "Coherence1", "-", "Regardless1"};
    tree.insert(0, tokens2);
    tokens = {"test", "Coherence0.1", "-", "Regardless0.1"};
    tree.insert(0, tokens);
    tokens = {"0", "1", "2"};
    tree.insert(2, tokens);
    //Prueba sobre kmers
    */


    /*
    Sequence sequence("ACGTETGTCA");
    printf("Sequencia %s\n", sequence.toString().c_str());

    //Prueba Read
    Reads reads;
    std::vector<Info> info_read = reads.readFile(argv[1]);

    //Prueba de los kmers
    FrecuencyStimator frec(0.05,0.01,10);
    for (auto &read: info_read)
        KmerProcessor::countKmers(read,frec,5);
    */
}
