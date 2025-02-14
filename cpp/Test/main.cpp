#include "Convention.h"
#include <Windows.h>

using namespace std;
using namespace convention_kit;

class Solution
{
public:
    vector<vector<int>> threeSum(vector<int>& nums)
    {
        sort(nums.begin(), nums.end());
        vector<vector<int>> ans;
        for (int i = 0; i < nums.size(); i++)
        {
            if (i > 0 && nums[i] == nums[i - 1]) continue;
            int j = i + 1, k = nums.size() - 1;
            while (j < k)
            {
                if (nums[i] + nums[j] + nums[k] == 0)
                {
                    ans.push_back({ nums[i], nums[j], nums[k] });
                    j++;
                    while (nums[j] == nums[j - 1] && j < k) j++;
                }
                else if (nums[i] + nums[j] + nums[k] < 0)
                {
                    j++;
                }
                else 
                {
                    k--;
                }
            }
        }
        return ans;
    }
};

int main()
{

}