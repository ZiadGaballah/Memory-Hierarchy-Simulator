#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <bitset>
#include <iomanip>

using namespace std;

struct CacheLine
{
    bool validBit;
    unsigned int tag;
    CacheLine() : validBit(false), tag(0) {}
};

bool readAddressesFromFile(const string &fileName, vector<unsigned long long> &addresses)
{
    ifstream inFile(fileName);
    if (!inFile)
    {
        cerr << "Error opening file: " << fileName << endl;
        return false;
    }

    string line;
    if (!getline(inFile, line))
    {
        cerr << "file error!" << endl;
        return false;
    }
    inFile.close();

    stringstream ss(line);
    string addressStr;
    while (getline(ss, addressStr, ','))
    {
        if (!addressStr.empty() && addressStr.find_first_not_of(' ') != string::npos)
        {
            addresses.push_back(stoull(addressStr));
        }
    }

    return true;
}

void simulateCache(const vector<unsigned long long> &addresses,
                   vector<CacheLine> &cache,
                   int memoryBits,
                   int memoryCycles,
                   int cacheCycles,
                   int S,
                   int L)
{
    int totalAccesses = 0;
    int hits = 0;
    int misses = 0;

    int lines = S / L;
    int blockOffsetBits = (int)log2(L);
    int indexBits = (int)log2(lines);
    int tagBitLength = memoryBits - indexBits - blockOffsetBits;

    for (auto addr : addresses)
    {
        totalAccesses++;

        unsigned int blockOffset = (unsigned int)(addr % L);
        unsigned int index = (unsigned int)((addr / L) % lines);
        unsigned int tag = (unsigned int)(addr >> (indexBits + blockOffsetBits));

        bool isHit = false;

        if (cache[index].validBit && cache[index].tag == tag)
        {
            isHit = true;
            hits++;
        }
        else
        {
            misses++;
            cache[index].validBit = true;
            cache[index].tag = tag;
        }

        int currentTagBits = (tagBitLength < 40) ? tagBitLength : 40;

        cout << "Access number " << totalAccesses << endl
             << "Decimal Address: " << addr << " (Binary: " << bitset<40>(addr).to_string().substr(40 - memoryBits) << ")" << endl;
        cout << "Displacement:    " << blockOffset
             << " (Binary: " << bitset<40>(blockOffset).to_string().substr(40 - blockOffsetBits) << ")" << endl;
        cout << "Index:           " << index
             << " (Binary: " << bitset<40>(index).to_string().substr(40 - indexBits) << ")" << endl;
        cout << "Tag:             " << tag << " (Binary: " << bitset<40>(tag).to_string().substr(40 - currentTagBits) << ")" << endl;
        if (isHit)
        {
            cout << "Result: HIT" << endl;
        }
        else
        {
            cout << "Result: MISS" << endl;
        }
        cout << "Total Hits so far: " << hits << endl;
        cout << "Total Misses so far: " << misses << endl;
        cout << "Total Accesses so far: " << totalAccesses << endl;
        cout << "-------------------------" << endl;
    }

    cout << "\nFinal Cache State:\n";
    int linesCount = S / L;
    for (int i = 0; i < linesCount; i++)
    {
        cout << "Line " << i << ": Valid Bit = " << cache[i].validBit
             << ", Tag = " << cache[i].tag
             << " (Binary: " << bitset<11>(cache[i].tag) << ")" << endl;
    }

    double hitRatio = (totalAccesses > 0) ? (double)hits / totalAccesses : 0.0;
    double missRatio = (totalAccesses > 0) ? (double)misses / totalAccesses : 0.0;
    double amat = 0.0;
    if (totalAccesses > 0)
    {
        double totalTime = hits * cacheCycles + misses * (cacheCycles + memoryCycles);
        amat = totalTime / totalAccesses;
    }

    cout << "\nHit Ratio: " << hitRatio << endl;
    cout << "Miss Ratio: " << missRatio << endl;
    cout << "Average Memory Access Time (AMAT): " << amat << " cycles\n";
}

int main()
{
    int memoryBits;
    int memoryCycles;
    int S;
    int L;
    int cacheCycles;
    string instructionFileName;
    string dataFileName;

    cout << "Enter the number of bits needed to address the memory (16-40): ";
    cin >> memoryBits;
    cout << "Enter the memory access time in cycles (50-200): ";
    cin >> memoryCycles;
    cout << "Enter the total cache size S (in bytes): ";
    cin >> S;
    cout << "Enter the cache line size L (in bytes): ";
    cin >> L;
    cout << "Enter the number of cycles needed to access the cache (1-10): ";
    cin >> cacheCycles;
    cout << "Enter the instruction access sequence file name: ";
    cin >> instructionFileName;
    cout << "Enter the data access sequence file name: ";
    cin >> dataFileName;

    int lines = S / L;

    vector<CacheLine> instructionCache(lines);
    vector<CacheLine> dataCache(lines);

    vector<unsigned long long> instructionAddresses;
    vector<unsigned long long> dataAddresses;

    if (!readAddressesFromFile(instructionFileName, instructionAddresses))
    {
        return 1;
    }
    if (!readAddressesFromFile(dataFileName, dataAddresses))
    {
        return 1;
    }

    cout << "===========================" << endl;
    cout << "INSTRUCTION CACHE" << endl;
    cout << "===========================" << endl;
    simulateCache(instructionAddresses, instructionCache, memoryBits, memoryCycles, cacheCycles, S, L);
    cout << "\n=======================" << endl;
    cout << "DATA CACHE" << endl;
    cout << "=======================" << endl;
    simulateCache(dataAddresses, dataCache, memoryBits, memoryCycles, cacheCycles, S, L);

    return 0;
}
