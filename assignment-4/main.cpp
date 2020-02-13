#include <iostream>
#include <string>

#define INTEGER_SIZE 4
#define DOUBLE_SIZE 8
#define STRING_SIZE 40
#define MM_BLOCKS 22

using namespace std;

class Dept {
private:    
    int did;
    string dname;
    double budget;
    int managerid;
public:
    int getDid() { return did; }
    string getDname() { return dname; }
    double getBudget() { return budget; }
    int getManagerId() { return managerid; }
};

class Emp {
private:
    int eid;
    string ename;
    int age;
    double salary;
public:
    int getEid() { return eid; }
    string getEname() { return ename; }
    int getAge() { return age; }
    double getSalary() { return salary; }
};