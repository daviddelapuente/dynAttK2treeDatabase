#include "SchemasV2.cpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <time.h>
#include <chrono>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#define k 2

struct attk2treeStruct{
    TreeDyn k2Tree;
    Schema nodes,edges;
};

//Save schema
void saveAttk2Tree(char * baseName, attk2treeStruct * tree){
    char nodeBaseName[256];sprintf(nodeBaseName,"%s.nodes",baseName);
    char edgeBaseName[256];sprintf(edgeBaseName,"%s.edges",baseName);
    char lnkFileName[256];sprintf(lnkFileName,"%s.lnk.tree",baseName);
    tree->nodes.saveSchema(nodeBaseName);
    tree->edges.saveSchema(edgeBaseName);
    tree->k2Tree.saveTL(lnkFileName);
}

int binarySearch(int label, std::vector<int> list){
    float posPiv = 0,posInf = 0, posSup = list.size(),
            pos = list.size()/2.0, trust = 1;
    if (posSup > 0) {
        trust = (label-list[pos]);
        while (trust) {
            posPiv = pos;
            pos = (trust < 0) ? floor((pos + posInf) / 2.0) : ceil((posSup + pos) / 2.0);
            if (posPiv == pos || pos == posSup || pos == posInf)
                return pos;
            posInf = (trust < 0) ? posInf : posPiv;
            posSup = (trust < 0) ? posPiv : posSup;
            trust = label-list[pos];
        }
    }
    return pos;
}

//Build dynk2Tree from a file
void buildk2LineByLine(TreeDyn &k2DynTree, char * baseName)
{
    char nameFile[256];sprintf(nameFile,"%s.edges.lnk",baseName);
    string line;int fila = 0;
    ifstream myfile(nameFile);
    if (myfile.is_open()){
        while(getline(myfile,line)) {
            vector<string> tokens;
            istringstream iss(line);
            copy(istream_iterator<string>(iss),
                 istream_iterator<string>(),
                 back_inserter(tokens));
            k2DynTree.insert(tokens);
            tokens.clear();
            if (!(fila % 100000))
                printf("Fila %d\n",fila);
            fila++;
        }
        k2DynTree.fitMem();
        myfile.close();
    }else
        printf("File doesn't exist\n");
}

//Build nodeSchema
int _buildBasicStructure(Schema &schema, char *baseName){
    char nameFile[256];sprintf(nameFile,"%s.typ",baseName);string line;int currentNumNodes = 0;
    ifstream myfile(nameFile);
    if (myfile.is_open()){
        while(getline(myfile,line)){
            vector<string> tokens;
            istringstream iss(line);
            copy(istream_iterator<string>(iss),
                 istream_iterator<string>(),
                 back_inserter(tokens));
            if (tokens.size()) {
                currentNumNodes += stoi(tokens[1]);
                schema.setVector(tokens[0], stoi(tokens[1]), currentNumNodes - 1);
            }else
                break;
        }
        myfile.close();
        printf("Fin step 1\n");
	    return 1;
    }else
	    printf("File doesn't exist\n");
    return 0;
}

int _buildAttTypes(Schema &schema, char *baseName){
    char nameFile[256];sprintf(nameFile,"%s.att",baseName);string line;
    int numAtt = 0;
    ifstream myfile(nameFile);
    if (myfile.is_open()){
        getline(myfile,line);
        numAtt = stoi(line);
        int firstDiscover[numAtt];for (int i = 0; i < numAtt;i++)firstDiscover[i] = -1;
        schema.setNumAtt(numAtt);
        int nodeType = 0;
        while(getline(myfile,line)){
            vector<string> tokens;
            istringstream iss(line);
            copy(istream_iterator<string>(iss),
                 istream_iterator<string>(),
                 back_inserter(tokens));
            int size = tokens.size();
            if (size) {
                int idNode = 0;
                for (uint i = 0; i < size; i++) {
                    if (stoi(tokens[i])) {
                        schema.createAttributes(nodeType, stoi(tokens[i]), firstDiscover[i],i);
                        if (firstDiscover[i] == -1)
                            firstDiscover[i] = nodeType;
                    }else
                        schema.createAttributes(nodeType, -1, 0,0);
                    idNode++;
                }
            }else
                break;
            nodeType++;
        }
        myfile.close();
        printf("Fin step 2\n");
        return numAtt;
    }else
	printf("File doesn't exist\n");
    return numAtt;
}

