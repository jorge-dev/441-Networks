#include <iostream>
#include <string>
#include <cstring>
#include <regex>
using namespace std;

int main() {
    string s1 = "i have a word Floppy in here ";
    regex s2 ("Floppy");
    smatch m;
    if(!regex_search(s1,m,s2))
    {
	cout << "Found" << endl;
    }
    else
	cout << "Not found" << endl;
    return 0;
}
