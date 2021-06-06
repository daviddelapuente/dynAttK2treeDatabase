#include "../dynamic.hpp"
#include "AttributeV2.cpp"
#include <fstream>

//Para los densos -> alternativa con k2-treesDynamic
struct node{
    string labelName;
    int numNodes;
    uchar id;
    std::vector<Att*> attributes;
};
int binarySearch(string label, std::vector<node> nodes){
    int posPiv = 0,posInf = 0, posSup = nodes.size(),
            pos = nodes.size()/2, trust = label.compare(nodes[pos].labelName);
    while (trust){
        posPiv = pos;
        pos = (trust < 0)?(pos+posInf)/2:(posSup+pos)/2;
        if (posPiv == pos)
            return pos;
        posInf = (trust < 0)?posInf:posPiv;
        posSup = (trust < 0)?posPiv:posSup;
        trust = label.compare(nodes[pos].labelName);
    }
    return pos;
}
class Schema;

class Schema {
public:
    //Build methods
    void setVector(string label, int num, int numNodesTotal){
        nodes.push_back(node());
        nodes[nodes.size()-1].labelName = label;
        nodes[nodes.size()-1].numNodes = numNodesTotal;
        printf("%d\n",numNodesTotal);
        nodes[nodes.size()-1].id = this->id++;
        labels.push_back(label);
    }

    void setNumAtt(int numAtts){
        this->numAtts = numAtts;
    }

    void createAttributes(int nodeType, int attType, int numAtt, int placeAtt){
        if (attType == 1)
            nodes[nodeType].attributes.push_back(new DenseAtt());
        else{
            if (attType == 2)
                nodes[nodeType].attributes.push_back(new SparseAtt());
            else
                nodes[nodeType].attributes.push_back(NULL);
        }
    }

    void modAttributesList(vector<string> tokens){
        int size = tokens.size(), att = 0, actualType = 0, num_node = stoi(tokens[0]);
        string  s = "-";
        //Si hay muchas labels malo
        while (this->nodes[actualType].numNodes < num_node)
            actualType++;
        for (int i = 1; i < size; i++) {
            if ((tokens[i] != s) & (tokens[i] != ""))
                nodes[actualType].attributes[att]->addAtt(tokens[i],wtSimbols.rank(wtSimbols.size(),actualType), 0);
            att++;
        }
        wtSimbols.push_back(actualType);
    }

    //Update methods
    void insertNode(string label, vector<string> tokens){
        int pos = binarySearch(label,this->nodes), size = tokens.size(), att = 0;
        string  s = "-";
        if (pos != -1) {
            for (int i = 0; i < size; i++) {
                if (!(tokens[i] == s)) {
                    this->nodes[pos].attributes[att]->addAtt(tokens[i], wtSimbols.rank(wtSimbols.size(), nodes[pos].id),1);
                }
                att++;
            }
            wtSimbols.push_back(nodes[pos].id);
        }
    }

    //Operation methods
    std::vector<string> getTypes(){
        std::vector<string> output;
        for (int i = 0; i < nodes.size(); i++)
            output.push_back(nodes[i].labelName);
        return output;
    }

    std::vector<int> Scan(string label) {
        std::vector<int> output;
        int pos = binarySearch(label, this->nodes);
        if (pos == this->nodes.size() || label.compare(this->nodes[pos].labelName)!=0)
            return output;
        int numLabels = wtSimbols.rank(wtSimbols.size(), nodes[pos].id),
                        ref = nodes[pos].numNodes- ((pos != 0)? nodes[pos-1].numNodes:-1);
        if (numLabels == ref){
            (pos != 0)?output.push_back(nodes[pos-1].numNodes+1):output.push_back(0);
            output.push_back(nodes[pos].numNodes);
            return output;
        }
        int posPiv = wtSimbols.select(0, nodes[pos].id);
        int j = posPiv;
        output.push_back(posPiv);
        for (int i = 1; i < numLabels; i++) {
            j = wtSimbols.select(i, nodes[pos].id);
            if (j == posPiv + 1)
                posPiv = j;
            else {
                output.push_back(posPiv);
                output.push_back(j);
                posPiv = j;
            }
        }
        output.push_back(j);
    }

    string getType(int idNode){
       return labels[wtSimbols.at(idNode)];
    }

    //En el paper hablan de un attribute como label en los ficheros no habia ninguna label
    //getNodeAttribute
    string getAttribute(int idNode, int numAtt){
        int pos = binarySearch(getType(idNode), this->nodes);
        if (this->nodes[pos].attributes[numAtt] != NULL)
            return this->nodes[pos].attributes[numAtt]->getValue(wtSimbols.rank(idNode+1,nodes[pos].id)-1);
        else
            return "";
    }
    //get
    std::vector<int> selectNodeAttVal(string label, int numAtt, string attVal){
        std::vector<int> nodes;
        int pos = binarySearch(label,this->nodes);
        int vals[2] = {0, this->nodes[pos].numNodes};
        (pos != 0)?vals[0] = this->nodes[pos-1].numNodes+1:0;
        if (this->nodes[pos].attributes[numAtt] == NULL)
            return nodes;
        std::vector<int> nodesA = this->nodes[pos].attributes[numAtt]->getNodesWithVal(attVal);
        for (int i = 0; i < nodesA.size(); i++){
            if (nodesA[i]+vals[0] < vals[1])
                nodes.push_back(nodesA[i]+vals[0]);
            else
                nodes.push_back(wtSimbols.select(nodesA[i],this->nodes[pos].id));
        }
        return nodes;
    }

