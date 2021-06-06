#include <string>
#define D 3

class Kmer{
public:
    Kmer(Sequence seq, size_t kmersize, size_t pos);

    long long HashUniversal(int numHash, int maxSize){
        printf("Así hasheo asi asi\n");
    }

    void inPrint(){
        printf("Values %d\n",_kmer_seq);
    }

    std::string toString(){
        std::string outcome;
        for (int i = 0; i < 32; i++)
            outcome.push_back(
                    tradId((_kmer_seq >> (i % 32) * 2) & 3));
        printf("Printf %s\n",outcome.c_str());
    }
private:
    size_t _kmer_seq;
};

class HashTable{
public:
    HashTable(){}

    long long getMaxValue(){
        return _maxSize;
    }
    void addItem(Sequence seq);
protected:
    long long _maxSize;
};

class KmerProcessor{
public:
    static std::vector<Kmer> countKmers(Info read, HashTable &hashTable, size_t kmerSize);
private:
    KmerProcessor(){}
};