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

    MapReduce(filename, mnum, rnum);

    return 0;
}