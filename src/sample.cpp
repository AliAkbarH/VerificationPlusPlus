// #include<iostream>
#include "sample.h"  
// using namespace std;



int main(int argc, char *argv[])
{
    int n;
    n=10;
    int *arr=new int[n];
    int ***a;
    for(int i=0;i<n;i++){
        arr[i]=i;
        arr[i]=arr[i]+1;
    }   
    int target=5;
    bool found=false;
    int index=0;

    while(index<n){
        if(arr[index]==target){
            found=true;
            break;
        }
        index=index+1;
    }
    if(found){
        return index;
    }
    else{
        return -1;
    }
}