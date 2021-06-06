#include "../dynamic.hpp"
#include <string.h>
#include <chrono>
#include <math.h>
#include <fstream>
#include <bitset>
#define numBits 17
int it = 0;
typedef unsigned int uint;
/*struct subject{
    std::vector<string> edges;
};*/

class TreeDyn {
public:

void initialize(){
    int f = this->k;
    for (int i = 0; i < f*f; i++)
        this->tl.push_front(false);
}

TreeDyn() {}
TreeDyn(char * baseName):k(2) {
    char treeName[256];sprintf(treeName,"%s.lnk.tree",baseName);
    loadTL(treeName);
}

TreeDyn(int f, int s, bool multi):k(f),matrixSize(s),multiEdge(multi){
    initialize();
}

TreeDyn(int f, bool multi):k(f),matrixSize(f),multiEdge(multi) {
    initialize();
}

TreeDyn(int f):k(f),matrixSize(f),multiEdge(false){
    initialize();
}

TreeDyn(int f, int s):k(f),matrixSize(s){
    initialize();
}

long long int numBytes() {
    long long int bytes = 9;
    bytes += tl.bit_size()/8+multi.bit_size()/8;
    bytes += sizeof(std::vector<string>());
    for (int i = 0; i < first.size();i++)
        bytes += first[i].size()*sizeof(string);
    return bytes;
}

void set(int k, int matrixSize, bool multi){
    this->k = k;this->matrixSize = matrixSize;this->multiEdge = multi;
    this->initialize();
}

int getMatrixSize(){
    return this->matrixSize;
}

int * getAllTree(){
    int * results = (int*) malloc(this->tl.size()*sizeof(int));
    for (uint i = 0; i < this->tl.size();i++)results[i] = this->tl.at(i);
    return results;
}

int getSize(){
    return this->tl.size();
}

void printTL(){
    printf("TL\n");
    for (uint i = 0; i < this->tl.size(); i++) {
        if (!(i % (k*k)))
            printf("   ");
        printf("%d ", this->tl.at(i));
    }
    printf("\n");
    if (this->multiEdge) {
        printf("Multi\n");
        for (uint i = 0; i < this->multi.size(); i++)
            printf("%d ", this->multi.at(i));
        printf("\n");
        printf("First\n");
        for (uint i = 0; i < this->first.size(); i++) {
            if (stoi(first[i],nullptr,2) == 0 ) {
                printf("Next\n");
                int posMulti = multi.rank1(i);
                for (uint j = 0; j < next[posMulti].size(); j++)
                    printf("%d ", stoi(next[posMulti][j],
                                       nullptr,2)-1);
                printf("\n");
            }else
                printf("%d\n",stoi(this->first[i],
                                   nullptr,2)-1);
        }
        printf("\n");
    }
}

void saveTL(char * fileName){
    std::fstream fs;
    fs.open (fileName, std::fstream::in | std::fstream::out | std::fstream::app);
    if (fs.is_open()) {
        if (this->multiEdge)
            fs << "1\n";
        else
            fs << "0\n";
        for (uint i = 0; i < tl.size();i++)
            fs << tl.at(i);
        fs << "\n";
        if (multiEdge){
            for (uint i = 0; i < multi.size();i++)
                fs << multi.at(i);
            fs << "\n";
            for (uint i = 0; i < first.size();i++){
                for (uint j = 0; j < first[i].size();j++)
                    fs << first[i][j] << '\t';
                fs << "\n";
            }
        }
        fs << matrixSize << '\n';
    }else
        printf("Cerrado\n");

    fs.close();
}

void loadTL(char * fileName){
    printf("Not supported yet\n");
    /*std::ifstream infile(fileName);
    std::string line;
    std::getline(infile,line);
    multiEdge = (stoi(line));
    std::getline(infile,line);
    for (int i = 0; i < line.size(); i++)
        tl.push_back(line[i]-'0');
    if (multiEdge) {
        std::getline(infile, line);
        for (int i = 0; i < line.size(); i++)
            multi.push_back(line[i] - '0');
        for (int i = 0; i < multi.size(); i++) {
            std::getline(infile, line);
            string token;
            vector <string> tokens;
            std::vector<string> edges;
            istringstream iss(line);
            while (std::getline(iss, token, '\t'))
                tokens.push_back(token);
            for (string s:tokens)
                edges.push_back(s);
            first.push_back(edges);
        }
    }
    std::getline(infile,line);
    matrixSize = stoi(line);
    infile.close();*/
}

void setTL(string tl){
    for (int i = 0; i < tl.size(); i++) {
        bool j = (int(tl[i])==49);
        if (i < k*k)
            this->tl.set(i,j);
        else
            this->tl.push_back(j);
    }
}

void setMulti(string multi){
    for (int i = 0; i < multi.size(); i++) {
        bool j = (int(multi[i]) == 49);
        if (i < k*k)
            this->multi.set(i,j);
        else
            this->multi.push_back(j);
    }
}

void setMatrizSize(int size){
    this->matrixSize = size;
}

void insertKzeros(int posOne){
    for (int i = 0; i < k*k; i++)
        this->tl.push_back(false);
}

void insertKzeros(int zeros, int posOne){
    for (int i = 0; i < this->k*this->k; i++)
        this->tl.insert0(zeros);
}

void updateSubStruct(int pos, int edgeId){
    if (!this->multiEdge)
        return;
    int numOnesTot = this->tl.rank1(), numOnesTilPos = this->tl.rank1(pos)+1, place = 0;
    place = !((this->multi.size()-numOnesTot+numOnesTilPos-1) > 0) ? 0:
            this->multi.size()-numOnesTot+numOnesTilPos-1;
    std::string edgeIdString = std::bitset<numBits>(edgeId+1).to_string();
    edgeIdString.shrink_to_fit();
    if (this->tl.at(pos)){
        if (!this->multi[place]) {
            int posMulti = multi.rank1(place);
            this->multi[place] = 1;
            if (posMulti >= next.size())
                next.push_back(std::vector<string>());
            else
                next.insert(next.begin()+posMulti,std::vector<string>());
            next[posMulti].push_back(first[place]);
            next[posMulti].push_back(edgeIdString);
            first[place] = std::bitset<numBits>(0).to_string();
        }else {
            int posMulti = multi.rank1(place);
            next[posMulti].push_back(edgeIdString);
        }
    }else{
        this->multi.insert0(place);
        if (first.begin()+place >= first.end())
            first.push_back(edgeIdString);
        else
            first.insert(first.begin() + place, edgeIdString);
    }
}

void checkParams(int tokens[2]) {
    while (tokens[0] >= this->matrixSize || tokens[1] >= this->matrixSize) {
        this->matrixSize *= 2;
        if (this->tl.rank1()) {
            for (int i = 0; i < (k*k) - 1; i++)
                this->tl.push_front(false);
            this->tl.push_front(true);
        }
    }
}

void insert(int edgeId, int param[2]) {
    checkParams(param);
    int *origin = (int*)malloc(sizeof(int)*2);
    origin[0] = 0; origin[1] = 0;
    int size = this->matrixSize, *rowOffset = (int*) malloc(sizeof(int)*4);
    rowOffset[0] =0,rowOffset[1] =0, rowOffset[2] = 1, rowOffset[3] = 1;
    int *colOffset = (int*) malloc(sizeof(int)*4);
    colOffset[0]=0,colOffset[1]=1,colOffset[2]=0,colOffset[3]=1;
    int pos = 0, k2 = this->k*this->k;
    int quadrant = findSubMatrix(param,size,origin);
    while (size != this -> k){
        pos +=quadrant;
        if (this->tl.at(pos) == 1)
            pos = (this->tl.rank1(pos)+1)*k2;
        else{
            tl[pos] = 1;
            pos = (this->tl.rank1(pos)+1)*k2;
            if ( pos >= tl.size())
                insertKzeros(pos);
            else
                insertKzeros(pos,pos+quadrant);
        }
        size /= 2;
        origin[0] += rowOffset[quadrant]*size;
        origin[1] += colOffset[quadrant]*size;
        quadrant = findSubMatrix(param,size,origin);
    }
    updateSubStruct(pos+quadrant, edgeId);
    this->tl[pos+quadrant] = 1;
    free(origin);free(rowOffset);free(colOffset);
}

void insert(std::vector<string> tokens){
    int *params = (int *) malloc(2*sizeof(int));
    params[0] =stoi(tokens[1]);params[1] = stoi(tokens[2]);
    insert(stoi(tokens[0]),params);
    free(params);
}

void fitMem(){
    if (multiEdge) {
        for (int i = 0; i < first.size(); i++)
            first[i].shrink_to_fit();
        for (int i = 0; i < next.size(); i++)
            next[i].shrink_to_fit();
        first.shrink_to_fit();
        next.shrink_to_fit();
    }
}

int findSubMatrix(int pos[2], int sizeMatrix, int origin[2]){
    int sizePart = sizeMatrix / this->k, row = pos[0], col = pos[1], quadrant = 0;
    for (int i = origin[0]; i < origin[0]+sizeMatrix; i+=sizePart)
        for (int j = origin[1]; j < origin[1]+sizeMatrix; j+=sizePart)
            if (row < i + sizePart and col < j + sizePart)
                return quadrant;
            else
                quadrant++;
    return 0;
}

bool isLeaf(int nIndex){
    return  (((this->tl.rank1(nIndex+k*k))*(k*k)) >= this->tl.size());
}

int findPlace(int node, int size, int k){
    return  (size<=k)?node:floor(node/(size/k))*k;
}

void recursiveTransverse(std::vector<int> &neighbors,
                                  int node, int size, int tlP, int col){
    int quad = findPlace(node,size,k);
    if (this->isLeaf(tlP)){
        for (int i = 0;i < k;i++)
            if (this->tl.at(tlP+quad+i))
                neighbors.push_back(col + quad + i);
    }else{
        for (int i = 0;i< k;i++) {
            int send = col+i*size/2;
            if (this->tl.at(tlP + quad + i)) {
                recursiveTransverse(neighbors, node - floor((quad / k) * (size / k))
                        , size / 2,tl.rank1(tlP + quad + i + 1) * (k * k), send);
            }
        }
    }
}

std::vector<int> neighbors(int node){
    std::vector<int> neighbors;
    int origin = 0;
    recursiveTransverse(neighbors,node, this->matrixSize, 0, origin);
    return neighbors;
}

bool isNeighbor(int node1, int node2){
    int nodes[2] = {node1,node2}, origin[2] = {0,0}, size = this->matrixSize, pos = 0,
            rowOffset[4] ={0,0,1,1},colOffset[4] ={0,1,0,1};
    int quadrant = findSubMatrix(nodes,size,origin);
    while (size != this->k){
        pos += quadrant;
        if (!tl.at(pos))
            return 0;
        else {
            pos = tl.rank1(pos+1)*k*k;
            size /= 2;origin[0] += rowOffset[quadrant]*size;
            origin[1]+= colOffset[quadrant]*size;
            quadrant = findSubMatrix(nodes,size,origin);
        }
    }
    if (!tl.at(pos+quadrant))
        return 0;
    else
        return 1;
}

void rangeQueryInt(std::vector<std::vector<int>> &neighbors, int nodes[2],
                                int connected[2], int origin[2], int size, int tlP, int pos){
    auto start = std::chrono::high_resolution_clock::now();
    int quadrant = 0,rowOffset[4] = {0,0,1,1},
            colOffset[4] = {0,1,0,1}, corner[2] = {nodes[0],connected[0]};
    int quadrantStore = findSubMatrix(corner,size,origin);
    size /=2;
    origin[0] += size * rowOffset[quadrantStore];origin[1] += size * colOffset[quadrantStore];
    if (!this->isLeaf(tlP)) {
        if (origin[1]+size > connected[1]) {
            if ( origin[0]+size > nodes[1]) {
                if (tl.at(tlP + quadrantStore))
                    rangeQueryInt(neighbors, nodes, connected, origin, size,
                                  tl.rank1(tlP + quadrantStore + 1) * (k * k), pos);
            }else{
                int q[2][2] = {{nodes[0],origin[0]+size-1},{origin[0]+size,nodes[1]}},
                        ori[2][2] = {{origin[0],origin[1]},{origin[0]+size,origin[1]}};
                for (int i= 0;i < k; i++)
                    if (tl.at(tlP+quadrantStore+i*k))
                        rangeQueryInt(neighbors, q[i], connected, ori[i], size,
                                      tl.rank1(tlP + quadrantStore + 1 + i * k) * (k * k),
                                      (i % 2) ? pos + size - nodes[0] % size : pos);
            }
        }else{
            if (origin[0]+size > nodes[1]){
                int q[2][2] = {{connected[0], origin[1]+size-1},{origin[1]+size,connected[1]}},
                        ori[2][2] = {{origin[0],origin[1]},{origin[0],origin[1]+size}};
                for (int i = 0; i < k; i++)
                    if (tl.at(tlP+quadrantStore+i))
                        rangeQueryInt(neighbors, nodes,q[i],ori[i],size,tl.rank1(tlP+quadrantStore+1+i)*(k*k),pos);
            }else{
                int  ori[4][2] = {{origin[0],origin[1]},{origin[0],origin[1]+size},{origin[0]+size,origin[1]},{origin[0]+size,origin[1]+size}},
                        q[4][2]={{nodes[0], origin[0]+size-1},{nodes[0], origin[0]+size-1},{origin[0]+size,nodes[1]},{origin[0]+size,nodes[1]}},
                        qA[4][2] = {{connected[0], origin[1]+size-1},{origin[1]+size,connected[1]},{connected[0], origin[1]+size-1},{origin[1]+size,connected[1]}};
                for (int i = 0; i < k*k; i++)
                    if (tl.at(tlP+quadrantStore+i)) {
                        rangeQueryInt(neighbors, q[i], qA[i], ori[i], size, tl.rank1(tlP + quadrantStore + i+1) * (k * k),
                                      (i >= 2)?pos+size-nodes[0]%size:pos);
                    }
            }
        }
    }else{
        for (int i = nodes[0]%k; i < nodes[1]%k+1; i++)
            for (int j = connected[0]%k; j < connected[1]%k+1; j++)
                if (tl.at(tlP + i * k + j)) {
                    if (!multiEdge) {
                        neighbors[pos + i - nodes[0] % k].push_back(connected[0] + j - connected[0] % k);
                    }else{
                        int placeInMulti = (multi.size()-(tl.rank1(tl.size())-tl.rank1(tlP + i * k + j+1)+1));
                        int posMulti = multi.rank1(placeInMulti);
                        if (multi.at(placeInMulti)){
                            neighbors[pos + i - nodes[0] % k].push_back(connected[0] + j - connected[0] % k);
                            //neighbors[pos + i - nodes[0] % k].push_back(first[pos+i-nodes[0]%k].size());
                            neighbors[pos+i-nodes[0]%k].push_back(next[posMulti].size());
                            for (int z = 0; z < next[posMulti].size(); z++)
                                neighbors[pos + i - nodes[0] % k].push_back(stoi(next[posMulti][z],
                                                                                 nullptr,2)-1);
                        }else {
                            neighbors[pos+i-nodes[0]%k].push_back(connected[0] + j - connected[0] % k);
                            neighbors[pos + i - nodes[0] % k].push_back(1);
                            neighbors[pos + i - nodes[0] % k].push_back(stoi(first[placeInMulti],
                                                                             nullptr,2)-1);
                        }
                    }
                }
    }
}

std::vector<std::vector<int>> rangeQuery(int nodes[2],
                                                  int connected[2]){
    it++;
    std::vector<std::vector<int>> v(nodes[1]-nodes[0]+1);
    int ori[2] = {0,0};
    rangeQueryInt(v,nodes,connected,ori,matrixSize,0,0);
    return v;
}
private:
    int k, matrixSize;
    dyn::suc_bv tl, multi;
    std::vector<string> first;
    std::vector<std::vector<string>> next;
    bool multiEdge = false;
};