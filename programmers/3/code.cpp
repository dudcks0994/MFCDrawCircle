#include <string>
#include <vector>
#include <set>
#include <map>
#include <cmath>

using namespace std;

int solution(int a, int b, int c, int d) {
    map<int, int> m;
    m[a]++;
    m[b]++;
    m[c]++;
    m[d]++;
    if (m.size() == 1)
        return (m.begin())->first * 1111;
    auto i = m.begin();
    int num = i->first;
    int count = i->second;
    auto next = ++i;
    if ((count == 1 || count == 3) && m.size() == 2)
    {
        if (count == 1)
            return pow(10 * next->first + num, 2);
        else
            return pow(10 * num + next->first, 2);
    }
    if (count == 2 && m.size() == 2)
        return (num + next->first) * abs(num - next->first);
    if (count == 2 && m.size() == 3)
    {
        num = next->first;
        int nnext = (++next)->first;
        return num * nnext;
    }
    if (count == 1 && m.size() == 3)
    {
        if (next->second == 2)
            ++next;
        return (num * next->first);
    }
    return num;
}