#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include "read.h"
void checkSeq(std::string &line){
    std::string nucleotydes = "ACGT";
    for (size_t i = 0; i < line.size(); i++)
        if (nucleotydes.find(line[i]) == std::string::npos)
            nucleotydes.erase(i);
}

std::vector<Info> Reads::readFile(char *fileName) {
    std::vector<Info> reads;
    //TODO: FastaRead Support
    readFastq(reads, fileName);

    for (auto &read:reads){
        std::string header = '-'+read.header;
        read.header = '+'+read.header;
        Sequence revComp = read.sequence.reverseComplement();
        reads.push_back(Info(revComp, header, read.id.reverse()));
    }

    for (int i = 0; i < reads.size(); i++)
        printf("%s\n",reads[i].sequence.toString().c_str());
    return reads;
}

size_t Reads::readFastq(vector <Info> &info, char *fileName) {
    std::string line, header;
    int numLine = 0, pos = 0;
    ifstream fastqFile(fileName);
    if (fastqFile.is_open()){
        while ( getline (fastqFile, line)) {
            pos = numLine % 4;
            if (line.back() == '\n')
                line.pop_back();
            if (!pos) {
                if (line.front() == '@')
                    line.erase(0);
                header = line;
            }
            if (pos == 1){
                checkSeq(line);
                char * lineChar = new char[line.size()+1];
                strcpy(lineChar, line.c_str());
                Sequence seq(lineChar);
                Info info1(seq,header, Id(seqId));
                info.push_back(info1);
                seqId+=2;
            }
            numLine++;
        }
        fastqFile.close();
    }else
        cout << "Fail opening\n";
}