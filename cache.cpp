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

int main()
{
    int memoryBits;
    int memoryCycles;
    int S;
    int L;
    int cacheCycles;
    string fileName;

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
    cout << "Enter the access sequence file name: ";
    cin >> fileName;

    int lines = S / L;
    vector<CacheLine> cache(lines);

    ifstream inFile(fileName);
    if (!inFile)
    {
        cerr << "Error opening file: " << fileName << endl;
        return 1;
    }

    string line;
    if (!getline(inFile, line))
    {
        cerr << "file error!" << endl;
        return 1;
    }
    inFile.close();

    // storing the addresses
    stringstream ss(line);
    string addressStr;
    vector<unsigned long long> addresses;

    while (getline(ss, addressStr, ','))
    {
        if (!addressStr.empty() && addressStr.find_first_not_of(' ') != string::npos)
        {
            addresses.push_back(stoull(addressStr));
        }
    }
    int totalAccesses = 0;
    int hits = 0;
    int misses = 0;
    int blockOffsetBits = log2(L);
    int indexBits = log2(lines);

    for (auto addr : addresses)
    {
        totalAccesses++;

        // Compute index and tag
        unsigned int blockOffset = addr % L;
        unsigned int index = (addr / L) % lines;
        unsigned int tag = addr >> (indexBits + blockOffsetBits);

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

        int tagBitLength = memoryBits - indexBits - blockOffsetBits;

        cout << "Access number " << totalAccesses << endl
             << "Decimal Address: " << addr << " (Binary: " << bitset<40>(addr).to_string().substr(40 - memoryBits) << ")" << endl;
        cout << "Displacement:    " << blockOffset
             << " (Binary: " << bitset<40>(blockOffset).to_string().substr(40 - blockOffsetBits) << ")" << endl;
        cout << "Index:           " << index
             << " (Binary: " << bitset<40>(index).to_string().substr(40 - indexBits) << ")" << endl;
        int currentTagBits = min(tagBitLength, 40);
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
    for (int i = 0; i < lines; i++)
    {
        cout << "Line " << i << ": Valid Bit = " << cache[i].validBit
             << ", Tag = " << cache[i].tag
             << " (Binary: " << bitset<11>(cache[i].tag) << ")" << endl;
    }

    double hitRatio = (totalAccesses > 0) ? (double)hits / totalAccesses : 0.0;
    double missRatio = (totalAccesses > 0) ? (double)misses / totalAccesses : 0.0;

    // AMAT = (Hits * CacheCycles + Misses * (CacheCycles + MemoryCycles)) / TotalAccesses
    double amat = 0.0;
    if (totalAccesses > 0)
    {
        double totalTime = hits * cacheCycles + misses * (cacheCycles + memoryCycles);
        amat = totalTime / totalAccesses;
    }

    cout << "\nHit Ratio: " << hitRatio << endl;
    cout << "Miss Ratio: " << missRatio << endl;
    cout << "Average Memory Access Time (AMAT): " << amat << " cycles\n";

    return 0;
}
