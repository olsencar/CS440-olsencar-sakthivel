#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>

using namespace std;

const string IDX_FILE_NAME = "EmployeeIndex.txt";

class Record {
public:
    string id; // 8 bytes = 8 chars
    string name; // 200 bytes = 200 chars
    string bio; // 500 bytes = 500 chars
    string manager_id; // 8 bytes = 8 chars

    int getRecordSize() {
        return id.length() + name.length() + bio.length() + manager_id.length();
    }

    void printRecord() {
        cout << "id: " << id << endl
        << "name: " << name << endl
        << "bio: " << bio << endl
        << "manager_id: " << manager_id << endl;
    }
};

class Block {
private:
    const int blockCapacity = 4096;
    int total_record_size;
    vector<Record> records;
public:
    Block* next;
// each block has 4096 bytes meaning we have a possibility of 5.72 (5). So we will will create a new block on record 6.
    int getTotalRecordSize() {
        return total_record_size;
    }

    int getTotalRecords() {
        return records.size();
    }

    vector<Record> getRecords() {
        return records;
    }

    void addRecord(Record r) {
        if (r.getRecordSize() + getTotalRecordSize() > blockCapacity) {
            next = new Block();
            next->addRecord(r);
        }
        else {
            records.push_back(r);
            total_record_size += r.getRecordSize();
        }
    }

    Block() {
        total_record_size = 0;
        next = NULL;
    }
};

class LinearHashTable {
private:
    int tableSize; // n
    vector<Block *> blocks;
    int startingBlocks;
    static int convertStringToInt(string key) {
        int res = 0;
        for (int i = 0; i < key.length(); i++) {
            res += key[i];
        }
        return res;
    }
public:
    static int hash(string key, int blockSize) {
        return convertStringToInt(key) % blockSize;
    }

    void addBlock(Block *b){
        blocks.push_back(b);
        tableSize++;
    }

    void reHash() {
        vector<Block *> newBlocks;
        tableSize = blocks.size() + 1;
        for (int j = 0; j <= blocks.size(); j++) {
            Block* b = new Block();
            newBlocks.push_back(b);
        }
        
        string cur_idx;
        for (int i = 0; i < blocks.size(); i++) {
            Block* currBlock = blocks[i];
            while (currBlock != NULL) {
                vector<Record> records = blocks[i]->getRecords();
                for(int k = 0; k < records.size(); k++) {
                   newBlocks[hash(records[k].id, tableSize)]->addRecord(records[k]);
                }
                Block * temp = currBlock;
                currBlock = currBlock->next;
                delete temp;
            }
        }
        blocks = newBlocks;
    }

    void addRecord(Record r) {
        blocks[hash(r.id, tableSize)]->addRecord(r);
        // cout << "avg cap: " << computeAvgCapacity() << endl;
        if (computeAvgCapacity() > .8) {
            // cout << "REHASHING" << endl;
            reHash();
        }
    }

    float computeAvgCapacity() {
        int totalRecordSize = 0;
        for (int i = 0; i < blocks.size(); i++) {
            totalRecordSize = blocks[i]->getTotalRecordSize();
        }
        return totalRecordSize / float(blocks.size() * 4096);
    }

    void generateIndexFile() {
        ofstream idxFile;
        idxFile.open(IDX_FILE_NAME, ios::out);
        
        Block *cur = NULL;
        int total_record_size = 0;
        int overflow = 0;
        for(int i = 0; i < blocks.size(); i++){
            cur = blocks[i];
            while(cur != NULL) {
                vector<Record> records = cur->getRecords();
                for(int j = 0; j < records.size(); j++) {
                    total_record_size += records[j].getRecordSize();
                    idxFile << i << "," << records[j].id << "," << records[j].name << "," << records[j].bio << "," << records[j].manager_id << endl;
                }
                total_record_size = 0;
                overflow++;
                cur = cur->next;
            }
            overflow = 0;
        }
    }

    LinearHashTable() {
        startingBlocks = 1;
        tableSize = 0;
        for (int i = 0; i < startingBlocks; i++) {
            addBlock(new Block());
        }
    }

    LinearHashTable(int startBlocks) {
        startingBlocks = startBlocks;
        tableSize = 0;
        for (int i = 0; i < startingBlocks; i++) {
            addBlock(new Block());
        }
    }

    ~LinearHashTable() {
        for (int i = 0; i < blocks.size(); i++) {
            Block* currBlock = blocks[i];
            while (currBlock != NULL) {
                Block * temp = currBlock;
                currBlock = currBlock->next;
                delete temp;
            }
        }
    }
};

void createIndex() {
    LinearHashTable tab(1);
    ifstream myFile;
    int count = 0;
    myFile.open("Employees.csv");
    string line;

    while(getline(myFile, line)){
        istringstream iss(line);
        string field;
        string row[4];
        Record r;
        int i = 0;
        while (getline(iss, field, ',')) {
            row[i++] = field;
        }

        r.id = row[0];
        r.name = row[1];
        r.bio = row[2];
        r.manager_id = row[3];
        tab.addRecord(r);
    }
    myFile.close();
    tab.generateIndexFile();
}

Record findRecordUsingIndex(string id) {
    LinearHashTable hashTab(1);
    string line;
    vector<vector<Record>> table;        
    vector<Record> records;

    ifstream inFile;
    inFile.open(IDX_FILE_NAME, ios::in);
    int currentBlock = 0;
    while(getline(inFile, line)) {
        // ascii check
        if (line[0] - 48 != currentBlock) {
            table.push_back(records);
            records.clear();
            currentBlock++;   
        }
        istringstream iss(line);
        string field;
        string row[5];
        Record r;
        int i = 0;

        while (getline(iss, field, ',')) {
            row[i++] = field;
        }
        r.id = row[1];
        r.name = row[2];
        r.bio = row[3];
        r.manager_id = row[4];
        // cout << r.id << ", " << r.name << ", " << r.bio << ", " << r.manager_id << endl;
        records.push_back(r);
    }
    table.push_back(records);
    inFile.close();

    Record searchRecord;
    int hashIdx = hashTab.hash(id, table.size());
    records = table[hashIdx];
    for (int i = 0; i < records.size(); i++) {
        if (records[i].id == id) {
            searchRecord = records[i];
            break;
        }
    }
    return searchRecord;
}

int main(int argc, char* argv[]) {
    if (strcmp(argv[1], "C") == 0) {
        createIndex();
    } else if (strcmp(argv[1], "L") == 0) {
        if (argc < 3 ) {
            return 1;
        }

        string id = argv[2];
        Record r = findRecordUsingIndex(id);
        r.printRecord();        
    }

    return 0;
}