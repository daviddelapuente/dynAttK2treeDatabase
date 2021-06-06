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

int _buildBasicStructure2(Schema &schema, std::vector<string> types,std::vector<int> N){
	int currentNumNodes = 0;
	for(int i=0;i<N.size();i++){
		currentNumNodes += N[i];
                schema.setVector(types[i], N[i], currentNumNodes - 1);
	}

	return 1;
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

int _buildAttTypes2(Schema &schema,int numAtt,std::vector< std::vector<int> > att){
        int firstDiscover[numAtt];
	for (int i = 0; i < numAtt;i++){
		firstDiscover[i] = -1;
	}
        schema.setNumAtt(numAtt);
        int nodeType = 0;

        for(int i=0;i<att.size();i++){
            
            int size = att[i].size();
            if (size) {
                int idNode = 0;
                for (uint j = 0; j < size; j++) {
                    if (att[i][j]) {
                        schema.createAttributes(nodeType, att[i][j], firstDiscover[j],j);
                        if (firstDiscover[j] == -1)
                            firstDiscover[j] = nodeType;
                    }else
                        schema.createAttributes(nodeType, -1, 0,0);
                    idNode++;
                }
            }else
                break;
            nodeType++;
        }

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

int buildSchema2(Schema &schema,std::vector<string> types,std::vector<int> N,int numAtt, std::vector< std::vector<int> > att){

    int result = _buildBasicStructure2(schema,types,N);
    if (result){
    	_buildAttTypes2(schema,numAtt,att);
    	//_buildValSchemas(schema,baseName);

        //schema.sortNodes();
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

class DynAttK2Tree{
public:
    DynAttK2Tree(std::vector<int> attTypeNode, std::vector<int> attTypeEdge,std::vector<string> nodeTypes, std::vector<int> nodeN, int nodeNumAtt, std::vector< std::vector<int> >nodeAtt,std::vector<string> edgeTypes, std::vector<int> edgeN, int edgeNumAtt, std::vector< std::vector<int> >edgeAtt){
        this->attTypeNode = attTypeNode;
        this->attTypeEdge = attTypeEdge;

	buildSchema2(nodes,nodeTypes,nodeN,nodeNumAtt,nodeAtt);
	buildSchema2(edges,edgeTypes,edgeN,edgeNumAtt,edgeAtt);

        this->bgv->init(4,S3,0.99);
   	this->t->init(bgv);

    }

    DynAttK2Tree(char * baseName){
        _constructFromFile(baseName);
    };


    void insert(int type, std::vector<string> info){
        //Keep an eye over the sort operation
        switch (type) {
            case 0:
                nodes.add(info,attTypeNode);
		//nodes.modAttributesList(info);
                break;
            case 1:
                edges.add(info,attTypeEdge);
		//edges.modAttributesList(info);
                break;
            default:
                k2Tree.insert(info);
                break;
        }
    }

    void insert2(int type, std::vector<string> info){
        //Keep an eye over the sort operation
        switch (type) {
            case 0:
                nodes.add2(info,attTypeNode);
		//nodes.modAttributesList(info);
                break;
            case 1:
                edges.add2(info,attTypeEdge);
		//edges.modAttributesList(info);
                break;
            default:
		uint8_t * str;
		str=morton(stoi(info[1]),stoi(info[2]),23);            
		insertTrie(t, str, 23, 22);
                break;
        }
    }

    //Neighbors Nodes
    std::vector<int> NeighborsNode(string label, int id){
        std::vector<int> esc = nodes.Scan(label);
        std::vector<int> output;
        std::vector<std::vector<int>> out;
        int q1[2] = {id,id}, q2[2], j = 0;
        for (int i = 0; i < esc.size();i+=2) {
            q2[0]= esc[i];q2[1] = esc[i+1];
            out = k2Tree.rangeQuery(q1,q2);
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

    std::vector<int> NeighborsNode2(string label, int id){
        std::vector<int> esc = nodes.Scan(label);
        std::vector<int> output;
        std::vector<std::vector<int>> out;
        int q1[2] = {id,id}, q2[2], j = 0;
        for (int i = 0; i < esc.size();i+=2) {
            q2[0]= esc[i];q2[1] = esc[i+1];
	    int pathLength=23;
            int left=0;
            int right=pow(2,pathLength)-1;
	    rangeQuery(t,id,id,esc[i],esc[i+1],23,0,22,left,right,left,right,out);
            if (!out[0].size())
                break;
            j = 0;
	    for(j=0;j<out.size();j++){
		output.push_back(out[j][1]);
	    }
	    out.clear();
        }
        return output;
    }


    //Neighbors Edges
    std::vector<int> NeighborsEdge(string label, int id){
        std::vector<int> esc = edges.Scan(label);
        std::vector<int> output;
        std::vector<std::vector<int>> out;
        int q1[2] = {id,id}, q2[2]={0,k2Tree.getMatrixSize()}, j = 0, i = 0,
                store = 0, pos = 0;
        out = k2Tree.rangeQuery(q1,q2);
        if (!out[0].size())
            return output;
        j = 0;

	for (int asd=0;asd<esc.size();asd++){
		printf("%i ",esc[asd]);
	}
	printf("\n");

	for (int asd=0;asd<9;asd++){
		printf("%i ",out[0][asd]);
	}
	printf("\n");
        while (j < out[0].size()){
            store = out[0][j];
            for (i = 1; i < (out[0][j+1]+1); i++){
                pos = binarySearch(out[0][j+i+1],esc);
		printf("pos %i \n",pos);
                if (pos < esc.size()) {
                    if (out[0][j+i+1] == esc[pos]){
			printf("escpos %i\n",esc[pos]);
                        output.push_back(store);
                    }else {
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

    void printNodeTypes(){
	    std::vector <string> types = nodes.getTypes();
	    for (int i=0;i<types.size();i++){
		std::cout << types[i] << "\n";
	    }
    }

    void printEdgeTypes(){
	    std::vector <string> types = edges.getTypes();
	    for (int i=0;i<types.size();i++){
		std::cout << types[i] << "\n";
	    }
    }


    void printScanNodes(string label){
	std::vector<int> scanResult = nodes.Scan(label);
	for (int i=0;i<scanResult.size();i++){
		std::cout << scanResult[i] << "\n";
	}
    }

    void printScanEdges(string label){
	std::vector<int> scanResult = edges.Scan(label);
	for (int i=0;i<scanResult.size();i++){
		std::cout << scanResult[i] << "\n";
	}
    }

    void printNodeType(int node){
	string type = nodes.getType(node);
	std::cout << type << "\n";
    }

    void printEdgeType(int node){
	string type = edges.getType(node);
	std::cout << type << "\n";
    }

    void printNodeAttribute(int node, int label){
	string type =  nodes.getAttribute(node,label);
	std::cout << type << "\n";
    }

    string getNodeAttribute(int node, int label){
	return nodes.getAttribute(node,label);
    }

    void printNodeAttribute2(int node, int label){
	string type =  nodes.getAttribute2(node,label);
	std::cout << type << "\n";
    }

    string getNodeAttribute2(int node, int label){
	return nodes.getAttribute2(node,label);
    }

    void printEdgeAttribute(int node, int label){
	string type =  edges.getAttribute(node,label);
	std::cout << type << "\n";
    }

    string getEdgeAttribute(int node, int label){
	return edges.getAttribute(node,label);
    }

    void printEdgeAttribute2(int node, int label){
	string type =  edges.getAttribute2(node,label);
	std::cout << type << "\n";
    }

    string getEdgeAttribute2(int node, int label){
	return edges.getAttribute2(node,label);
    }


    void printNodeAttributeValue(string type, int att,string value){
	std::vector<int> result = nodes.selectNodeAttVal(type,att,value);
	for (int i=0;i<result.size();i++){
		std::cout << result[i] << "\n";
	}
    } 

    std::vector<int> getNodeAttributeValue(string type, int att,string value){
	return nodes.selectNodeAttVal(type,att,value);
    } 

    void printNodeAttributeValue2(string type, int att,string value){
	std::vector<int> result = nodes.selectNodeAttVal2(type,att,value);
	for (int i=0;i<result.size();i++){
		std::cout << result[i] << "\n";
	}
    } 

    std::vector<int> getNodeAttributeValue2(string type, int att,string value){
	return nodes.selectNodeAttVal2(type,att,value);
    } 

    void printEdgeAttributeValue(string type, int att,string value){
	std::vector<int> result = edges.selectNodeAttVal(type,att,value);
	for (int i=0;i<result.size();i++){
		std::cout << result[i] << "\n";
	}
    } 

    std::vector<int> getEdgeAttributeValue(string type, int att,string value){
	return edges.selectNodeAttVal(type,att,value);
    } 

    void printEdgeAttributeValue2(string type, int att,string value){
	std::vector<int> result = edges.selectNodeAttVal2(type,att,value);
	for (int i=0;i<result.size();i++){
		std::cout << result[i] << "\n";
	}
    } 

    std::vector<int> getEdgeAttributeValue2(string type, int att,string value){
	return edges.selectNodeAttVal2(type,att,value);
    } 

    void printNeighbourNodes(string type, int id){
	std::vector<int> result = NeighborsNode(type,id);
	for (int i=0;i<result.size();i++){
		std::cout << result[i] << "\n";
	}
    }

    std::vector<int> getNeighbourNodes(string type, int id){
	return NeighborsNode(type,id);

    }


    void printNeighbourNodes2(string type, int id){
	std::vector<int> result = NeighborsNode2(type,id);
	for (int i=0;i<result.size();i++){
		std::cout << result[i] << "\n";
	}
    }

    std::vector<int> getNeighbourNodes2(string type, int id){
	return NeighborsNode2(type,id);
    }

    /*
    void printNeighbourEdges(string type, int id){
	std::vector<int> result = NeighborsEdge(type,id);
	for (int i=0;i<result.size();i++){
		std::cout << result[i] << "\n";
	}
    }*/

    int printSize(){
	int size=k2Tree.numBytes()+edges.numBytesUsed()+nodes.numBytesUsed();
	printf("total size= %i \n",size);
	return size;
    }

    uint64_t printSize2(){
	uint64_t size = sizeTrie(t)+edges.numBytesUsed2()+nodes.numBytesUsed2();
	printf("total size= %lu \n",size);
	return size;
    }


private:
    void _constructFromFile(char * baseName){
        printf("Building a k2Tree inserting each edge one by one\n");
        //Build a k2DynTreeBasic
        auto start = std::chrono::high_resolution_clock::now();
        //k2TreeDyn de los enlaces
        time_t timer;
        time(&timer);
        buildk2LineByLine(k2Tree, baseName);
        time_t timer2;
        time(&timer2);
        long long seconds = difftime(timer2,timer)*1000000;
        printf ("End building k2 initial %llu microseconds \n", seconds);

        //Schema: Nodes
        char nodeBaseName[256];sprintf(nodeBaseName,"%s.nodes",baseName);
        buildSchema(nodes,nodeBaseName);

        //Schema: edges
        char edgesBaseName[256];sprintf(edgesBaseName,"%s.edges",baseName);
        int trustEdgeSchema = buildSchema(edges,edgesBaseName);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;

        long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        printf("Complete time %llu\n",microseconds);
        printf("End Build step\n");
        //sleep(100000000000000000);
    }
    SchemaEdge edges;
    SchemaNode nodes;

    blockGlobalVars *bgv=(blockGlobalVars*) malloc(sizeof(blockGlobalVars));
    trieNode *t = (trieNode *) malloc(sizeof(trieNode));

    TreeDyn k2Tree = TreeDyn(k, true);
    std::vector<int> attTypeNode,attTypeEdge;
};
