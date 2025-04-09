#include <iostream>
#include <vector>
#include <map>

std::map<int, std::vector<int>> prnTaps = {
    {1, {2, 6}},  
    {2, {3, 7}}, 
    {3, {4, 8}},
    {4, {5, 9}}, 
    {5, {1, 9}}, 
    {6, {2, 10}}, 
    {7, {1, 8}}, 
    {8, {2, 9}}, 
    {9, {3, 10}}, 
    {10, {2, 3}}, 
    {11, {3, 4}}, 
    {12, {5, 6}}, 
    {13, {6, 7}}, 
    {14, {7, 8}}, 
    {15, {8, 9}}, 
    {16, {9, 10}}, 
    {17, {1, 4}}, 
    {18, {2, 5}}, 
    {19, {3, 6}}, 
    {20, {4, 7}}, 
    {21, {5, 8}}, 
    {22, {6, 9}}, 
    {23, {1, 3}}, 
    {24, {4, 6}}, 
    {25, {5, 7}}, 
    {26, {6, 8}}, 
    {27, {7, 9}}, 
    {28, {8, 10}}, 
    {29, {1, 6}}, 
    {30, {2, 7}}, 
    {31, {3, 8}}, 
    {32, {4, 9}},    
};

std::vector<int> shift(std::vector<int>& G,const std::vector<int>& indices){

    int sumMod = 0;

    for (int i : indices){
        sumMod += G[i - 1];
    }

    sumMod = sumMod%2;

    for (int j = G.size() - 1;j>0;j--){
        G[j] = G[j-1];
    }
    G[0] = sumMod;
    return G;
  }

  std::vector<int> caCode(int sv){
    std::vector<int> G1(10, 1);  
    std::vector<int> G2(10, 1);
    std::vector<int> indicesG1 = {3, 10};
    std::vector<int> indicesG2 = {2,3,6,8,9,10};

        int k {0};
        std::vector<int> ca;
        int g1 {0};
        int g2 {0};
        while (k < 10) {
            g1 = G1[9];
            g2 = (G2[prnTaps[sv][0] - 1] + G2[prnTaps[sv][1]-1])%2;
            G1 = shift(G1,indicesG1); 
            G2 = shift(G2,indicesG2);
            ca.push_back((g1 + g2)%2);
            ++k;
        }
        return ca; 
  }

  int main(){

    auto result = caCode(1);

    std::cout << "caCodesv1: ";
    for (int val : result) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    return 0;
  }