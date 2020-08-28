#include <iostream>
#include "mapreduce.h"

MapReduce::MapReduce(std::string fileName, size_t mnum, size_t rnum) 
    : m_fileName(fileName), m_numOfMapThreads(mnum), m_numOfReduceThreads(rnum), m_posInVectorOfPos(0)
{
    m_vecOfWordsAfterShuffle.resize(m_numOfMapThreads); // TODO: Подумать над этим
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

        while(m_fin.peek() != 32) //TODO: проверка на '\n'
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
}

void MapReduce::Map(std::function<std::vector<std::string>(std::string)> map_function)
{
    auto begPos = m_splitPositions[m_posInVectorOfPos].first;
    auto lastPos = m_splitPositions[m_posInVectorOfPos].second;
    ++m_posInVectorOfPos;

    m_fin.seekg(begPos, std::ios_base::beg);

    std::string curString;
    while(m_fin.tellg() < lastPos)
    {
        curString.push_back(m_fin.get());
    }
    
    auto vecOfWords = map_function(curString); // std::move?
    std::sort(vecOfWords.begin(), vecOfWords.end());

    m_vecOfWordsAfterMap.emplace_back(std::move(vecOfWords));

    // TODO: Сделать мультипоточной
    // Построчное извлечение данных
    // Передача этих данных лямбде, лямбду нужно запускать в потоке
    // Лямбда должна разбить строку на слова
}

void MapReduce::shuffle(size_t numOfVec)
{
    for(const auto& word: m_vecOfWordsAfterMap[numOfVec])
    {
        auto hash = std::hash<std::string>{}(word);
        m_vecOfWordsAfterShuffle[hash%m_numOfMapThreads].emplace_back(word); 
        // TODO: обратить внимание, что бы вектора были отсортированы!(проверить, мб ничего дополнительного делать не надо)
    }
    // TODO: Сделать мультипоточным
    // Создать векторов по кол-ву М потоков
    // Примерно равномерно заполнить их словами из векторов с предыдущего шага
    // При этом одинаковые слова обязатель должны быть в 1 векторе
}

void MapReduce::print()
{
    for(const auto& vecOfWords: m_vecOfWordsAfterShuffle)
    {
        std::cout << "\n--------------\n";
        for(const auto& words: vecOfWords)
        {
            std::cout << words << '\n';
        }
    }
}
