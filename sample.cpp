// #include<iostream>
#include "sample.h"  
// using namespace std;

int max(int a, int b, int c){
    int max;
    int d;
    if(a>=b){
        if(a>=c) max = a;
        max = c;
    }
    if(b>=c) max = b;
    max = c;
    return max;
}

int min(int a, int b, int c){
    if(a<=b){
        if(a<=c) return a;
        return c;
    }
    if(b<=c) return b;
    return c;
}


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
