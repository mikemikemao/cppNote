// leetcode.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <set>
#include <stdexcept>
#include <string>
#include <cstdlib> //abort()
#include <cstdio>  //snprintf()
#include <iostream>
#include <ctime>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <list>
using namespace std;
namespace two_sum
{
    /*输入：nums = [2,7,11,15], target = 9
输出：[0,1]
解释：因为 nums[0] + nums[1] == 9 ，返回 [0, 1] 。
来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/two-sum
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。*/
    class SolutionTraverse
    {
    public:
        vector<int> twoSum(vector<int>& nums, int target)
        {
            for (int i = 0; i < nums.size(); i++)
                for (int j = i + 1; j < nums.size(); j++)
                {
                    if (nums.at(j) == target - nums.at(i))
                    {
                        return { i, j };
                    }
                }
            return {};
        }
    };

    class SolutionHash
    {
    public:
        vector<int> twoSum(vector<int>& nums, int target)
        {
            unordered_map<int, int> hashtable;
            for (int i = 0; i < nums.size(); ++i)
            {
                auto it = hashtable.find(target - nums[i]);
                if (it != hashtable.end())
                {
                    return { it->second, i };
                }
                hashtable[nums[i]] = i;
            }
            return {};
        }
    };
    int test()
    {
        vector<int> intVec = { 2, 7, 11, 15 };
        int target = 9;
        SolutionTraverse solutionTraverse;
        vector<int> resultVec = solutionTraverse.twoSum(intVec, target);
        cout << "SolutionTraverse Result:\n"
            << endl;
        for (auto& vect : resultVec)
            cout << vect << endl;
        cout << "SolutionHash Result:\n"
            << endl;
        SolutionHash solutionHash;
        resultVec = solutionHash.twoSum(intVec, target);
        for (auto& vect : resultVec)
            cout << vect << endl;
    }
} // namespace name

/////////////////////////////////////////////////////////////////////////
// 给你两个 非空 的链表，表示两个非负的整数。它们每位数字都是按照 逆序 的方式存储的，并且每个节点只能存储 一位 数字。

// 请你将两个数相加，并以相同形式返回一个表示和的链表。

// 你可以假设除了数字 0 之外，这两个数都不会以 0 开头。

//  

// 来源：力扣（LeetCode）
// 链接：https://leetcode-cn.com/problems/add-two-numbers
// 著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。
// 输入：l1 = [2,4,3], l2 = [5,6,4]
// 输出：[7,0,8]
// 解释：342 + 465 = 807.
namespace add_two_numbers
{
    struct ListNode
    {
        int val;
        ListNode* next;
        ListNode() : val(0), next(nullptr) {}
        ListNode(int x) : val(x), next(nullptr) {}
        ListNode(int x, ListNode* next) : val(x), next(next) {}
    };
    class Solution
    {
    public:
        ListNode* addTwoNumbers(ListNode* l1, ListNode* l2)
        {
            ListNode* dummy = new ListNode();
            ListNode* curr = dummy;
            int carry = 0;
            while (l1 || l2)
            {
                int x = l1 ? l1->val : 0;
                int y = l2 ? l2->val : 0;

                int sum = x + y + carry;
                curr->next = new ListNode(sum % 10);
                // bug 修复：视频中忘了移动 curr 指针了
                curr = curr->next;
                carry = sum / 10;

                if (l1)
                    l1 = l1->next;
                if (l2)
                    l2 = l2->next;
            }
            if (carry != 0)
                curr->next = new ListNode(carry);
            return dummy->next;
        }
    };
    void test()
    {
        ListNode* pListNodl1 = NULL;
        ListNode* pend = NULL;
        ListNode* pnew = NULL;
        ListNode* pListNodl2 = NULL;
        int valel1[7] = { 9, 9, 9, 9, 9, 9, 9 };
        int valel2[4] = { 9, 9, 9, 9 };
        for (size_t i = 0; i < 7; i++)
        {
            pnew = (ListNode*)malloc(sizeof(ListNode));
            pnew->val = valel1[i];
            pnew->next = NULL;
            if (pListNodl1 == NULL)
            {
                pListNodl1 = pnew;
                pend = pListNodl1;
            }
            else
            {
                pend->next = pnew;
                pend = pnew;
            }
        }
        for (size_t i = 0; i < 4; i++)
        {
            pnew = (ListNode*)malloc(sizeof(ListNode));
            pnew->val = valel2[i];
            pnew->next = NULL;
            if (pListNodl2 == NULL)
            {
                pListNodl2 = pnew;
                pend = pListNodl2;
            }
            else
            {
                pend->next = pnew;
                pend = pnew;
            }
        }

        Solution msolution;
        ListNode* l3 = msolution.addTwoNumbers(pListNodl1, pListNodl2);
        for (size_t i = 0; i < 9; i++)
        {
            if (l3 != NULL)
            {
                cout << l3->val << endl;
                l3 = l3->next;
            }
        }
    }
}
/*
给定一个字符串 s ，请你找出其中不含有重复字符的 最长子串 的长度。
示例 1:
输入: s = "abcabcbb"
输出: 3
解释: 因为无重复字符的最长子串是 "abc"，所以其长度为 3。
示例 2:
输入: s = "bbbbb"
输出: 1
解释: 因为无重复字符的最长子串是 "b"，所以其长度为 1。
示例 3:
输入: s = "pwwkew"
输出: 3
解释: 因为无重复字符的最长子串是 "wke"，所以其长度为 3。
     请注意，你的答案必须是 子串 的长度，"pwke" 是一个子序列，不是子串。
示例 4:
输入: s = ""
输出: 0
提示：
0 <= s.length <= 5 * 104
s 由英文字母、数字、符号和空格组成
来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/longest-substring-without-repeating-characters
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。*/
//---------------------------------------------------
#include <set>
#include <stdexcept>
#include <string>
#include <cstdlib> //abort()
#include <cstdio>  //snprintf()
#include <iostream>
#include <ctime>
namespace lengthOfLongestSubstring
{
    class Solution
    {
    public:
        int lengthOfLongestSubstring(string s)
        {
            set<char> current;
            set<char> present;
            int presentsize = 0;
            int currentsize = 0;
            int times = 0;
            for (int i = 0; i < s.size(); i++)
            {
                presentsize = current.size();
                cout << "presentsize=" << presentsize << endl;
                current.insert(s[i]);
                currentsize = current.size();
                cout << "currentsize=" << currentsize << endl;
                if (currentsize == presentsize)
                {
                    if (times == 0)
                    {
                        for (auto temp : current)
                        {
                            present.insert(temp);
                        }
                        current.clear();
                        current.insert(s[i]);
                    }
                    else
                    {
                        //碰到相同的字符 判断前后set的数量 数量小的抛弃
                        if (current.size() >= present.size())
                        {
                            present.clear();
                            for (auto temp : current)
                            {
                                present.insert(temp);
                            }
                        }
                        current.clear();
                        current.insert(s[i]);
                    }
                    times++;
                }
            }
            if (current.size() >= present.size())
            {
                return current.size();
            }

            return present.size();
        }
    };
    int test()
    {
        string s = " ";
        Solution solution;
        int num = solution.lengthOfLongestSubstring(s);
        cout << "num=" << num << endl;
    }

}


int main()
{
    two_sum::test();
    add_two_numbers::test();
    lengthOfLongestSubstring::test();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
