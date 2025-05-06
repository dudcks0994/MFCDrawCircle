#include <string>
#include <vector>
#include <set>
#include <stdio.h>

using namespace std;

double cal_inclination(vector<int> a, vector<int>b)
{
    double dx = a[0] - b[0];
    double dy = a[1] - b[1];
    return dy/dx;
}

int solution(vector<vector<int>> dots) {
    double a1 = cal_inclination(dots[0], dots[1]);
    double a2 = cal_inclination(dots[2], dots[3]);
    if (a1 == a2)
        return 1;
    double a3 = cal_inclination(dots[0], dots[2]);
    double a4 = cal_inclination(dots[1], dots[3]);
    if (a3 == a4)
        return 1;
    double a5 = cal_inclination(dots[0], dots[3]);
    double a6 = cal_inclination(dots[1], dots[2]);
    if (a5 == a6)
        return 1;
    return 0;
}