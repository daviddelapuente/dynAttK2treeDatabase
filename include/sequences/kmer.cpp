#include "kmer.h"
//Kmers are reversed so (REMEMBER)
Kmer::Kmer(Sequence seq, size_t kmersize, size_t pos) {
    _kmer_seq = 0;
    for (int i = pos; i < kmersize+pos; i++){
        _kmer_seq <<=2;
        _kmer_seq += seq.getStraight(i);
    }
}

class FrecuencyStimator: public HashTable {
public:
    FrecuencyStimator(float sigma, float phi, int genomeSize){
        _maxSize = (int) genomeSize/sigma;
        std::vector<std::vector<int>> initia(D, std::vector<int>(_maxSize));
        hashTable = initia;
    }

    void addItem(Sequence seq){
        for (int i = 0; i < D; i++)
            printf("Value %d\n",i);
    }

private:
    std::vector<std::vector<int>> hashTable;
    std::vector<Sequence> countSkip;
};

std::vector<Kmer> KmerProcessor::countKmers(Info read, HashTable &hashTable,
                                            size_t kmerSize) {
    std::vector<Kmer> outcomeKmers;
    printf("A contar kmers %s\n",read.sequence.toString().c_str());
    for (size_t i = 0; i < read.sequence.size() - kmerSize; i++){
        Kmer kmer(read.sequence, kmerSize, i);
        kmer.toString();
        outcomeKmers.push_back(kmer);
        kmer.HashUniversal(0,hashTable.getMaxValue());
    }
    return outcomeKmers;
}