#include <iostream>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

int main(){
    while(1){
        srand((unsigned)time(NULL));
        cout << rand()%100 << endl;
        sleep(1);
    }
}