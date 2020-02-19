#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

constexpr int INTEGER_SIZE = 4;
constexpr int DOUBLE_SIZE = 8;
constexpr int STRING_SIZE = 40;
constexpr int MM_BLOCKS = 22;
constexpr int MM_BLOCKS_SQRD = 484;
constexpr char EMP_INPUT_FILENAME[] = "Emp.csv";
constexpr char DEPT_INPUT_FILENAME[] = "Dept.csv";
constexpr char EMP_OUTPUT_FILENAME[] = "EmpOut.txt";
constexpr char DEPT_OUTPUT_FILENAME[] = "DeptOut.txt";
constexpr char JOIN_OUTPUT_FILENAME[] = "join.csv";
constexpr char RUN_SEPARATOR[] = "===========";
using namespace std;

class Block {
public:
    int id;
    virtual void parseRow(string) = 0;
    virtual void setAttributes(string[4]) = 0;
	virtual string getWritableRow() = 0;
};

class Dept : public Block {
public:
	int did;
    string dname;
    double budget;

	Dept() {}

	Dept(Dept& d) {
		setDept(d);
	}
	void setDept(Dept d) {
		did = d.did;
		dname = d.dname;
		budget = d.budget;
		id = d.id;
	}



    void parseRow(string line) {
        string row[4];
        istringstream iss(line);
        string field;

        int i = 0;
        while (getline(iss, field, ',')) {
            row[i++] = field;
        }

        setAttributes(row);
    }    
    void setAttributes(string row[4]) {
        did = atoi(row[0].c_str());
        dname = row[1];
        budget = atof(row[2].c_str());
        id = atoi(row[3].c_str());
    }

	string getWritableRow() {
		return to_string(did) + "," + dname + "," + to_string(budget) + "," + to_string(id);
	}
};

class Emp : public Block {
public:
    string ename;
    int age;
    double salary;

	Emp() { }
	Emp(Emp& e) {
		setEmp(e);
	}

	void setEmp(Emp e) {
		id = e.id;
		ename = e.ename;
		age = e.age;
		salary = e.salary;
	}

    void parseRow(string line) {
        string row[4];
        istringstream iss(line);
        string field;

        int i = 0;
        while (getline(iss, field, ',')) {
	        row[i++] = field;
        }

        setAttributes(row);
    }

    void setAttributes(string row[4]) {
        id = atoi(row[0].c_str());
        ename = row[1];
        age = atoi(row[2].c_str());
        salary = atof(row[3].c_str());
    }

	string getWritableRow() {
		return to_string(id) + "," + ename + "," + to_string(age) + "," + to_string(salary);
	}
};

void merge(vector<Block*>& mem, int start, int middle, int end) {
	vector<Block*> leftArray;
	vector<Block*> rightArray;

	// fill in left Array
	for (int i = 0; i < middle - start + 1; ++i) 
		leftArray.push_back(mem[start + i]);

	// fill in right Array
	for (int j = 0; j < end - middle; ++j)
		rightArray.push_back(mem[middle + 1 + j]);

	/* Merge the temp Arrays */

	// initial indexes of first and second subArrays
	int leftIndex = 0, rightIndex = 0;

	// the index we will start at when adding the subArrays back into the main Array
	int currentIndex = start;

	Block* temp;
	// compare each index of the subArrays adding the lowest value to the currentIndex
	while (leftIndex < middle - start + 1 && rightIndex < end - middle) {
		if (leftArray[leftIndex]->id <= rightArray[rightIndex]->id) {
			mem[currentIndex] = leftArray[leftIndex];
			leftIndex++;
		}
		else {
			mem[currentIndex] = rightArray[rightIndex];
			rightIndex++;
		}
		currentIndex++;
	}

	// copy remaining elements of leftArray[] if any
	while (leftIndex < middle - start + 1) {
		temp = mem[currentIndex];
		mem[currentIndex++] = leftArray[leftIndex++];
	}

	// copy remaining elements of rightArray[] if any
	while (rightIndex < end - middle) {
		temp = mem[currentIndex];
		mem[currentIndex++] = rightArray[rightIndex++];
	}
}

void mergeSort(vector<Block*>& mem, int start, int end) {
	// base case
	if (start < end) {
		// find the middle point
		int middle = (start + end) / 2;

		mergeSort(mem, start, middle); // sort first half
		mergeSort(mem, middle + 1, end);  // sort second half

		// merge the sorted halves
		merge(mem, start, middle, end);
	}
}

class MainMemory {
private:
    int capacity;
    vector<Block*> mem;

	bool capacityReached() {
		if (mem.size() == capacity) {
			return true;
		}
		return false;
	}

public:
    MainMemory(int cap) {
        capacity = cap;
    }

    int getSize() { return mem.size(); }

    int addEmployee(Emp* e) {
		Block *b = e;
        mem.push_back(b);
		return capacity - mem.size();
    }

    int addDepartment(Dept* d) {
		Block* b = d;
        mem.push_back(b);
		return capacity - mem.size();
    }

	void sortRun(int start, int end) {
		mergeSort(mem, start, end);
	}

