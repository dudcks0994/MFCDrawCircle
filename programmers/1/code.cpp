#include <string>
#include <vector>

using namespace std;

int solution(int n, int k) {
    int bonus = n / 10;
    return 12000 * n + 2000 * (k - bonus);
}