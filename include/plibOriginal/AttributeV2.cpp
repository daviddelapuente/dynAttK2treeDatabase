#include <vector>
#include <string>
#include <math.h>
#include <algorithm>
#include <chrono>
#include "k2TreeDyn.cpp"
#include <fstream>

class Att;

struct data{
    char * s;
    int node;
    bool operator < (const data &a) const {
        return *s < (*a.s);
    }
    bool operator > (const data &a) const {
        return *s > *a.s;
    }
    bool operator == (const data &a) const {
        return *s == *a.s;
    }
};

int binarySearch(std::string label, std::vector<data> attValsSorted){
    float posPiv = 0,posInf = 0, posSup = attValsSorted.size(),
            pos = attValsSorted.size()/2.0, trust = 1;
    if (posSup > 0) {
        trust = label.compare(string(attValsSorted[pos].s));
        while (trust) {
            posPiv = pos;
            pos = (trust < 0) ? floor((pos + posInf) / 2.0) : ceil((posSup + pos) / 2.0);
            if (posPiv == pos || pos == posSup || pos == posInf) {
                if (pos == attValsSorted.size())
                    return pos;
                else
                    return pos+(label > string(attValsSorted[pos].s));
            }
            (trust < 0)?posInf =  posInf : posPiv;
            (trust < 0)?posSup =  posPiv : posSup;
            trust = label.compare(string(attValsSorted[pos].s));
        }
    }
    return pos;
}

class Att {
public:
    Att(){}
    virtual void addAtt(std::string, int, bool){};
    virtual std::string getValue(int){};
    virtual void printFullAttList(){};
    virtual void printFullAttSortedList(){};
    virtual void sortValues(){};
    virtual std::vector<int> getNodesWithVal(string){};
    virtual int getByteUsed(){};
    virtual long long int numBytes(){};
    virtual void saveValues(char *fileName){};
    virtual void loadValues(char *fileName){};
};

class SparseAtt:public Att{
public:
    SparseAtt():Att(){
    }

    void addAtt(std::string val, int node, bool insert){
        char *y = new char[val.size()+1];
        strcpy(y,val.c_str());
        attVals.push_back(y);
        data a = {y, node};
        attValsSorted.push_back(a);
    }

    void sortValues(){
        std::sort(this->attValsSorted.begin(),this->attValsSorted.end());
    }

    void printFullAttList(){
        for (int i = 0; i < attVals.size(); i++)
            printf("%s ",attVals[i]);
        printf("\n");
    }

    void printFullAttSortedList(){
        for (int i = 0; i < attVals.size(); i++)
            printf("%s ", attValsSorted[i].s);
        printf("\n");
    }

    //Recovery
    std::string getValue(int pos){
        return attVals[pos];
    }

    std::vector<int> getNodesWithVal(string attVal){
        std::vector<int> output;
        int pos = binarySearch(attVal,attValsSorted);

        if (pos == attValsSorted.size() && string(attValsSorted[pos-1].s).compare(attVal))
            return output;
        if (pos == attValsSorted.size())
            pos--;
        if (string(attValsSorted[pos].s).compare(attVal))
            return output;
        output.push_back(attValsSorted[pos].node);
        int posPiv = pos;
        for (int i = -1; i < 2; i+=2) {
            pos = posPiv+i;
            while (true) {
                if ((pos) < 0 || (pos) >= (attValsSorted.size()) || string(attValsSorted[pos].s).
                        compare(attVal))
                    break;
                output.push_back(attValsSorted[pos].node);
                pos+=i;
            }
        }
        return output;
    }

    //save
    void saveValues(char * fileName){
        std::fstream fs;
        fs.open (fileName, std::fstream::in | std::fstream::out | std::fstream::app);
        if (fs.is_open()) {
            for (int i = 0; i < attVals.size(); i++)
                fs << attVals[i] << "\t";
            fs << "\n";
            for (int i = 0; i < attValsSorted.size(); i++)
                fs << attValsSorted[i].s<<'|'<<attValsSorted[i].node << "\t";
        }else
            printf("Cerrado\n");

        fs.close();
    }

