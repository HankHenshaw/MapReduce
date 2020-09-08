#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <functional>
#include <atomic>
#include <thread>
#include <future>

class MapReduce {
public:
    MapReduce(std::string fileName, size_t mnum, size_t rnum, 
        std::function<std::vector<std::string>(std::string)> mapLambda,
        std::function<void(std::string, size_t, size_t)> reduceLambda);
    ~MapReduce();
private:
    std::string m_fileName;
    size_t m_numOfMapThreads;
    size_t m_numOfReduceThreads;

    std::ifstream m_fin;

    std::vector<std::pair<std::streampos, std::streampos>> m_splitPositions;

    std::vector<std::vector<std::string>> m_vecOfWordsAfterMap;
    std::vector<std::vector<std::string>> m_vecOfWordsAfterShuffle;

    std::function<std::vector<std::string>(std::string)> m_mapLambda;
    std::function<void(std::string, size_t, size_t)> m_reduceLambda;

    std::atomic_int m_shuffleInt;
private:
    void openFile();
    void splitFile();
    void map(std::streampos begPos, std::streampos lastPos);
    void reduce(size_t numOfVec);
    void shuffle(size_t numOfVec);
};