#include <stdio.h>

int main(void){
    int arr[5] = {5,1,15,20,25};
    int i, j, k;
    i = ++arr[1];
    j = arr[1]++;
    k = arr[i++];
    printf ( "%d, %d, %d", i, j, k );
}