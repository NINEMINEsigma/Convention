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
    vector<int> corpFlightBookings(vector<vector<int>>& bookings, int n)
    {
        vector<int> ds(n + 1, 0);
        vector<int> result(n + 1, 0);
        for (auto&& line : bookings)
        {
            ds[line[0] - 1] += line[2];
            ds[line[1]] -= line[2];
        }
        for (int i = 0; i < n; i++)
        {
            result[i + 1] = result[i] += ds[i];
        }
        result.pop_back();
        return result;
    }
};

int main()
{
    vector<vector<int>> bookings = {
        {1,2,10},
        {2,3,20},
        {2,5,25}
    };
    Solution().corpFlightBookings(bookings, 5);
}