int _buildValSchemas(Schema &schema, char *baseName){
    char nameFile[256];sprintf(nameFile,"%s.val",baseName);string line;
    ifstream myfile(nameFile);
    int fila = 0;
    if (myfile.is_open()){
        while(getline(myfile,line)){
            string token;
            vector<string> tokens;
            istringstream iss(line);
            while(std::getline(iss, token, '\t'))
                tokens.push_back(token);
            int size = tokens.size();
            if (size)
                schema.modAttributesList(tokens);
            else
                break;
            if (!(fila % 10000))
                printf("Fila %d\n",fila);
            fila++;
        }
        myfile.close();
        printf("Fin step 3\n");
	    return 1;
    }else
	    printf("File doesn't exist \n");
    myfile.close();
    return 0;
}

//Queries

std::vector<int> NeighborsNode(string label, int id, attk2treeStruct * tree){
    std::vector<int> esc = tree->nodes.Scan(label);
    std::vector<int> output;
    std::vector<std::vector<int>> out;
    int q1[2] = {id,id}, q2[2], j = 0;
    for (int i = 0; i < esc.size();i+=2) {
        q2[0]= esc[i];q2[1] = esc[i+1];
        out = tree->k2Tree.rangeQuery(q1,q2);
        if (!out[0].size())
            break;
        j = 0;
        while (j < out[0].size()){
            output.push_back(out[0][j]);
            j += 2+out[0][j+1];
        }
    }
    return output;
}

std::vector<int> NeighborsEdge(string label, int id, attk2treeStruct * tree){
    std::vector<int> esc = tree->edges.Scan(label);
    std::vector<int> output;
    std::vector<std::vector<int>> out;
    int q1[2] = {id,id}, q2[2]={0,tree->k2Tree.getMatrixSize()}, j = 0, i = 0,
            store = 0, pos = 0;
    out = tree->k2Tree.rangeQuery(q1,q2);
    if (!out[0].size())
        return output;
    j = 0;
    while (j < out[0].size()){
        store = out[0][j];
        for (i = 1; i < (out[0][j+1]+1); i++){
            pos = binarySearch(out[0][j+i+1],esc);
            if (pos < esc.size()) {
                if (out[0][j+i+1] == esc[pos])
                    output.push_back(store);
                else {
                    if (out[0][j+i+1] > esc[pos]) {
                        if (!(pos % 2))
                            output.push_back(store);
                    } else {
                        if (pos % 2)
                            output.push_back(store);
                    }
                }
            }
        }
        j += 2+out[0][j+1];
    }
    return output;
}

void showInfo(int div, int resto, long long microseconds){
    switch (div) {
        case 1:
            (resto == 1)?printf("Nodes: GetType %llu\n",microseconds):
                    printf("Edges: GetType %llu\n",microseconds);
            break;
        case 2:
            (resto == 1)?printf("Nodes: Scan %llu\n",microseconds):
                    printf("Edges: Scan %llu\n",microseconds);
            break;
            break;
        case 3:
            (resto == 1)?printf("Nodes: GetNodeType %llu\n",microseconds):
            printf("Edges: GetNodeType %llu\n",microseconds);
            break;
        case 4:
            (resto == 1)?printf("Nodes: GetNodeAttribute %llu\n",microseconds):
            printf("Edges: GetNodeAttribute %llu\n",microseconds);
            break;
        case 5:
            (resto == 1)?printf("Nodes: SelectNodeAttVal %llu\n",microseconds):
            printf("Edges: SelectNodeAttVal %llu\n",microseconds);
            break;
        case 6:
            (resto == 1)?printf("Nodes: RelatedNodes %llu\n",microseconds):
            printf("Edges: RelatedEdges %llu\n",microseconds);
            break;
    }
}