    //Utils methods
    void sortNodes(){
        std::sort(this->nodes.begin(),this->nodes.end(),[](const node &a, const node &b)
            {return (a.labelName.compare(b.labelName) < 0);});
        printf("Sort\n");
        for (int i = 0; i < nodes.size();i++)
            for (int j = 0; j < nodes[i].attributes.size(); j++) {
                nodes[i].attributes.shrink_to_fit();
                if (nodes[i].attributes[j] != NULL)
                    nodes[i].attributes[j]->sortValues();
            }
    }

    int numBytesUsed(){
        long long int bytes = sizeof(std::vector<node>())+sizeof(std::vector<string>());
        for (int i = 0; i < nodes.size(); i++){
            bytes += nodes[i].labelName.size();
            bytes += 5;
            for (int j = 0; j < nodes[i].attributes.size();j++)
                if (nodes[i].attributes[j] != NULL)
                    bytes += nodes[i].attributes[j]->numBytes()+ sizeof(Att);
        }
        for (int i = 0; i < labels.size(); i++)
            bytes += labels[i].size();
        return bytes+wtSimbols.bit_size()/8;
    }

    //Show methods
    void showSchema(){
        for (int i = 0; i < nodes.size(); i++){
            printf("NodeLabel %s\n",nodes[i].labelName.c_str());
            printf("NumNodes %d\n",wtSimbols.rank(wtSimbols.size(),nodes[i].id));
            printf("Id %d\n",nodes[i].id);
            for (int j = 0; j < nodes[i].attributes.size();j++){
                if (nodes[i].attributes[j] != NULL) {
                    nodes[i].attributes[j]->printFullAttList();
                    nodes[i].attributes[j]->printFullAttSortedList();
                }
            }
        }
    }

    //Save schema
    void saveSchema(char * baseName){
        char nodesFileName[256];sprintf(nodesFileName,"%s.wt",baseName);
        std::fstream fs;
        fs.open (nodesFileName, std::fstream::in | std::fstream::out | std::fstream::app);
        if (fs.is_open()){
            for (int i = 0; i < wtSimbols.size();i++)
                fs << wtSimbols.at(i);
        }
        fs.close();
        for (int i = 0; i < nodes.size(); i++){
            char attbaseName[256];sprintf(attbaseName,"%s.att_",baseName);
            char numberLabel[10];sprintf(numberLabel,"%d",nodes[i].id);
            strcat(attbaseName,numberLabel);
            for (int j = 0; j < nodes[i].attributes.size();j++){
                if (nodes[i].attributes[j] != NULL) {
                    char attbaseNameNum[256];
                    sprintf(attbaseNameNum, "%s_", attbaseName);
                    char numberAtt[10];
                    sprintf(numberAtt, "%d", j);
                    strcat(attbaseNameNum, numberAtt);
                    nodes[i].attributes[j]->saveValues(attbaseNameNum);
                }
            }
        }
    }

    //Load schema
    void loadAtts(char * baseName){
        char nodesFileName[256];sprintf(nodesFileName,"%s.wt",baseName);
        std::ifstream infile(nodesFileName);
        std::string line;
        std::getline(infile, line);
        for (int i = 0; i < line.size();i++)
            wtSimbols.push_back(line[i]-'0');
        infile.close();
        for (int i = 0; i < nodes.size();i++){
            char attbaseName[256];sprintf(attbaseName,"%s.att_",baseName);
            char numberLabel[10];sprintf(numberLabel,"%d",nodes[i].id);
            strcat(attbaseName,numberLabel);
            for (int j = 0; j < nodes[i].attributes.size();j++){
                if (nodes[i].attributes[j] != NULL) {
                    char attbaseNameNum[256];
                    sprintf(attbaseNameNum, "%s_", attbaseName);
                    char numberAtt[10];
                    sprintf(numberAtt, "%d", j);
                    strcat(attbaseNameNum, numberAtt);
                    nodes[i].attributes[j]->loadValues(attbaseNameNum);
                }
            }
        }
    }
protected:
private:
    int numAtts = 0, id = 0;
    std::vector<node> nodes;
    std::vector<string> labels;
    dyn::wt_str wtSimbols;
};

class SchemaNode:public Schema{
public:
protected:
private:
};

class SchemaEdge:public Schema{
public:
protected:
private:
};
