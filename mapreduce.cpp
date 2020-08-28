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

    splitFile();
//    Map();
}

void MapReduce::splitFile()
{
    size_t curPos = m_fin.tellg();

    m_fin.seekg(0, std::ios_base::end);
    auto lastPos = m_fin.tellg();

    if(m_numOfMapThreads == 1)
    {
        m_splitPositions.emplace_back(std::make_pair(curPos, lastPos));
        return;
    }

    size_t numOfMapThreads = m_numOfMapThreads;
    size_t quotient = lastPos/numOfMapThreads;

    while(numOfMapThreads - 1)
    {
        m_fin.seekg(quotient, std::ios_base::beg);

        while(m_fin.peek() != 32)
        {
            m_fin.seekg(1, std::ios_base::cur);

            if(m_fin.peek() == -1)
            {
                throw std::length_error("eof reached\n");
            }
        }

        m_splitPositions.emplace_back(std::make_pair(curPos, m_fin.tellg()));

        curPos = m_fin.tellg();
        --numOfMapThreads;
    }

    m_splitPositions.emplace_back(std::make_pair(curPos, lastPos));
}

void MapReduce::Map() // TODO: Почему-то последняя строка в файле дублируется
{
    std::string tmp;
    while(m_fin)
    {
        std::cout << "File pos:" << m_fin.tellg() << '\n';
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
