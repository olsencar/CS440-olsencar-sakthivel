#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <bits/stdc++.h>

#define INTEGER_SIZE 4
#define DOUBLE_SIZE 8
#define STRING_SIZE 40
#define MM_BLOCKS 22
#define MM_BLOCKS_SQRD 484

using namespace std;

class Dept {
public:
    int did;
    string dname;
    double budget;
    int managerid;

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
        managerid = atoi(row[3].c_str());
    }
};

class Emp {
public:
    int eid;
    string ename;
    int age;
    double salary;

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
        eid = atoi(row[0].c_str());
        ename = row[1];
        age = atoi(row[2].c_str());
        salary = atof(row[3].c_str());
    }
};


class MainMemory {
private:
    int size;
    int capacity;
    vector<Emp> employees;
    vector<Dept> departments;

    void checkForCapacityReached() {
        if (size == capacity - 1) {
            // We have reached the max capacity
            sort(employees.begin(), employees.end(), less_than_key_emp());
            sort(departments.begin(), departments.end(), less_than_key_dept());
    
            // Now that both are sorted, we merge them ?
        }
    }


public:
    MainMemory(int cap) {
        capacity = cap;
    }

    int getSize() { return size; }
    vector<Emp> getEmployees() { return employees; }
    vector<Dept> getDepartments() { return departments; }

    void addEmployee(Emp e) {
        employees.push_back(e);
        size++;
        checkForCapacityReached();
    }

    void addDepartment(Dept d) {
        departments.push_back(d);
        size++;
        checkForCapacityReached();
    }
};

struct less_than_key_emp {
    inline bool operator() (const Emp& e, const Emp& e2) {
        return (e.eid < e2.eid);
    }
};

struct less_than_key_dept {
    inline bool operator() (const Dept& d, const Dept& d2) {
        return (d.did < d2.did);
    }
};

struct less_than_key_dept_emp {
    inline bool operator() (const Dept& d, const Emp& e) {
        return (d.did < e.eid);
    }
};

vector<Emp> readFiles(MainMemory& mem, string empFileName, string deptFileName) {
    ifstream empFile(empFileName);
    ifstream deptFile(deptFileName);
    string empLine;
    string deptLine;
    Emp e;
    Dept d;
    // Ignore the CSV headers in each file
    getline(empFile, empLine);
    getline(deptFile, deptLine);

    // We can only fit Emp and Dept into 22 MM blocks
    // Memory requirement: B(R) + B(S) <= M^2
    // Since 1 record = 1 Block, then NumRows(Emp) + NumRows(Dept) <= 484

    while (getline(empFile, empLine) && getline(deptFile, deptLine)) {
        e.parseRow(empLine);
        d.parseRow(deptLine);
        mem.addEmployee(e);
        mem.addDepartment(d);

    }
    empFile.close();
    deptFile.close();
}

int main (int argc, char* argv[]) {

    return 0;
}