    //load
    void loadValues(char * fileName){
        printf("Not supported yet\n");
       /* std::ifstream infile(fileName);
        std::string line;
        for (int i = 0; i < 2; i++){
            std::getline(infile, line);
            std::string token;
            vector<string> tokens;
            istringstream iss(line);
            while(std::getline(iss, token, '\t'))
                tokens.push_back(token);
            for (int j = 0; j < tokens.size();j++)
                if (!i)
                    attVals.push_back(tokens[j]);
                else {
                    vector<string> tokens2;string token2;
                    istringstream iss2(tokens[j]);
                    while(std::getline(iss2, token2, '|'))
                        tokens2.push_back(token2);
                    data a = {new string(attVals[stoi(tokens2[1])]),stoi(tokens2[1])};
                    attValsSorted.push_back(a);
                }
        }
        infile.close();*/
    }

    //Size
    long long int numBytes(){
        long long int bytes = 0;
        for (uint i = 0; i < attVals.size(); i++)
            bytes += strlen(attVals[i]);
        return 5*attValsSorted.size()+bytes;
    }
private:
    std::vector<char *>  attVals;
    std::vector<data> attValsSorted;
};

//Almacenan donde empiezan en las columnas y los distintos valores que toman.
class DenseAtt:public Att{
public:
    DenseAtt():Att(){}

    void addAtt(std::string val, int node, bool insert){
       std::vector<std::string>::iterator it =
                std::find(diffValues.begin(),diffValues.end(),val);
        int index[2] = {node,it-diffValues.begin()};
        if (it == diffValues.end())
            diffValues.push_back(val);
        this->tree.insert(0,index);
    }

    void printFullAttList(){
        for (int i = 0; i < diffValues.size();i++)
            printf("Value %s\n",diffValues[i].c_str());
        this->tree.printTL();
    }

    std::string getValue(int pos){
        int r1[2] = {pos,pos}, r2[2] = {0,diffValues.size()-1};
        std::vector<std::vector<int>> neighbors = tree.rangeQuery(r1,r2);
        return diffValues[neighbors[0][0]];
    }

    std::vector<int> getNodesWithVal(string attVal){
        std::vector<int> output;
        std::vector<std::string>::iterator it = std::find(diffValues.begin(),
                                                          diffValues.end(),attVal);
        if (it == diffValues.end())
            return output;
        int q1[2] = {0,tree.getMatrixSize()-1},q2[2] = {it-diffValues.begin(),it-diffValues.begin()};
        std::vector<std::vector<int>> rangeQuery = tree.rangeQuery(q1,q2);
        for (int i = 0; i < rangeQuery.size(); i++)
            if (rangeQuery[i].size())
                output.push_back(i);
        return output;
    }

    void saveValues(char *fileName){
        std::fstream fs;
        fs.open (fileName, std::fstream::in | std::fstream::out | std::fstream::app);
        if (fs.is_open()) {
            for (int i = 0; i < diffValues.size(); i++)
                fs << diffValues[i] << "\t";
            fs << "\n";
            this->tree.saveTL(fileName);
        }else
            printf("Cerrado\n");

        fs.close();
    }

    void loadValues(char * fileName){
        std::ifstream infile(fileName);
        std::string line;
        for (int i = 0; i < 2; i++){
            if (!i) {
                std::getline(infile, line);
                int multi = stoi(line);
                std::getline(infile, line);
                this->tree.setTL(line);
                std::getline(infile, line);
                this->tree.setMatrizSize(stoi(line));
            }else{
                std::getline(infile, line);
                string token;
                vector<string> tokens;
                istringstream iss(line);
                while(std::getline(iss, token, '\t'))
                    tokens.push_back(token);
                for (int j = 0; j < tokens.size(); j++)
                    diffValues.push_back(tokens[j]);
            }
        }
        while (std::getline(infile, line)){
            printf("%s\n",line.c_str());
        }
        infile.close();
    }

    long long int numBytes(){
        long long int bytes = 0;
        for (int i = 0; i < diffValues.size(); i++)
            bytes += diffValues[i].size();
        return bytes+tree.numBytes();
    }

private:
    std::vector<std::string> diffValues;
    TreeDyn tree = TreeDyn(2);
};
