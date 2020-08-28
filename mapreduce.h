#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <functional>

class MapReduce {
public:
    MapReduce(std::string fileName, size_t mnum, size_t rnum);
    ~MapReduce();

    void Map(std::function<std::vector<std::string>(std::string)> map_function); // TODO: make private
    void shuffle(size_t numOfVec); // TODO: make private
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
private:
    void openFile();
    void splitFile();
};