void _workQueries(string fileName, attk2treeStruct * tree){
    char nameFile[256];sprintf(nameFile,"%s",fileName.c_str());string line;
    printf("Queries %s\n", fileName.c_str());
    ifstream myfile(nameFile);int fila = 0;
    long long microseconds = (long long) 0;
    int resto = 0, div = 0;
    if (myfile.is_open()){
        while(getline(myfile,line)){
            string token;
            vector<string> tokens;
            istringstream iss(line);
            while(std::getline(iss, token, '\t'))
                tokens.push_back(token);
            int size = tokens.size();
            if (!fila){
                resto = stoi(tokens[0])%10;
                div = stoi(tokens[0])/10;
            }else {
                switch (div) {
                    case 1: {
                        auto start = std::chrono::high_resolution_clock::now();
                        std::vector <string> types = (resto == 1) ? tree->nodes.getTypes() :
                                                     tree->edges.getTypes();
                        auto elapsed = std::chrono::high_resolution_clock::now() - start;
                        microseconds += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                    }
                        break;
                    case 2:
                    {
                        auto start = std::chrono::high_resolution_clock::now();
                        std::vector<int> types = (resto == 1) ? tree->nodes.Scan(tokens[0]) :
                                                     tree->edges.Scan(tokens[0]);
                        auto elapsed = std::chrono::high_resolution_clock::now() - start;
                        microseconds += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                    }
                        break;
                    case 3: {
                        auto start = std::chrono::high_resolution_clock::now();
                        string type = (resto == 1) ? tree->nodes.getType(stoi(tokens[0])) :
                                      tree->edges.getType(stoi(tokens[0]));
                        auto elapsed = std::chrono::high_resolution_clock::now() - start;
                        microseconds += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                    }
                        break;
                    case 4:{
                        auto start = std::chrono::high_resolution_clock::now();
                        string type = (resto == 1) ? tree->nodes.getAttribute(stoi(tokens[0]),stoi(tokens[1])) :
                                      tree->edges.getAttribute(stoi(tokens[0]),stoi(tokens[1]));
                        auto elapsed = std::chrono::high_resolution_clock::now() - start;
                        microseconds += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                    }
                        break;
                    case 5:{
                        if  (tokens.size() != 3)
                            break;
                        auto start = std::chrono::high_resolution_clock::now();
                        std::vector<int> type = (resto == 1) ? tree->nodes.selectNodeAttVal(tokens[0],stoi(tokens[1]),tokens[2]) :
                                      tree->edges.selectNodeAttVal(tokens[0],stoi(tokens[1]),tokens[2]);
                        auto elapsed = std::chrono::high_resolution_clock::now() - start;
                        microseconds += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                    }
                        break;
                    case 6:{
                        auto start = std::chrono::high_resolution_clock::now();
                        std::vector<int> type = (resto == 1) ? NeighborsNode(tokens[1],stoi(tokens[0]), tree):
                                                NeighborsEdge(tokens[1],stoi(tokens[0]), tree);
                        auto elapsed = std::chrono::high_resolution_clock::now() - start;
                        microseconds += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                    }
                        break;
                }
            }
            fila++;
        }
    }else
        printf("File does not exist %s\n",nameFile);
    showInfo(div, resto, microseconds);
}

