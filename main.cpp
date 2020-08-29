#include <iostream>
#include <string>
#include <thread>
#include "mapreduce.h"

int main(int argc, char *argv[])
{
    size_t rnum = 1;
    size_t mnum = 1;
    std::string filename;
    if(argc > 4 || argc < 4)
    {
        std::cout << "Usage: <src> <mnum> <rnum> [src - name of text file, " << 
                    "mnum - number of threads for map, rnum - number of threads for reduce]\n";

        return 1;
    } else {
        filename = argv[1];
        mnum = atoi(argv[2]);
        rnum = atoi(argv[3]);
        if(mnum > std::thread::hardware_concurrency())
        {
            mnum = std::thread::hardware_concurrency();
        }
        if(rnum > std::thread::hardware_concurrency())
        {
            rnum = std::thread::hardware_concurrency();
        }
    }

    auto mapLambda = [](std::string string)
    {
        std::vector<std::string> vecOfWords;

        size_t pos = string.find(' ');
        size_t begPos = 0;

        while(pos != std::string::npos)
        {
            vecOfWords.emplace_back(string.substr(begPos, pos - begPos));
            begPos = pos + 1;

            pos = string.find(' ', begPos);
        }

        vecOfWords.emplace_back(string.substr(begPos, pos - begPos));

        return vecOfWords;
    };

    auto reduceLambda = [](std::string string, size_t num)
    {
        std::ofstream fout;
        fout.open(std::to_string(num+1) + "_reduce.txt", std::ios_base::app | std::ios_base::out);
        if(!fout.is_open())
        {
            throw std::ios_base::failure("can't open file");
        } else {
            // TODO!
        }
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
    };

    //TODO: Получать пользовательскую лямбду для map и для reduce в к-торе
    MapReduce map_reduce(filename, mnum, rnum, mapLambda, reduceLambda);

    return 0;
}