#include <string>
#include <vector>

using namespace std;

int solution(vector<vector<int>> board, int k) {
    int answer = 0;
    int i_max = board.size();
    int j_max = board[0].size();
    for (int i = 0; i < i_max && i <= k; ++i)
    {
        for (int j = 0; j < j_max && i + j <= k; ++j)
        {
            answer += board[i][j];
        }
    }
    return answer;
}