#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
char tradId(size_t id){
    switch (id) {
        case 0:
            return 'A';
        case 1:
            return 'C';
        case 2:
            return 'G';
        case 3:
            return 'T';
    }
}
class Sequence{
private:
    int nuclPerBit = sizeof(size_t)*8/2;
    struct sequence {
        size_t use = 0;
        size_t length = 0;
        std::vector<size_t> nucleotides;
    };
    size_t tradNucl(char c){
        switch (c){
            case 'A':
            case 'a':
                return 0;
            case 'C':
            case 'c':
                return 1;
            case 'G':
            case 'g':
                return 2;
            case 'T':
            case 't':
                return 3;
            default:
                return 0;
        }
    }

public:
    Sequence():_complement(false) {
        _sequence = new sequence;
        _sequence->use++;
    }

    Sequence(const Sequence &seq):_sequence(seq._sequence),
                                  _complement(seq._complement){
        _sequence->use++;
    }

    //Ojito: Here the values are in order (REMEMBER)
    Sequence(char * chain):_complement(false){
        std::string __file(chain);
        if (__file.empty()) return;
        _sequence = new sequence;
        _sequence->length = __file.size();
        _sequence->nucleotides.assign((__file.size()-1)/nuclPerBit+1,0);
        for (size_t i = 0; i < __file.size();i++){
            size_t val = tradNucl(chain[i]);
            _sequence->nucleotides[i/nuclPerBit] |= val << (i % nuclPerBit) * 2;
        }
        _sequence->use = 1;
    }

    Sequence& operator=(const Sequence seq){
        --_sequence->use;
        if (!--_sequence->use) delete _sequence;
        _complement = seq._complement;
        _sequence = seq._sequence;
        ++_sequence->use;
        return *this;
    }

    size_t size(){
        return _sequence->length;
    }

    //Get the sequence parts
    char get(size_t index){
        if (_complement){
            index = _sequence->length - index - 1;
            return tradId(~((_sequence->nucleotides[index / nuclPerBit]
                    >> (index % nuclPerBit)*2)&3)&3);}
        return tradId((_sequence->nucleotides[index / nuclPerBit]
                >> (index % nuclPerBit)*2)&3);
    }

    char getStraight(size_t index){
        if (_complement)
            index = _sequence->length - index - 1;
        size_t id = (_sequence->nucleotides[index/nuclPerBit] >>
            (index % nuclPerBit)*2) & 3;
        return !_complement ? id: ~ id & 3;
    }

    string toString(){
        std::string outcome;
        for (size_t i = 0; i < _sequence->length; i++)
            outcome.push_back(this->get(i));
        outcome.shrink_to_fit();
        return outcome;
    }

    Sequence reverseComplement() {
        Sequence compleSeq(*this);
        compleSeq._complement = true;
        return compleSeq;
    }

private:
    sequence* _sequence;
    bool _complement;
};