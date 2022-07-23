#include <bits/stdc++.h>
#include <fstream>
using namespace std;
#define N 2005

string input_path = "C:\\Users\\Vishwanath Pawar\\Downloads\\input_small.txt"; // path of input file
string output_path = "C:\\Users\\Vishwanath Pawar\\Downloads\\output-1.txt";   // path of output file
/*
eg input
5 6 50 75
item1   item2   item3  item4   item5   item6
1       1       1      1       0       0
1       1       0      1       0       0
1       0       0      0       1       1
1       0       0      1       1       0
0       1       0      1       1       0
*/

struct node
{
     int support;
     vector<int> t_IDs;
};

int n, m;
vector<vector<int>> transactions(N, vector<int>(N)), pre(N, vector<int>(N, 0));

void generate_k_size_subset(int pos, int k, vector<int> v, vector<int> &arr, vector<vector<int>> &cur)
{
     if (pos == arr.size())
          return;
     generate_k_size_subset(pos + 1, k, v, arr, cur);
     v.push_back(arr[pos]);

     if (v.size() == k)
     {
          cur.push_back(v);
     }

     generate_k_size_subset(pos + 1, k, v, arr, cur);
}

int calculate_support(vector<int> &temp, vector<node> &L1)
{

     int mn = INT_MAX, mn_item;
     for (int i = 0; i < temp.size(); i++)
     {
          if (L1[temp[i]].support < mn)
          {
               mn = L1[temp[i]].support;
               mn_item = temp[i];
          }
     }
     int cnt = 0;
     for (int j = 0; j < L1[mn_item].t_IDs.size(); j++)
     {
          int id, t = 1;
          vector<int> v;
          id = L1[mn_item].t_IDs[j];
          v = transactions[id];
          map<int, int> mp;
          for (int i = 1; i <= m; i++)
          {
               if (v[i])
                    mp[i] = 1;
          }
          for (int i : temp)
          {
               if (mp[i] == 0)
               {
                    t = 0;
               }
          }
          if (t)
               cnt++;
     }
     return cnt * 100 / n;
}

int main()
{

     ifstream input(input_path);
     ofstream output(output_path);
     if (!input)
     {
          cout << "Unable to open file";
          exit(1); // terminate with error
     }
     int x, y, support, confidence;
     string str;
     x = 0, y = 0;
     map<int, string> names;
     input >> n >> m >> support >> confidence;

     for (int i = 1; i <= m; i++)
     {
          input >> str;
          names[i] = str;
     }

     vector<node> L1(m + 1);
     for (int i = 1; i <= n; i++)
     {
          for (int j = 1; j <= m; j++)
          {

               input >> transactions[i][j];
               if (transactions[i][j])
               {
                    L1[j].support++;
                    L1[j].t_IDs.push_back(i);
               }
          }
     }

     // support check---------------------------------------------------------------

     // for 1-size frequent set
     vector<int> qualified(m + 1, 0);
     int z = 0;
     for (int i = 1; i <= m; i++)
     {
          if (1.0 * 100 * L1[i].support / n >= support)
          {
               pre[z++][0] = i;
               qualified[i] = 1;
          }
     }

     // for k-size frequent sets
     int k = 2;
     while (1)
     {

          vector<vector<int>> cur;
          vector<int> arr, v, temp(m + 1, 0);
          for (int i = 1; i <= m; i++)
          {
               if (qualified[i])
                    arr.push_back(i);
               qualified[i] = 0;
          }

          generate_k_size_subset(0, k, v, arr, cur);

          if (cur.size() == 0)
               break;

          z = 0;
          for (int i = 0; i < cur.size(); i++)
          {
               int mn = INT_MAX, mn_item;

               for (int j = 0; j < k; j++)
               {
                    if (L1[cur[i][j]].support < mn)
                    {
                         mn = L1[cur[i][j]].support;
                         mn_item = cur[i][j];
                    }
               }

               int cnt = 0;
               for (int j = 0; j < L1[mn_item].t_IDs.size(); j++)
               {
                    int t = 1;
                    int id = L1[mn_item].t_IDs[j];
                    for (int l = 0; l < k; l++)
                    {
                         int r = cur[i][l];
                         t &= transactions[id][r];
                    }
                    if (t)
                         cnt++;
               }

               if ((1.0 * 100 * cnt / n) >= support)
               {
                    for (int l = 0; l < k; l++)
                    {
                         pre[z][l] = cur[i][l];
                         qualified[cur[i][l]] = 1;
                    }
                    z++;
               }
          }
          if (z == 0)
               break;
          x = z, y = k;
          k++;
     }

     // end of support part -----------------------------------------------------

     // confidence check---------------------------------------------------------

     vector<vector<int>> possible_candidates;
     for (int i = 0; i < x; i++)
     {
          vector<int> temp;
          for (int j = 0; j < y; j++)
               temp.push_back(pre[i][j]);
          possible_candidates.push_back(temp);
     }

     vector<pair<vector<int>, vector<int>>> rules;

     for (auto current_candidate : possible_candidates)
     {
          int len = current_candidate.size();
          k = 1;
          while (k < len)
          {
               vector<vector<int>> cur;
               vector<int> v, w;

               generate_k_size_subset(0, k, v, current_candidate, cur);

               int support_AUB = calculate_support(current_candidate, L1);

               for (auto v : cur)
               {
                    map<int, int> mp;
                    vector<int> w;
                    for (int i : v)
                    {
                         mp[i] = 1;
                    }
                    for (int i : current_candidate)
                    {
                         if (mp[i] == 0)
                         {
                              w.push_back(i);
                         }
                    }

                    int support_A = calculate_support(v, L1);
                    if ((100 * support_AUB) / support_A >= confidence)
                    {
                         rules.push_back({v, w});
                    }
               }
               k++;
          }
     }

     //---------------------------  Association-rules  -----------------------
     // For printing output in output file

     output << "Association rules :" << endl;
     for (auto p : rules)
     {
          output << "if  ( ";
          int ct = 0;
          for (int i : p.first)
          {
               output << " ( " << names[i] << " ) ";
               ct++;
               if (ct < p.first.size())
               {
                    output << " && ";
               }
          }

          output << ") then ( ";
          ct = 0;
          for (int i : p.second)
          {
               output << " ( " << names[i] << " ) ";
               ct++;
               if (ct < p.second.size())
               {
                    output << " && ";
               }
          }
          output << " ) " << endl;
     }

     // For printing output in console

     // for (auto p : rules)
     // {
     //      cout << "if  ( ";
     //      int ct = 0;
     //      for (int i : p.first)
     //      {
     //           cout << " ( " << names[i] << " ) ";
     //           ct++;
     //           if (ct < p.first.size())
     //           {
     //                cout << " && ";
     //           }
     //      }

     //      cout << ") then ( ";
     //      ct = 0;
     //      for (int i : p.second)
     //      {
     //           cout << " ( " << names[i] << " ) ";
     //           ct++;
     //           if (ct < p.second.size())
     //           {
     //                cout << " && ";
     //           }
     //      }
     //      cout << " ) " << endl;
     // }

     return 0;
}
