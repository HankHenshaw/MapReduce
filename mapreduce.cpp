#include <iostream>
#include "mapreduce.h"

using namespace std::chrono_literals;

MapReduce::MapReduce(std::string fileName, size_t mnum, size_t rnum, 
        std::function<std::vector<std::string>(std::string)> mapLambda,
            std::function<void(std::string, size_t, size_t)> reduceLambda) 
    : m_fileName(std::move(fileName)), m_numOfMapThreads(mnum), m_numOfReduceThreads(rnum), m_posInVectorOfPos(0),
        m_mapLambda(std::move(mapLambda)),
        m_reduceLambda(std::move(reduceLambda)),
        m_shuffleInt(-1)
{
    m_vecOfWordsAfterShuffle.resize(m_numOfReduceThreads);
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
}

void MapReduce::splitFile()
{
    size_t curPos = m_fin.tellg();

    m_fin.seekg(0, std::ios_base::end);
    auto lastPos = m_fin.tellg();

    if(m_numOfMapThreads == 1)
    {
        m_splitPositions.emplace_back(std::make_pair(curPos, lastPos));
        map(m_splitPositions[0].first, m_splitPositions[1].second);
        return;
    }

    size_t numOfMapThreads = m_numOfMapThreads;
    size_t quotient = lastPos/numOfMapThreads;

    while(numOfMapThreads - 1)
    {
        m_fin.seekg(quotient, std::ios_base::beg);

        while(m_fin.peek() != 32) //TODO: проверка на '\n'?
        {
            m_fin.seekg(1, std::ios_base::cur);

            if(m_fin.peek() == -1)
            {
                throw std::length_error("eof reached\n");
            }
        }

        m_splitPositions.emplace_back(std::make_pair(curPos, m_fin.tellg()));
        m_fin.seekg(1, std::ios_base::cur); // Чтобы пропустить пробел

        curPos = m_fin.tellg();
        --numOfMapThreads;
    }

    m_splitPositions.emplace_back(std::make_pair(curPos, lastPos));

    for(const auto &val: m_splitPositions)
    {
        std::async(std::launch::async, &MapReduce::map, this, val.first, val.second);
    }

    size_t counter = 0;
    for(size_t i = 0; i < m_numOfReduceThreads; ++i)
    {
        counter = i;

        std::async(std::launch::async, &MapReduce::reduce, this, counter);
    }
}

void MapReduce::map(std::streampos begPos, std::streampos lastPos)
{
    std::string curString;

    m_fin.seekg(begPos, std::ios_base::beg);
    while(m_fin.tellg() < lastPos)
    {
        curString.push_back(m_fin.get());
    }

    auto vecOfWords = m_mapLambda(curString);
    std::sort(vecOfWords.begin(), vecOfWords.end());

    m_vecOfWordsAfterMap.emplace_back(std::move(vecOfWords));

    ++m_shuffleInt;

    shuffle(m_shuffleInt);
}

void MapReduce::shuffle(size_t numOfVec)
{
    for(const auto& word: m_vecOfWordsAfterMap[numOfVec])
    {
        auto hash = std::hash<std::string>{}(word);
        
        m_vecOfWordsAfterShuffle[hash%m_numOfReduceThreads].emplace_back(word); 
    }
}

void MapReduce::reduce(size_t numOfVec)
{
    size_t counter = 1;
    auto size = m_vecOfWordsAfterShuffle[numOfVec].size() - 1;
    for(size_t i = 0; i < size; ++i)
    {
        if(m_vecOfWordsAfterShuffle[numOfVec][i] == m_vecOfWordsAfterShuffle[numOfVec][i+1])
        {
            ++counter;
        } else {
            m_reduceLambda(m_vecOfWordsAfterShuffle[numOfVec][i], counter, numOfVec);
            counter = 1;
        }
    }
    m_reduceLambda(m_vecOfWordsAfterShuffle[numOfVec][size], counter, numOfVec);
}
