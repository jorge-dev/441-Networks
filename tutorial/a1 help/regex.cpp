#include <iostream> 
#include <string> 
#include <regex> 
#include <iterator> 
using namespace std; 
  
int main() 
{  
    char s [1000]= "I am looking for GeeksForGeek \n"; 
      
    // matches words beginning by "Geek" 
    regex r("Geek[a-zA-z]"); 
    char u [3333]; 
    // regex_replace() for replacing the match with 'geek'  
    u = regex_replace(s, r, "geek");
      cout << u << endl;
      
    string result; 
      
    //regex_replace( ) for replacing the match with 'geek' 
//    regex_replace(back_inserter(result), s.begin(), s.end(), r,  "geek"); 
  
    cout << result; 
  
    return 0; 
} 
