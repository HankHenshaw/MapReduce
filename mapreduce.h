#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>

class MapReduce {
public:
    MapReduce(std::string fileName, size_t mnum, size_t rnum);
    ~MapReduce();
private:
    std::string m_fileName;
    size_t m_numOfMapThreads;
    size_t m_numOfReduceThreads;

    std::ifstream m_fin;

    std::unordered_map<std::string, size_t> m_hash; // Remove?

    std::vector<std::pair<std::streampos, std::streampos>> m_splitPositions;
private:
    void openFile();
    void splitFile();
    void Map();

    void printHash(); //Remove
};