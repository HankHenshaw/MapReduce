#include <iostream>
#include <thread>
#include <future>
#include "mapreduce.h"

MapReduce::MapReduce(std::string fileName, size_t mnum, size_t rnum, 
        std::function<std::vector<std::string>(std::string)> mapLambda,
        std::function<void(std::string, size_t)> reduceLambda) 
    : m_fileName(std::move(fileName)), m_numOfMapThreads(mnum), m_numOfReduceThreads(rnum), m_posInVectorOfPos(0),
        m_mapLambda(std::move(mapLambda)),
        m_reduceLambda(std::move(reduceLambda))
{
    m_vecOfWordsAfterShuffle.resize(m_numOfReduceThreads); // TODO: Подумать над этим
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
        map(m_mapLambda, m_splitPositions[0].first, m_splitPositions[1].second);
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

    //TODO: Дождаться окончания работы потоков!
    for(const auto &val: m_splitPositions)
    {
        std::async(std::launch::async, &MapReduce::map, this, m_mapLambda, val.first, val.second);
    }

    for(size_t i = 0; i < m_numOfReduceThreads; ++i)
    {
        std::async(std::launch::async, &MapReduce::reduce, this, m_reduceLambda, i);
    }
}

void MapReduce::map(std::function<std::vector<std::string>(std::string)> map_function, std::streampos begPos, std::streampos lastPos)
{
    std::string curString;

    /*Mutex?*/
    m_fin.seekg(begPos, std::ios_base::beg);
    while(m_fin.tellg() < lastPos)
    {
        curString.push_back(m_fin.get());
    }
    /*Mutex?*/

    auto vecOfWords = map_function(curString); // std::move?
    std::sort(vecOfWords.begin(), vecOfWords.end());

    m_vecOfWordsAfterMap.emplace_back(std::move(vecOfWords));

    // TODO: Сделать мультипоточной
    // Построчное извлечение данных
    // Передача этих данных лямбде, лямбду нужно запускать в потоке
    // Лямбда должна разбить строку на слова

    /*Mutex!*/
    shuffle(m_shuffleInt++); // Тут будет гонка если не блокировать, надо подумать что делать
    /*Mutex!*/
}

void MapReduce::shuffle(size_t numOfVec)
{
    for(const auto& word: m_vecOfWordsAfterMap[numOfVec])
    {
        auto hash = std::hash<std::string>{}(word);
        m_vecOfWordsAfterShuffle[hash%m_numOfReduceThreads].emplace_back(word); 
        // TODO: обратить внимание, что бы вектора были отсортированы!(проверить, мб ничего дополнительного делать не надо)
    }
    // TODO: Сделать мультипоточным
    // Создать векторов по кол-ву М потоков
    // Примерно равномерно заполнить их словами из векторов с предыдущего шага
    // При этом одинаковые слова обязатель должны быть в 1 векторе
}

void MapReduce::reduce(std::function<void(std::string, size_t)> reduce_function, size_t numOfVec)
{
    for(const auto &val: m_vecOfWordsAfterShuffle)
    {
        reduce_function(val[numOfVec], numOfVec);
    }
    //TODO: Сделать мультипоточным
    //Вывести в файл данные в виде [кол-во этого слова][слово]
    // std::ofstream fout;
    // fout.open(std::to_string(numOfVec+1) + "_reduce.txt", std::ios_base::app | std::ios_base::out);
    // if(fout.is_open())
    // {
    //     auto counter = 1;
    //     auto size = m_vecOfWordsAfterShuffle[numOfVec].size();
    //     for(size_t i = 0; i < size - 1; ++i)
    //     {
    //         if(m_vecOfWordsAfterShuffle[numOfVec][i] == m_vecOfWordsAfterShuffle[numOfVec][i+1])
    //         {
    //             ++counter;
    //         } else {
    //             fout << counter << ' ' << m_vecOfWordsAfterShuffle[numOfVec][i] << '\n';
    //             counter = 1;
    //         }
    //     }
    //     fout << counter << ' ' << m_vecOfWordsAfterShuffle[numOfVec][size - 1] << '\n';
    //     fout.close();
    // } else {
    //     throw std::ios_base::failure("can't open file");
    // }

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

// https://habr.com/ru/post/103467/
// https://www.codeinstinct.pro/2012/08/mapreduce-design.html
// http://blogerator.org/page/objasnjaem-sut-mapreduce-na-palcah-nosql-highload