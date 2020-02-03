#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <assert.h>
using namespace std;

const string IDX_FILE_NAME = "EmployeeIndex.txt";

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

    void generate_base_index_file()
    {
        ofstream idxFile(IDX_FILE_NAME.c_str(), ios::out);
        idxFile << tableSize << endl;
        idxFile.close();
    }

    static string toString(int data)
    {
        stringstream s;
        s << data;
        return s.str();
    }

    void generateIndexFile()
    {
        string fileName = "./indexed_relations/EmployeeIndex";
        Block *cur = NULL;
        for (int i = 0; i < blocks.size(); i++)
        {
            string tempFileName = fileName + toString(i) + ".txt";
            ofstream idxFile(tempFileName.c_str(), ios::out);
            cur = blocks[i];
            while (cur != NULL)
            {
                vector<Record> records = cur->getRecords();
                for (int j = 0; j < records.size(); j++)
                {
                    idxFile << records[j].id << "," << records[j].name << "," << records[j].bio << "," << records[j].manager_id << endl;
                }
                cur = cur->next;
            }
            idxFile.close();
        }
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
    tab.generate_base_index_file();
    tab.generateIndexFile();
}

Record findRecordUsingIndex(string id)
{
    LinearHashTable hashTab(1);
    string line;
    int tablesize = 0;
    Record r;
    r.id = "";

    ifstream inFile;
    inFile.open(IDX_FILE_NAME.c_str(), ios::in);
    while (getline(inFile, line))
    {
        tablesize = line[0] - 48;
    }
    inFile.close();
    int id_idx = hashTab.hash(id, tablesize);
    string fileName = "./indexed_relations/EmployeeIndex" + hashTab.toString(id_idx) + ".txt";
    cout << fileName << endl;
    inFile.open(fileName.c_str(), ios::in);
    while (getline(inFile, line))
    {
        istringstream iss(line);
        string field;
        string row[4];
        int i = 0;
        while (getline(iss, field, ','))
        {
            row[i++] = field;
        }

        r.id = row[0];
        r.name = row[1];
        r.bio = row[2];
        r.manager_id = row[3];
        if (r.id == id)
        {
            inFile.close();
            return r;
        }
    }
    r.id = "";
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
