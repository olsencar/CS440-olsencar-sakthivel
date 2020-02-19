#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <sys/stat.h>
#include <cstdlib>
#if defined(_WIN32) //_mkdir
#include <direct.h>
#endif
using namespace std;

const string IDX_FILE_NAME = "EmployeeIndex.txt";

/* FROM Stack overflow to make dir cross platform */
void makeDir(const string& path) {
    #if defined(_WIN32)
        int ret = _mkdir(path.c_str());
    #else
        mode_t mode = 0755;
        int ret = mkdir(path.c_str(), mode);
    #endif
}

string getNullFilledString(string s, size_t length) {
    for (size_t i = s.length(); i < length; i++) {
        s += '\0';
    }
    return s;
}

class Record
{
public:
    string id;         // 8 bytes = 8 chars
    string name;       // 200 bytes = 200 chars
    string bio;        // 500 bytes = 500 chars
    string manager_id; // 8 bytes = 8 chars

    int getRecordSize()
    {
        return id.length() + name.length() + bio.length() + manager_id.length();
    }

    void printRecord()
    {
        cout << "id: " << id << endl
             << "name: " << name << endl
             << "bio: " << bio << endl
             << "manager_id: " << manager_id << endl;
    }
};

class Block
{
private:
    static const int blockCapacity = 4096;
    int total_record_size;
    vector<Record> records;

public:
    Block *next;
    // each block has 4096 bytes meaning we have a possibility of 5.72 (5). So we will will create a new block on record 6.
    int getTotalRecordSize()
    {
        return total_record_size;
    }

    int getTotalRecords()
    {
        return records.size();
    }

    vector<Record> getRecords()
    {
        return records;
    }

    void addRecord(Record r)
    {
        if (r.getRecordSize() + getTotalRecordSize() > blockCapacity)
        {
            next = new Block();
            next->addRecord(r);
        }
        else
        {
            records.push_back(r);
            total_record_size += r.getRecordSize();
        }
    }

    Block()
    {
        total_record_size = 0;
        next = NULL;
    }
};

class LinearHashTable
{
private:
    int tableSize; // n
    vector<Block *> blocks;
    int startingBlocks;
    static int convertStringToInt(string key)
    {
        int res = 0;
        for (int i = 0; i < key.length(); i++)
        {
            res += key[i];
        }
        return res;
    }

public:
    static int hash(string key, int blockSize)
    {
        return convertStringToInt(key) % blockSize;
    }

    void addBlock(Block *b)
    {
        blocks.push_back(b);
        tableSize++;
    }

    void reHash()
    {
        vector<Block *> newBlocks;
        tableSize = blocks.size() + 1;
        for (int j = 0; j <= blocks.size(); j++)
        {
            Block *b = new Block();
            newBlocks.push_back(b);
        }

        string cur_idx;
        for (int i = 0; i < blocks.size(); i++)
        {
            Block *currBlock = blocks[i];
            while (currBlock != NULL)
            {
                vector<Record> records = blocks[i]->getRecords();
                for (int k = 0; k < records.size(); k++)
                {
                    newBlocks[hash(records[k].id, tableSize)]->addRecord(records[k]);
                }
                Block *temp = currBlock;
                currBlock = currBlock->next;
                delete temp;
            }
        }
        blocks = newBlocks;
    }

    void addRecord(Record r)
    {
        blocks[hash(r.id, tableSize)]->addRecord(r);
        // cout << "avg cap: " << computeAvgCapacity() << endl;
        if (computeAvgCapacity() > .8)
        {
            // cout << "REHASHING" << endl;
            reHash();
        }
    }

    float computeAvgCapacity()
    {
        int totalRecordSize = 0;
        for (int i = 0; i < blocks.size(); i++)
        {
            totalRecordSize = blocks[i]->getTotalRecordSize();
        }
        return totalRecordSize / float(blocks.size() * 4096);
    }

    void generate_index_file(vector<size_t> blockOffsets)
    {
        ofstream idxFile(IDX_FILE_NAME.c_str(), ios::out);
        idxFile << tableSize << endl;
        for (size_t i = 0; i < blockOffsets.size(); i++) {
            idxFile << blockOffsets[i] << endl;
        }
        idxFile.close();
    }

