#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

struct CacheLine
{
    bool validBit;
    unsigned long long tag;
    CacheLine() : validBit(false), tag(0) {}
};

int main()
{
    // Input parameters
    int memoryBits;
    int memoryCycles; // Memory access time in cycles
    int S;            // Total cache size (in bytes)
    int L;            // Cache line size (in bytes)
    int cacheCycles;  // Cache access time in cycles
    string fileName;

    // Prompt the user for all required inputs
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

    // Compute the number of cache lines
    int lines = S / L;
    vector<CacheLine> cache(lines);

    // Open the file containing the sequence of memory accesses
    ifstream inFile(fileName);
    if (!inFile)
    {
        cerr << "Error opening file: " << fileName << endl;
        return 1;
    }

    // Read the entire line containing comma-separated addresses
    string line;
    if (!getline(inFile, line))
    {
        cerr << "Error reading from file or file is empty." << endl;
        return 1;
    }
    inFile.close();

    // Parse the addresses
    stringstream ss(line);
    string addressStr;
    vector<unsigned long long> addresses;
    while (getline(ss, addressStr, ','))
    {
        // Trim possible spaces
        if (!addressStr.empty() && addressStr.find_first_not_of(' ') != string::npos)
        {
            unsigned long long addr = stoull(addressStr);
            addresses.push_back(addr);
        }
    }

    // Simulation variables
    int totalAccesses = 0;
    int hits = 0;
    int misses = 0;

    // Simulation
    for (auto addr : addresses)
    {
        totalAccesses++;
        // Compute index and tag
        unsigned long long index = (addr / L) % lines;
        unsigned long long tag = (addr / (lines * (unsigned long long)L));

        bool isHit = false;

        // Check the cache line
        if (cache[index].validBit && cache[index].tag == tag)
        {
            // Hit
            isHit = true;
            hits++;
        }
        else
        {
            // Miss
            misses++;
            // Update cache line
            cache[index].validBit = true;
            cache[index].tag = tag;
        }

        // After each access, print the required info
        cout << "Access #" << totalAccesses << ": Address = " << addr << endl;
        cout << "Index: " << index << ", Tag: " << tag << endl;
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

    // End of simulation results
    cout << "\nFinal Cache State:\n";
    for (int i = 0; i < lines; i++)
    {
        cout << "Line " << i << ": Valid Bit = " << cache[i].validBit
             << ", Tag = " << cache[i].tag << endl;
    }

    double hitRatio = (totalAccesses > 0) ? (double)hits / totalAccesses : 0.0;
    double missRatio = (totalAccesses > 0) ? (double)misses / totalAccesses : 0.0;

    // Compute AMAT
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