int buildSchema(Schema &schema, char * baseName){
    int result = _buildBasicStructure(schema,baseName);
    if (result){
    	_buildAttTypes(schema,baseName);
    	_buildValSchemas(schema,baseName);
        time_t timer;
        time(&timer);
        schema.sortNodes();
        time_t timer2;
        time(&timer2);
        long long seconds = difftime(timer2,timer)*1000000;

        printf ("End sort %llu microseconds \n", seconds);
    	//schema.showSchema();
    }
    return result;
}

void processQueries(char * baseName, attk2treeStruct * tree){
    printf("Processing queries\n");
    std::vector<string> v;
    std::string nameDir(baseName);
    DIR* dirp = opendir(nameDir.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL)
        if (dp->d_name[0] != '.')
            v.push_back(dp->d_name);
    closedir(dirp);
    for (int i = 0; i < v.size(); i++)
        _workQueries(nameDir+v[i],tree);
}

attk2treeStruct * constructFirstApproach(char * baseName, char * queriesName, int matrixSize, int nodes, int edges){
    printf("Building a k2Tree inserting each edge one by one\n");
    //Build a k2DynTreeBasic
    auto start = std::chrono::high_resolution_clock::now();
    TreeDyn k2DynTree(k, true);
    SchemaNode nodeSchema;
    SchemaEdge edgeSchema;
    //k2TreeDyn de los enlaces
    time_t timer;
    time(&timer);
    buildk2LineByLine(k2DynTree, baseName);
    time_t timer2;
    time(&timer2);
    long long seconds = difftime(timer2,timer)*1000000;

    printf ("End building k2 initial %llu microseconds \n", seconds);
    //k2DynTree.printTL();
    //printf("\n");

    //Schema: Nodes
    char nodeBaseName[256];sprintf(nodeBaseName,"%s.nodes",baseName);
    buildSchema(nodeSchema,nodeBaseName);
    //return k2DynTree;

    //Schema: edges
    char edgesBaseName[256];sprintf(edgesBaseName,"%s.edges",baseName);
    int trustEdgeSchema = buildSchema(edgeSchema,edgesBaseName);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    printf("Complete time %llu\n",microseconds);
    //sleep(1000000);
    /*attTree.k2Tree = k2DynTree;
    attTree.nodes = nodeSchema;
    attTree.edges = edgeSchema;*/
    printf("End Build step\n");
    attk2treeStruct * treeStruct = new attk2treeStruct({k2DynTree, nodeSchema, edgeSchema});
    long long int bytesTotales = treeStruct->k2Tree.numBytes()+
            treeStruct->edges.numBytesUsed()+treeStruct->nodes.numBytesUsed();
    printf("Number of bytes %llu %llu\n",bytesTotales);

    //saveAttk2Tree(baseName,treeStruct);
    //sleep(100000000);
    processQueries(queriesName,treeStruct);
    return treeStruct;
}

int LoadSchema(Schema &schema, char * baseName){
    int result = _buildBasicStructure(schema,baseName);
    if (result){
        int numAtt = _buildAttTypes(schema,baseName);
        schema.loadAtts(baseName);
        schema.sortNodes();

        //schema.showSchema();
    }
    return result;
}

void reloadAttk2Tree(char * baseName, char * queriesName) {
    printf("Reload attk2tree\n");
    SchemaNode nodeSchema;
    SchemaEdge edgeSchema;

    TreeDyn k2DynTree(baseName);

    printf("Fin building tree\n");

    //Schema: Nodes
    char nodeBaseName[256];sprintf(nodeBaseName,"%s.nodes",baseName);
    LoadSchema(nodeSchema,nodeBaseName);

    printf("Fin building nodes schema\n");

    char edgesBaseName[256];sprintf(edgesBaseName,"%s.edges",baseName);
    LoadSchema(edgeSchema,edgesBaseName);

    printf("Fin building edges schema\n");
    attk2treeStruct * treeStruct = new attk2treeStruct({k2DynTree, nodeSchema, edgeSchema});
    processQueries(queriesName,treeStruct);
}

