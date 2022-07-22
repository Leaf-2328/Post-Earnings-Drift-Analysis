//6883 Final Project

#include <iostream>
#include "Market.h"

using namespace FinalProj;
using namespace std;

bool valid(char op){
    string s="12345"; // Vector of char
    for(char a : s){
        if(op==a) return true;
    }
    return false;
}


int main() {
    cout << "6883 Final Project" << endl;
    
    //Procedure starts:
    Market market;
    bool work = true; // Flag
    while(work){
        cout << "Please choose an operation:" << endl;
        cout << "1. N; 2. Stock; 3. Group; 4. Graph; 5. Exit" << endl;
        char op;
        cin >> op; // Only catch the first character
        while(!valid(op)) {
            cout << "Please enter a valid operation" << endl;
            cin>>op;
        }
        work = market.action(op);
    }

}

