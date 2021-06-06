#include <vector>
#include <string>
#include <limits>
#include "dnaSeq.h"

class Id
{
public:
    Id(int id):_id(id){}

    //TODO: Remember the case where the strand is not the standar is the reverse (-_id%2*2)
    Id reverse(){
        return Id(_id+1);
    }

    //TODO: Change
    size_t hash(){
        size_t x = _id;
        size_t z = (x += 0x9E3779B97F4A7C15ULL);
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        return z ^ (z >> 31);
    }
private:
    int _id;
};

struct Info{
    Id id;
    Sequence sequence;
    std::string header;
    Info(Sequence &seq, std::string header, Id id):id(id),sequence(seq),header(header)
    {}
};

class Reads
{
public:
    Reads(){}
    std::vector<Info> readFile(char *);
private:
    size_t readFasta(vector<Info> &info, char * fileName);
    size_t readFastq(vector<Info> &info, char * fileName);
    size_t seqId;
};
