#include <unistd.h>
#include <iostream>
#include <vector>

int main()
{
    std::vector<int> v = {10, 20, 30, 40, 50, 60, 70, 80, 90};
    

    for (size_t i = 0; i < v.size(); i++)
    {
        std::cout <<"i : " << i << " v[i] : " << v[i] << std::endl;
    }
    int a = -1;
    
    std::cout << v.size() << std::endl << *v.end() << std::endl;

    v.erase(v.begin() + 2);
    v.erase(v.begin() + 7);

    std::cout << v.size() << std::endl << *v.end() << std::endl;

    

    return 0;
}