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
    int minimumOperations(vector<int>& nums) 
    {
        map<int,int> mapper;
        for (auto&& num : nums)
            mapper[num]++;
        int i = 0;
        for (int e = nums.size(); i*3 < e;i++)
        {
            if (any_of(mapper.begin(), mapper.end(), [](auto& p) {return p.second > 1; }) == false)
                break;
            for (int j = i * 3; j < e; j++)
            {
                mapper[nums[j]]--;
            }
        }
        return i;
    }
};

int main()
{
    vector<int> nums = { 1,2,3,4,2,3,3,5,7 };
    Solution().minimumOperations(nums);
}