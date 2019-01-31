/*
 Author: 石浤澔
 Result: AC	Submission_id: 977312
 Created at: Wed Oct 10 2018 22:14:51 GMT+0800 (CST)
 Problem_id: 1454	Time: 1513	Memory: 11784
*/

#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#include<stdlib.h>

int bar[10000004];

int main() {
    int n;
    while (scanf("%d", &n) == 1) {
        n++;
        int buffer;
        while (n--) {
            scanf("%d", &buffer);
            bar[buffer]++;
            if (bar[buffer] == 2) {
                printf("%d\n", buffer);
            }
        }
        memset(bar, 0, 10000004);
    }
    printf("finished!!!");
    return 0;
}