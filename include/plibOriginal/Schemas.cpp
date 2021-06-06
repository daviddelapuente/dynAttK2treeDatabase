#include "../dynamic.hpp"
#include "Attribute.cpp"
#include <boost/container/flat_set.hpp>

//Para los densos -> contador de numero de densos diferentes actuales
struct node{
    string labelName;
    int numNodes;
    std::vector<Att*> attributes;
};
class Schema;

class Schema {
public:
    void setVector(string label, int num){
        nodes.push_back(node());
        nodes[nodes.size()-1].labelName = label;
        nodes[nodes.size()-1].numNodes = currentNumNodes+num-1;
        oc.insert(currentNumNodes+num-1);
        currentNumNodes += num;
        printf("New Info: %s %d\n",nodes[nodes.size()-1].labelName.c_str(),
               nodes[nodes.size()-1].numNodes);
    }

    void setNumAtt(int numAtts){
        this->numAtts = numAtts;
    }

    void createAttributes(int nodeType, int attType, int numAtt){
        printf("Attribute type %d %s %d\n", attType, nodes[nodeType].labelName.c_str(),
            numAtt);
        if (attType) {
            std::vector<int>::iterator it =
                    std::find(idDenseAtts.begin(),idDenseAtts.end(),numAtt);
            if (it == idDenseAtts.end()) {
                printf("New dense att %d\n");
                idDenseAtts.push_back(numAtt);
                denseAtts.push_back(new DenseAtt());
                nodes[nodeType].attributes.push_back(denseAtts[denseAtts.size()-1]);
            }else{
                int pos = it-idDenseAtts.begin();
                printf("Num att %d pos %d\n",numAtt,pos);
                nodes[nodeType].attributes.push_back(denseAtts[pos]);
            }
        }else
            nodes[nodeType].attributes.push_back(new SparseAtt());
    }

    void modAttributesList(vector<string> tokens){
        int size = tokens.size(), ret = 0, att = 0;
        string  s = "-";
        for (int i = 1; i < size; i++) {
            printf("Tokens[i] %s, i %d, att %d, actualType %d, length %d\n",
                tokens[i].c_str(),i,att,actualType, nodes[actualType].attributes.size());
            if (!(tokens[i] == s)) {
                ret += nodes[actualType].attributes[att]->addAtt(tokens[i],stoi(tokens[0]));
                att++;
            }
        }
        actualNode++;
        if (actualNode > nodes[actualType].numNodes) {
            for (int i = 0; i < nodes[actualType].attributes.size(); i++) {
                nodes[actualType].attributes[i]->sortAttList();
                nodes[actualType].attributes[i]->printFullAttList();
                nodes[actualType].attributes[i]->printFullAttSortedList();
            }
            actualType++;
        }
    }

protected:
private:
    int currentNumNodes = 0, actualNode = 0, actualType = 0, numAtts = 0;
    std::vector<node> nodes;
    std::vector<int> idDenseAtts;
    std::vector<Att*> denseAtts;
    boost::container::flat_set<int> oc;
};

class SchemaNode:public Schema{

private:
};