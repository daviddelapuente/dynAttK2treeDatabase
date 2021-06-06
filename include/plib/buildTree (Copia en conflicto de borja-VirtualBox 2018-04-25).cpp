#include "SchemasV2.cpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#define k 2

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
    string line;
    ifstream myfile(nameFile);
    if (myfile.is_open()){
        while(getline(myfile,line)) {
            vector<string> tokens;
            istringstream iss(line);
            copy(istream_iterator<string>(iss),
                 istream_iterator<string>(),
                 back_inserter(tokens));
            for (int i = 0; i < tokens.size(); i++)
                printf("%d ",stoi(tokens[i]));
            k2DynTree.insert(tokens);
            printf("\n");
        }
        myfile.close();
    }else
        printf("File doesn't exist\n");
}

//Build dynk2Tree from adj list

//Build nodeSchema
void _buildBasicStructure(Schema &schema, char *baseName){
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
    }
}

void _buildAttTypes(Schema &schema, char *baseName){
    char nameFile[256];sprintf(nameFile,"%s.att",baseName);string line;
    ifstream myfile(nameFile);
    if (myfile.is_open()){
        getline(myfile,line);
        int numAtt = stoi(line);
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
                        schema.createAttributes(nodeType, stoi(tokens[i]) - 1, firstDiscover[i],i);
                        if (firstDiscover[i] == -1){
                            firstDiscover[i] = nodeType;
                        }
                    }else
                        schema.createAttributes(nodeType, -1, 0,0);
                    idNode++;
                }
            }else
                break;
            nodeType++;
        }
        myfile.close();
    }
}

void _buildValSchemas(Schema &schema, char *baseName){
    char nameFile[256];sprintf(nameFile,"%s.val",baseName);string line;
    ifstream myfile(nameFile);
    if (myfile.is_open()){
        int nodeType = 0;
        while(getline(myfile,line)){
            string token;
            vector<string> tokens;
            istringstream iss(line);
            while(std::getline(iss, token, '\t'))
                tokens.push_back(token);
            int size = tokens.size();
            if (size) {
                for (uint i = 0; i < size; i++) {
                    printf("%s ",tokens[i].c_str());
                }
                printf("\n");
                schema.modAttributesList(tokens);
            }else
                break;
            nodeType++;
        }
        myfile.close();
    }
}

void buildSchema(Schema &schema, char * baseName){
    _buildBasicStructure(schema,baseName);
    _buildAttTypes(schema,baseName);
    _buildValSchemas(schema,baseName);
    schema.sortNodes();
    schema.showSchema();
}

std::vector<int> NeighborsNode(string label, int id, Schema &schema, TreeDyn &k2Tree){
    std::vector<int> esc = schema.Scan(label);
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

std::vector<int> NeighborsEdge(string label, int id, Schema &schema, TreeDyn &k2tree){
    std::vector<int> esc = schema.Scan(label);
    std::vector<int> output;
    std::vector<std::vector<int>> out;
    int q1[2] = {id,id}, q2[2]={0,k2tree.getMatrixSize()}, j = 0, i = 0,
            store = 0, pos = 0;
    out = k2tree.rangeQuery(q1,q2);
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

void constructFirstApproach(char * baseName, int matrixSize, int nodes, int edges){
    printf("Building a k2Tree inserting each edge one by one\n");
    //Build a k2DynTreeBasic
    TreeDyn k2DynTree(k, matrixSize, true);
    SchemaNode nodeSchema;
    SchemaEdge edgeSchema;
    //k2TreeDyn de los enlaces
    buildk2LineByLine(k2DynTree, baseName);
    k2DynTree.printTL();
    printf("\n");

    //Schema: Nodes
    char nodeBaseName[256];sprintf(nodeBaseName,"%s.nodes",baseName);
    buildSchema(nodeSchema,nodeBaseName);
    //return k2DynTree;

    //Schema: edges
    char edgesBaseName[256];sprintf(edgesBaseName,"%s.edges",baseName);
    buildSchema(edgeSchema,edgesBaseName);

    printf("Fin\n");
    /*
    //Pruebas de unidad para borrar
    printf("Pruebas unidad\n");
    std::vector<string> tokens;
    tokens.push_back("ESP");tokens.push_back("-");tokens.push_back("-");
    nodeSchema.insertNode("USER",tokens);
    std::vector<int> userPos = nodeSchema.Scan("USER");
    for (int i = 0; i < userPos.size(); i++)
        printf("%d\n",userPos[i]);
    printf("Identifier node 12 %s\n",nodeSchema.getType(12).c_str());
    printf("%s\n",nodeSchema.getType(8).c_str());
    std::vector<string> tokens2;
    tokens2.push_back("-");tokens2.push_back("-");tokens2.push_back("87");tokens2.push_back("-");
    edgeSchema.insertNode("ESC",tokens2);
    std::vector<int> escPos = edgeSchema.Scan("ESC");
    for (int i = 0; i < escPos.size(); i++)
        printf("%d\n",escPos[i]);
    tokens.clear();
    tokens.push_back("ABS");tokens.push_back("-");tokens.push_back("-");
    nodeSchema.insertNode("USER",tokens);
    userPos = nodeSchema.Scan("USER");
    for (int i = 0; i < userPos.size(); i++)
        printf("%d\n",userPos[i]);

    //Pruebas sobre attributes
    printf("Attribute %s\n",nodeSchema.getAttribute(12,0).c_str());
    printf("Attribute %s\n",edgeSchema.getAttribute(15,2).c_str());
    printf("Attribute %s\n",nodeSchema.getAttribute(13,0).c_str());
    printf("Attribute %s\n",nodeSchema.getAttribute(9,2).c_str());

    //Pruebas getNodeWithVal
    std::vector<int> output = nodeSchema.selectNodeAttVal("USER",0,"ABS");
    for (int i = 0; i < output.size(); i++)
        printf("%d ",output[i]);
    printf("\n");

    std::vector<int> output2 = edgeSchema.selectNodeAttVal("BUY",0,"SGDA");
    for (int i = 0; i < output2.size(); i++)
        printf("%d ",output2[i]);
    printf("\n");

    //Pruebas relations
    output2 = NeighborsNode("BOOK",5,nodeSchema,k2DynTree);
    for (int i = 0; i < output2.size(); i++)
        printf("%d ",output2[i]);
    printf("\n");

    output2 = NeighborsEdge("ESC",6,edgeSchema,k2DynTree);
    for (int i = 0; i < output2.size(); i++)
        printf("%d ",output2[i]);
    printf("\n");
    printf("End tests\n");
     */
}
