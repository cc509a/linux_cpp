#include <iostream>

using namespace std;

class Solution {

private:
      Solution(){}
    
public:
    /**
     * @return: The same instance of this class every time
     */
    static Solution* getInstance() {
        // write your code here
        if(instance == NULL)
        {
            instance = new Solution();
        }
        return instance;
    }
    static Solution *instance;

};

Solution* Solution::instance = nullptr;

int main (int argc,const char* argv[])
{
    Solution::getInstance();
    return 0;
}