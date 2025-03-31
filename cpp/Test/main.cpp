//#include "Convention.h"
//#include <Windows.h>
//#include <stdio.h>

#include<iostream>
#include<string>
#include<vector>
#include<stack>
#include<algorithm>
#include<numeric>

using namespace std;
//using namespace convention_kit;

class Solution 
{
public:
    vector<int> countPoints(vector<vector<int>>& points, vector<vector<int>>& queries) 
    {
        vector<int> result;
        for (auto query : queries)
        {
            int sum = 0;
            for (auto point : points)
            {
                int r = query[2];
                int dx = query[0] - point[0];
                int dy = query[1] - query[1];
                if(r*r>=dx*dx+dy*dy)
            }
        }
    }
};

int main()
{

}