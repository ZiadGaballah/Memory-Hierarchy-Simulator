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
    unsigned long long tag;
    CacheLine() : validBit(false), tag(0) {}
};

int main()
{
    // Input parameters
    unsigned long long memoryBits;
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
    unsigned long long lines = S;
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
try
            {
                // Convert the string address from binary to unsigned long long
                unsigned long long address = stoull(addressStr, nullptr, 2); // Convert from binary string to unsigned long long
                addresses.push_back(address);
            }
            catch (const invalid_argument& e)
            {
                cerr << "Invalid address format: " << addressStr << endl;
                return 1;
            }        }
    }

    // Simulation variables
    int totalAccesses = 0;
    int hits = 0;
    int misses = 0;
    int blockOffsetBits = log2(L);         // Number of bits to index a byte within a cache line
    int indexBits = log2(lines);           // Number of bits to index the cache lines

    // Simulation
    for (auto addr : addresses)
    {
        totalAccesses++;
        
        // // Convert the binary string to unsigned int using memoryBits for bit length
        // unsigned long long addr = bitset<40>(addressStr).to_ulong();  // Convert the binary string to unsigned int
        // if (addressStr.length() > memoryBits)
        // {
        //     cerr << "Address in the input exceeds the defined memory bits." << endl;
        //     return 1;
        // }
        
        // Compute index and tag
        unsigned long long blockOffset = addr % L; // Block offset (address % cache line size)
        unsigned long long index = ((addr) >> blockOffsetBits) % lines; // Index of cache line
        unsigned long long tag = addr >> (indexBits + blockOffsetBits); // Tag (shift the address to the right)

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
        cout << "Access #:" << totalAccesses << endl << "Address:      " << bitset<40>(addr) << endl;
        cout <<"Displacement: " << bitset<40>(blockOffset) << endl << "Index:        " << bitset<40>(index) << endl << "Tag:          " << bitset<40>(tag) << endl;
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
        if (cache[i].validBit == 1)
        {
        cout << "Line " << i << ": Valid Bit = " << cache[i].validBit
             << ", Tag = " << bitset<40>(cache[i].tag) << endl;
        }
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
