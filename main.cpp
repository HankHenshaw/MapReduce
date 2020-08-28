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

    //TODO: Получать пользовательскую лямбду для map и для reduce в к-торе
    MapReduce map_reduce(filename, mnum, rnum);

    /*Remove*/
    map_reduce.Map(mapLambda);
    map_reduce.Map(mapLambda);
    map_reduce.shuffle(0);
    map_reduce.shuffle(1);
    map_reduce.print();
    /*Remove*/

    return 0;
}