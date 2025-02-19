#include "Convention.h"
#include <Windows.h>

using namespace std;
using namespace convention_kit;

class Solution
{
public:
    vector<string> generateParenthesis(int n)
    {
        vector<string> res;
        func(res, "", 0, 0, n);
        return res;
    }

    void func(vector<string>& res, string str, int l, int r, int n) 
    {
        if (l > n || r > n || r > l) return;
        if (l == n && r == n) { res.push_back(str); return; }
        func(res, str + '(', l + 1, r, n);
        func(res, str + ')', l, r + 1, n);
        return;
    }
};
int main()
{
    auto file = make_instance(filesystem::path("F:"));
    for (auto&& i : file.dir_iter())
    {
        cout << i << "\n";
    }
}