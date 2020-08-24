#include <string>
#include <fstream>
#include <unordered_map>

class MapReduce {
public:
    MapReduce(std::string fileName, size_t mnum, size_t rnum);
    ~MapReduce();
private:
    std::string m_fileName;
    size_t m_numOfMapThreads;
    size_t m_numOfReduceThreads;

    std::ifstream m_fin;

    std::unordered_map<std::string, size_t> m_hash;
private:
    void openFile();
    void Map();

    void printHash(); //Remove
};