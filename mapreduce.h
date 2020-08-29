#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <functional>
#include <atomic>

class MapReduce {
public:
    MapReduce(std::string fileName, size_t mnum, size_t rnum, 
        std::function<std::vector<std::string>(std::string)> mapLambda,
        std::function<void(std::string, size_t)> reduceLambda);
    ~MapReduce();

    void print();
private:
    std::string m_fileName;
    size_t m_numOfMapThreads;
    size_t m_numOfReduceThreads;

    std::ifstream m_fin;

    std::vector<std::pair<std::streampos, std::streampos>> m_splitPositions;
    size_t m_posInVectorOfPos;

    std::vector<std::vector<std::string>> m_vecOfWordsAfterMap;
    std::vector<std::vector<std::string>> m_vecOfWordsAfterShuffle;

    std::function<std::vector<std::string>(std::string)> m_mapLambda;
    std::function<void(std::string, size_t)> m_reduceLambda;

    std::atomic_int m_shuffleInt;
    std::atomic_int m_reduceInt;
private:
    void openFile();
    void splitFile();
    void map(std::function<std::vector<std::string>(std::string)> map_function, std::streampos begPos, std::streampos lastPos);
    void reduce(std::function<void(std::string, size_t num)> reduce_function, size_t numOfVec);
    void shuffle(size_t numOfVec);
};