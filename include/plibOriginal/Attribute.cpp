#include <vector>
#include <string>
#include <algorithm>
#include <boost/container/flat_set.hpp>

class Att;

class Att {
public:
    Att(){}
    virtual int addAtt(std::string val, int node){};
    virtual void printFullAttList(){};
    virtual void sortAttList(){};
    virtual void printFullAttSortedList(){};
    virtual void setStartPos(int pos){};
};

class SparseAtt:public Att{
public:
    SparseAtt():Att(){
    }

    void setStartPos(int pos){}

    void sortAttList(){
        std::sort(attValsSorted.begin(),attValsSorted.end());
    }

    int addAtt(std::string val, int node){
        attVals.push_back(val);
        attValsSorted.push_back(val);
        return 0;
    }
    void printFullAttList(){
        for (int i = 0; i < attVals.size(); i++)
            printf("%s ",attVals[i].c_str());
        printf("\n");
    }
    void printFullAttSortedList(){
        for (int i = 0; i < attVals.size(); i++)
            printf("%s ", attValsSorted[i].c_str());
        printf("\n");
    }
private:
    std::vector<std::string>  attVals, attValsSorted;
};

//Almacenan donde empiezan en las columnas y los distintos valores que toman.
class DenseAtt:public Att{
public:
    DenseAtt():Att(){}

    void sortAttList(){

    }

    void setStartPos(int pos){
        this->startPos = pos;
    }

    int addAtt(std::string val, int node){
        std::vector<std::string>::iterator it =
                std::find(diffValues.begin(),diffValues.end(),val);
        if (it == diffValues.end()){
            diffValues.push_back(val);
            std::vector<int> col;
            col.push_back(node);
            nodesValues.push_back(col);
        }else{
            int pos = it-diffValues.begin();
            nodesValues[pos].push_back(node);
        }
        return 0;
    }

    void printFullAttList(){
        for (int i = 0; i < diffValues.size();i++){
            printf("Value %s\n",diffValues[i].c_str());
            for (int j = 0; j < nodesValues[i].size();j++)
                printf("Nodes %d\n",nodesValues[i][j]);
        }

    };

    void printFullAttSortedList(){}

private:
    int startPos = 0;
    std::vector<std::string> diffValues;
    std::vector<std::vector<int>> nodesValues;
};
