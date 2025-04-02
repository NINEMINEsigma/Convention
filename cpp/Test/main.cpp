//#include "Convention.h"
//#include <Windows.h>
//#include <stdio.h>

#include<iostream>
#include<string>
#include<vector>
#include<stack>
#include<algorithm>
#include<map>
#include<numeric>

using namespace std;
//using namespace convention_kit;

class Solution 
{
public:
    int trap(vector<int>& height) 
    {
        int n = height.size();
        vector<int> left(n, 0), right(n, 0);
        *left.begin() = *height.begin();
        for (int i = 1; i != n; i++)
        {
            left[i] = std::max(left[i - 1], height[i]);
        }
        *right.rbegin() = *height.rbegin();
        for (int i = n-2; i !=-1; i--)
        {
            right[i] = std::max(right[i + 1], height[i]);
        }
        int result = 0;
        for (int i = 0; i != n; i++)
        {
            result += std::max(0, std::min(right[i], left[i]) - height[i]);
        }
        return result;
    }
};

int main()
{
    map<int, int> a;
    a[1] = 1;
    a[2] = 2;
    for (auto [key, b] : a)
    {
        cout << key << ": " << b << endl;
    }
}