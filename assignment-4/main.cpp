#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <cstdlib>
using namespace std;

const int INTEGER_SIZE = 4;
const int DOUBLE_SIZE = 8;
const int STRING_SIZE = 40;
const int MM_BLOCKS = 22;
const int MM_BLOCKS_SQRD = 484;
const string EMP_INPUT_FILENAME = "Emp.csv";
const string DEPT_INPUT_FILENAME = "Dept.csv";
const string EMP_OUTPUT_FILENAME = "EmpOut.";
const string DEPT_OUTPUT_FILENAME = "DeptOut.";
const string JOIN_OUTPUT_FILENAME = "join.csv";
const string RUN_SEPARATOR = "===========";

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
		ostringstream s;
		s << did << "," << dname << "," << budget << "," << id;
		return s.str();
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
		ostringstream s;
		s << id << "," << ename << "," << age << "," << salary;
		return s.str();
	}
};

class RunPointer {
public:
	Block* data;
	ifstream* filePtr;

	RunPointer(ifstream* file) {
		filePtr = file;
	}

	~RunPointer() {
		if (filePtr->is_open()) {
			filePtr->close();
			delete filePtr;
		}
		delete data;
	}
};

void merge(vector<RunPointer*>& mem, int start, int middle, int end) {
	vector<RunPointer*> leftArray;
	vector<RunPointer*> rightArray;

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

	// compare each index of the subArrays adding the lowest value to the currentIndex
	while (leftIndex < middle - start + 1 && rightIndex < end - middle) {
		if (leftArray[leftIndex]->data->id <= rightArray[rightIndex]->data->id) {
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
		mem[currentIndex++] = leftArray[leftIndex++];
	}

	// copy remaining elements of rightArray[] if any
	while (rightIndex < end - middle) {
		mem[currentIndex++] = rightArray[rightIndex++];
	}
}

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
		mem[currentIndex++] = leftArray[leftIndex++];
	}

	// copy remaining elements of rightArray[] if any
	while (rightIndex < end - middle) {
		mem[currentIndex++] = rightArray[rightIndex++];
	}
}

template <class T>
void mergeSort(vector<T>& mem, int start, int end) {
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
		mem.clear();
    }

	void getEmployeesFromRuns(vector<RunPointer*>& empRuns) {
		string empLine;
		for (int i = 0; i < empRuns.size(); ++i) {
			if (getline(*(empRuns[i]->filePtr), empLine)) {
				Emp* e = new Emp();
				e->parseRow(empLine);
				empRuns[i]->data = e;
			}
		}
	}

	void getDepartmentsFromRuns(vector<RunPointer*>& deptRuns) {
		string deptLine;
		for (int i = 0; i < deptRuns.size(); ++i) {
			if (getline(*(deptRuns[i]->filePtr), deptLine)) {
				Dept* d = new Dept();
				d->parseRow(deptLine);
				deptRuns[i]->data = d;
			}
		}
	}

	void clearVectorBlocks(vector<Block*>& blocks) {
		for (int i = 0; i < blocks.size(); i++) {
			delete blocks[i];
		}
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

	Block* readNextInRun(ifstream& run, bool employee) {
		string line;
		if (getline(run, line)) {
			if (employee) {
				Emp* e = new Emp();
				e->parseRow(line);
				return e;
			} else {
				Dept* d = new Dept();
				d->parseRow(line);
				return d;
			}
		}
		return NULL;
	}

	void sortMergeJoin(vector<RunPointer*>& empRuns, vector<RunPointer*>& deptRuns, ofstream& joinFile) {
		string empLine;
		string deptLine;
		Block * temp;

		getEmployeesFromRuns(empRuns);
		getDepartmentsFromRuns(deptRuns);
		for (int i = 0; i < empRuns.size(); i++) {
			mergeSort(empRuns, 0, empRuns.size() - 1);
			for (int k = 0; k < deptRuns.size(); k++) {
				mergeSort(deptRuns, 0, deptRuns.size() - 1);
				if (empRuns[i]->data && deptRuns[k]->data) {
					if (empRuns[i]->data->id < deptRuns[k]->data->id) {
						temp = empRuns[i]->data;
						empRuns[i]->data = readNextInRun(*empRuns[i]->filePtr, true);
						delete temp;
						if (!empRuns[i]->data) {
							delete empRuns[i];
							empRuns.erase(empRuns.begin() + i);
						}
						i = -1;
						break;
					} else if (empRuns[i]->data->id > deptRuns[k]->data->id) {
						temp = deptRuns[k]->data;
						deptRuns[k]->data = readNextInRun(*deptRuns[k]->filePtr, false);
						delete temp;
						if (!deptRuns[k]) {
							delete deptRuns[k];
							deptRuns.erase(deptRuns.begin() + k);
						}
					} else {
						joinFile << empRuns[i]->data->getWritableRow() << "," << deptRuns[k]->data->getWritableRow() << endl;
						temp = deptRuns[k]->data;
						deptRuns[k]->data = readNextInRun(*deptRuns[k]->filePtr, false);
						delete temp;
						if (!deptRuns[k]) {
							delete deptRuns[k];
							deptRuns.erase(deptRuns.begin() + k);
						}
						k--;
					}
				}
			}
		}
	}

	~MainMemory() {
		for (int i = 0; i < mem.size(); i++) {
			delete mem[i];
		}
	}

};

