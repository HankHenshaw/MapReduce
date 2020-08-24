#include <iostream>
#include "mapreduce.h"

MapReduce::MapReduce(std::string fileName, size_t mnum, size_t rnum) 
    : m_fileName(fileName), m_numOfMapThreads(mnum), m_numOfReduceThreads(rnum)
{
    openFile();
}

MapReduce::~MapReduce()
{
    m_fin.close();
}

void MapReduce::openFile()
{
    m_fin.open(m_fileName);
    if(!m_fin.is_open())
    {
        throw std::invalid_argument("Invalid Filename\n");
    }

    Map();
}

void MapReduce::Map() // TODO: Почему-то последняя строка в файле дублируется
{
    std::string tmp;
    while(m_fin)
    {
        m_fin >> tmp;
        m_hash[tmp] += 1;
    }

//    printHash();
}

void MapReduce::printHash() // Remove
{
    auto itb = m_hash.begin();
    while(itb != m_hash.end())
    {
        std::cout << "Key: " << itb->first << "|Value: " << itb->second << '\n';
        ++itb; 
    }
}

