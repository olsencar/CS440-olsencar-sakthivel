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
constexpr char EMP_OUTPUT_FILENAME[] = "EmpOut.text";
constexpr char DEPT_OUTPUT_FILENAME[] = "DeptOut.text";
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
		return did + "," + dname + "," + to_string(budget) + "," + to_string(id);
	}
};

class Emp : public Block {
public:
    string ename;
    int age;
    double salary;

	Emp() { }

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
		return id + "," + ename + "," + to_string(age) + "," + to_string(salary);
	}
};

class MainMemory {
private:
    int capacity;
    vector<unique_ptr<Block>> mem;

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

    int addEmployee(Emp e) {
		unique_ptr<Block> b(&e);
        mem.push_back(b);
		return capacity - mem.size();
    }

    int addDepartment(Dept d) {
		unique_ptr<Block> b(&d);
        mem.push_back(b);
		return capacity - mem.size();
    }

	void sortRun(int start, int end) {
		mergeSort(mem, start, end);
	}

    void writeRun(ofstream& outFile) {
		sortRun(0, static_cast<int>(mem.size() - 1));
		for (int i = 0; i < mem.size(); i++) {
			outFile << mem[i]->getWritableRow() << endl;
		}
		outFile << RUN_SEPARATOR << endl;
		mem.clear();
    }
	void mergeAndJoinRuns(int middle, ofstream& outfile) {

		// initial indexes of first and second subarrays
		int leftIndex = 0, rightIndex = middle;

		// the index we will start at when adding the subarrays back into the main array
		int currentIndex = 0;

		// compare each index of the subarrays adding the lowest value to the currentIndex
		while (leftIndex < middle && rightIndex < mem.size()) {
			if (mem[leftIndex]->id > mem[rightIndex]->id) {
				rightIndex++;
			}
			else if (mem[leftIndex]->id < mem[rightIndex]->id) {
				leftIndex++;
			}
			else {
				// write the join
			}
		}
	}

};

void merge(vector<unique_ptr<Block>>& arr, int start, int middle, int end) {

	vector<Block> leftArray(middle - start + 1);
	vector<Block> rightArray(end - middle);

	// fill in left array
	for (int i = 0; i < leftArray.size(); ++i)
		leftArray[i] = *arr[start + i];

	// fill in right array
	for (int i = 0; i < rightArray.size(); ++i)
		rightArray[i] = *arr[middle + 1 + i];

	/* Merge the temp arrays */

	// initial indexes of first and second subarrays
	int leftIndex = 0, rightIndex = 0;

	// the index we will start at when adding the subarrays back into the main array
	int currentIndex = start;

	// compare each index of the subarrays adding the lowest value to the currentIndex
	while (leftIndex < leftArray.size() && rightIndex < rightArray.size()) {
		if (leftArray[leftIndex].id <= rightArray[rightIndex].id) {
			arr[currentIndex].reset(&leftArray[leftIndex]);
			leftIndex++;
		}
		else {
			arr[currentIndex].reset(&rightArray[rightIndex]);
			rightIndex++;
		}
		currentIndex++;
	}

	// copy remaining elements of leftArray[] if any
	while (leftIndex < leftArray.size()) arr[currentIndex++].reset(&leftArray[leftIndex++]);

	// copy remaining elements of rightArray[] if any
	while (rightIndex < rightArray.size()) arr[currentIndex++].reset(&rightArray[rightIndex++]);
}

void mergeSort(vector<unique_ptr<Block>>& arr, int start, int end) {
	// base case
	if (start < end) {
		// find the middle point
		int middle = (start + end) / 2;

		mergeSort(arr, start, middle); // sort first half
		mergeSort(arr, middle + 1, end);  // sort second half

		// merge the sorted halves
		merge(arr, start, middle, end);
	}
}

int getAllRuns(const int idx, MainMemory& mem, ifstream& empFile, ifstream& deptFile) {
	string line;
	int i = 0;
	Emp e;
	Dept d;

	// Gets the blocks for empFile that are at index = idx in their runs
    while (getline(empFile, line)) {
		if (i == idx) {
			e.parseRow(line);
			// If we are at 11 blocks left, then stop
			if (mem.addEmployee(e) == MM_BLOCKS / 2) {
				break;
			}
		} else {
			if (line == RUN_SEPARATOR) {
				i = 0;
				continue;
			} else {
				++i;
			}
		}
	}
	int start = mem.getSize();
	mem.sortRun(0, mem.getSize() - 1);  

	// Gets the blocks for deptFile that are at index = idx in their runs
    while (getline(deptFile, line)) {
		if (i == idx) {
			d.parseRow(line);
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


void sortMergeJoin(MainMemory& mem, ifstream& empFile, ifstream& deptFile, ofstream& joinFile) {
    string empLine;
    string deptLine;
    Emp e;
    Dept d;

	int middle = getAllRuns(0, mem, empFile, deptFile);
	mem.mergeAndJoinRuns(middle, joinFile);
}

int main (int argc, char* argv[]) {
	ifstream empFile(EMP_INPUT_FILENAME);
	ifstream deptFile(DEPT_INPUT_FILENAME);
	ofstream oEmpFile(EMP_OUTPUT_FILENAME);
	ofstream oDeptFile(DEPT_OUTPUT_FILENAME);
	ofstream joinFile;

	string line;
	Emp e;
	Dept d;
	MainMemory mMemory(MM_BLOCKS);

	// Create sorted runs for Emp relation
	while (getline(empFile, line)) {
		e.parseRow(line);
		if (mMemory.addEmployee(e) == 0) {
			mMemory.writeRun(oEmpFile);
		}
	}
	if (mMemory.getSize() > 0) {
		mMemory.writeRun(oEmpFile);
	}
	empFile.close();
	oEmpFile.close();

	// Create sorted runs for Dept relation
	while (getline(deptFile, line)) {
		d.parseRow(line);
		if (mMemory.addDepartment(d) == 0) {
			mMemory.writeRun(oEmpFile);
		}
	}
	if (mMemory.getSize() > 0) {
		mMemory.writeRun(oDeptFile);
	}
	deptFile.close();
	oDeptFile.close();

	// Join and merge runs 
	empFile.open(EMP_OUTPUT_FILENAME);
	deptFile.open(DEPT_OUTPUT_FILENAME);
	joinFile.open(JOIN_OUTPUT_FILENAME);

	sortMergeJoin(mMemory, empFile, deptFile, joinFile);


	empFile.close();
	deptFile.close();
	joinFile.close();
    return 0;
}