    void writeRun(ofstream& outFile) {
		sortRun(0, mem.size() - 1);
		for (int i = 0; i < mem.size(); i++) {
			outFile << mem[i]->getWritableRow() << endl;
		}
		outFile << RUN_SEPARATOR << endl;
		mem.clear();
    }
	void mergeAndJoinRuns(int middle, ofstream& outfile) {

		// initial indexes of first and second subArrays
		int leftIndex = 0, rightIndex = middle;

		// the index we will start at when adding the subArrays back into the main Array
		int currentIndex = 0;

		// compare each index of the subArrays adding the lowest value to the currentIndex
		while (leftIndex < middle && rightIndex < mem.size()) {
			if (mem[leftIndex]->id > mem[rightIndex]->id) {
				rightIndex++;
			}
			else if (mem[leftIndex]->id < mem[rightIndex]->id) {
				leftIndex++;
			}
			else {
				outfile << mem[leftIndex]->getWritableRow() << "," << mem[rightIndex]->getWritableRow() << endl;
				leftIndex++;
				rightIndex++;
			}
		}
	}

	~MainMemory() {
		for (int i = 0; i < mem.size(); i++) {
			delete mem[i];
		}
	}

};

int getAllRuns(const int idx, MainMemory& mem, ifstream& empFile, ifstream& deptFile) {
	string line;
	int i = 0;

	// Gets the blocks for empFile that are at index = idx in their runs
    while (getline(empFile, line)) {
		if (i == idx) {
			Emp* e = new Emp();

			e->parseRow(line);
			// If we are at 11 blocks left, then stop
			if (mem.addEmployee(e) == MM_BLOCKS / 2) {
				break;
			}
		} else {
			if (line == RUN_SEPARATOR) {
				i = 0;
				continue;
			} 
		}
		++i;
	}
	int start = mem.getSize();
	mem.sortRun(0, mem.getSize() - 1);  

	// Gets the blocks for deptFile that are at index = idx in their runs
    while (getline(deptFile, line)) {
		if (i == idx) {
			Dept* d = new Dept();

			d->parseRow(line);
			// If we have 1 block left in memory, stop
			if (mem.addDepartment(d) == 1) {
				break;
			}
		} else {
			if (line == RUN_SEPARATOR) {
				i = 0;
			} else {
				++i;
			}
		}
	} 
	mem.sortRun(start, mem.getSize() - 1);
	return start;	
}


void sortMergeJoin(MainMemory& mem, ifstream& empFile, ifstream& deptFile, ofstream& joinFile, int empCount, int deptCount) {
    string empLine;
    string deptLine;
	
	int middle = getAllRuns(0, mem, empFile, deptFile);
	mem.mergeAndJoinRuns(middle, joinFile);
	
}


// Create file for each run (with its own file pointer)
// Create a vector of file pointers
/* for each empid in empPointers
		sortDeptPointers()
		for each managerid in deptPointers
			if empid < managerid
				incrementEmployeeRunPointer
			else if empid > managerid
				incrementRunPointerForDept
				empPointer--;
				break
			else if empid == managerid
				outputJoin
				incrementRunPointerForDept
				managerId--;


*/

int main (int argc, char* argv[]) {
	ifstream empFile;
	ifstream deptFile;
	ofstream oEmpFile;
	ofstream oDeptFile;
	ofstream joinFile;

	string line;
	MainMemory mMemory(MM_BLOCKS);
	int empCount = 0, deptCount = 0;


	empFile.open(EMP_INPUT_FILENAME, ios::in);
	oEmpFile.open(EMP_OUTPUT_FILENAME, ios::out);
	// Create sorted runs for Emp relation
	while (getline(empFile, line)) {
		empCount++;
		Emp *e = new Emp();
		e->parseRow(line);
		if (mMemory.addEmployee(e) == 0) {
			mMemory.writeRun(oEmpFile);
		}
	}
	if (mMemory.getSize() > 0) {
		mMemory.writeRun(oEmpFile);

	}
	empFile.close();
	oEmpFile.close();

	deptFile.open(DEPT_INPUT_FILENAME, ios::in);
	oDeptFile.open(DEPT_OUTPUT_FILENAME, ios::out);
	// Create sorted runs for Dept relation
	while (getline(deptFile, line)) {
		deptCount++;
		Dept* d = new Dept();
		d->parseRow(line);
		if (mMemory.addDepartment(d) == 0) {
			mMemory.writeRun(oDeptFile);
		}
	}
	if (mMemory.getSize() > 0) {
		mMemory.writeRun(oDeptFile);
	}
	deptFile.close();
	oDeptFile.close();

	// Join and merge runs 
	empFile.open(EMP_OUTPUT_FILENAME, ios::in);
	deptFile.open(DEPT_OUTPUT_FILENAME, ios::in);
	joinFile.open(JOIN_OUTPUT_FILENAME, ios::out);

	sortMergeJoin(mMemory, empFile, deptFile, joinFile, empCount, deptCount);

	empFile.close();
	deptFile.close();
	joinFile.close();
    return 0;
}