    static string toString(int data)
    {
        stringstream s;
        s << data;
        return s.str();
    }

    vector<size_t> createSortedFile()
    {
        const string fileName = "EmployeesSorted.txt";
        ofstream sortedFile(fileName.c_str(), ios::out);
        Block *cur = NULL;
        vector<size_t> blockOffsets; 
        for (int i = 0; i < blocks.size(); i++)
        {
            blockOffsets.push_back(sortedFile.tellp());
            cur = blocks[i];
            while (cur != NULL)
            {
                vector<Record> records = cur->getRecords();
                for (int j = 0; j < records.size(); j++)
                {
                    sortedFile << getNullFilledString(records[j].id, 8) << getNullFilledString(records[j].name, 200) << getNullFilledString(records[j].bio, 500) << getNullFilledString(records[j].manager_id, 8) << endl;
                }
                cur = cur->next;
            }
        }
        sortedFile.close();
        return blockOffsets;
    }

    LinearHashTable()
    {
        startingBlocks = 1;
        tableSize = 0;
        for (int i = 0; i < startingBlocks; i++)
        {
            addBlock(new Block());
        }
    }

    LinearHashTable(int startBlocks)
    {
        startingBlocks = startBlocks;
        tableSize = 0;
        for (int i = 0; i < startingBlocks; i++)
        {
            addBlock(new Block());
        }
    }

    ~LinearHashTable()
    {
        for (int i = 0; i < blocks.size(); i++)
        {
            Block *currBlock = blocks[i];
            while (currBlock != NULL)
            {
                Block *temp = currBlock;
                currBlock = currBlock->next;
                delete temp;
            }
        }
    }
};

void createIndex()
{
    LinearHashTable tab(1);
    ifstream myFile;
    int count = 0;
    myFile.open("Employees.csv");
    string line;

    while (getline(myFile, line))
    {
        istringstream iss(line);
        string field;
        string row[4];
        Record r;
        int i = 0;
        while (getline(iss, field, ','))
        {
            row[i++] = field;
        }

        r.id = row[0];
        r.name = row[1];
        r.bio = row[2];
        r.manager_id = row[3];
        tab.addRecord(r);
    }
    myFile.close();
    vector<size_t> blockOffsets = tab.createSortedFile();

    tab.generate_index_file(blockOffsets);
}

Record findRecordUsingIndex(string id)
{
    LinearHashTable hashTab(1);
    string line;
    vector<size_t> blockOffsets;
    int tablesize = 0;
    Record r;
    r.id = "";

    ifstream inFile;
    inFile.open(IDX_FILE_NAME.c_str(), ios::in);
    getline(inFile, line);
    tablesize = atoi(line.c_str());
    
    while(getline(inFile, line)) {
        blockOffsets.push_back((size_t)atoi(line.c_str()));
    }

    inFile.close();
    int id_idx = hashTab.hash(id, tablesize);
    size_t offset = blockOffsets[id_idx];

    inFile.open("EmployeesSorted.txt", ios::in);
    // go to location in file
    inFile.seekg(offset, ios::beg);

    while (getline(inFile, line))
    {
        const char *tempId = line.substr(0, 8).c_str();
        
        if (tempId == id) {
            r.id = tempId;
            r.name = line.substr(8, 200).c_str();
            r.bio = line.substr(208, 500).c_str();
            r.manager_id = line.substr(708, 8).c_str();
            break;
        }
        
    }
    return r;
}

int main(int argc, char *argv[])
{
    if (argc > 1 && argc < 4)
    {

        if (strcmp(argv[1], "C") == 0)
        {
            createIndex();
        }
        else if (strcmp(argv[1], "L") == 0)
        {
            string id = argv[2];
            Record r = findRecordUsingIndex(id);
            if (r.id == "")
            {
                cout << "No records found for " << id << endl;
            }
            else
            {
                r.printRecord();
            }
        }
    }
    else
    {
        cerr << "Argument count not correct" << endl;
        return 1;
    }

    return 0;
}