int main (int argc, char* argv[]) {
	ifstream empFile;
	ifstream deptFile;
	ofstream joinFile;

	string line;
	MainMemory mMemory(MM_BLOCKS);
	vector<RunPointer*> empRuns;
	vector<RunPointer*> deptRuns;
	int empRunCount = 0, deptRunCount = 0;

	empFile.open(EMP_INPUT_FILENAME.c_str(), ios::in);
	
	// Create sorted runs for Emp relation
	while (getline(empFile, line)) {
		Emp *e = new Emp();
		e->parseRow(line);
		if (mMemory.addEmployee(e) == 0) {
			ostringstream fileName; 
			fileName << EMP_OUTPUT_FILENAME << "r" << empRunCount << ".txt";
			ofstream outRun(fileName.str().c_str(), ios::out);
			mMemory.writeRun(outRun);
			outRun.close();

			empRuns.push_back(new RunPointer(new ifstream(fileName.str().c_str(), ios::in)));
			empRunCount++;
		}
	}
	if (mMemory.getSize() > 0) {
		ostringstream fileName; 
		fileName << EMP_OUTPUT_FILENAME << "r" << empRunCount << ".txt";
		ofstream outRun(fileName.str().c_str(), ios::out);
		mMemory.writeRun(outRun);
		outRun.close();

		empRuns.push_back(new RunPointer(new ifstream(fileName.str().c_str(), ios::in)));
		empRunCount++;
	}
	empFile.close();

	deptFile.open(DEPT_INPUT_FILENAME.c_str(), ios::in);
	// Create sorted runs for Dept relation
	while (getline(deptFile, line)) {
		Dept* d = new Dept();
		d->parseRow(line);
		if (mMemory.addDepartment(d) == 0) {
			ostringstream fileName; 
			fileName << DEPT_OUTPUT_FILENAME << "r" << deptRunCount << ".txt";
			ofstream outRun(fileName.str().c_str(), ios::out);
			mMemory.writeRun(outRun);			
			outRun.close();

			deptRuns.push_back(new RunPointer(new ifstream(fileName.str().c_str(), ios::in))); 
			deptRunCount++;
		}
	}
	if (mMemory.getSize() > 0) {
		ostringstream fileName; 
		fileName << DEPT_OUTPUT_FILENAME << "r" << deptRunCount << ".txt";
		ofstream outRun(fileName.str().c_str(), ios::out);
		mMemory.writeRun(outRun);
		outRun.close();

		deptRuns.push_back(new RunPointer(new ifstream(fileName.str().c_str(), ios::in)));
		deptRunCount++;
	}
	deptFile.close();

	joinFile.open(JOIN_OUTPUT_FILENAME.c_str(), ios::out);

	mMemory.sortMergeJoin(empRuns, deptRuns, joinFile);

	for (int i = 0; i < empRuns.size(); i++) {
		delete empRuns[i];
	}

	for (int i = 0; i < deptRuns.size(); i++) {
		delete deptRuns[i];
	}

	joinFile.close();
    return 0;
}
