#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
class ShelfFloorplan
{
    public:
        ShelfFloorplan(string inputfile_name, bool verbose)
        {
            Verbose = verbose;
            if(verbose)
                FloorplanLog.open("FloorplanLog");
            
            // Read input 
            inputfile.open(inputfile_name);
            outputfile.open("Floorplanresult");
            string line;
            string tmp;
            getline(inputfile, line);
            istringstream iss(line);
            while(iss >> tmp)
            {
                if(tmp == ":")
                    iss >> ChipWidth;
            }
            getline(inputfile, line);
            // Construct Block List
            while(getline(inputfile, line))
            {
                int indice;
                istringstream iss(line);
                string tmp;
                iss >> indice;
                while(iss >> tmp)
                {
                    if(tmp == ":")
                    {
                        iss >> Blocklist[indice].width;
                        iss >> Blocklist[indice].height;
                        if(Blocklist[indice].width > Blocklist[indice].height)
                        {
                            swap(Blocklist[indice].width, Blocklist[indice].height);
                            Blocklist[indice].rotate = 1;
                        }
                        else
                            Blocklist[indice].rotate = 0;
                    }
                }
            }
            // for(int i=0;i < Blocklist.size(); i++)
            //     listorder.push_back(i);
        }
        ~ShelfFloorplan()
        {
            Blocklist.clear();
            inputfile.close();
            FloorplanLog.close();
        }
    ifstream inputfile;
    ofstream FloorplanLog;
    ofstream outputfile;
    typedef struct 
    {
        int width;
        int height;
        int x;
        int y;
        bool rotate;
    } Block;
    typedef struct 
    {
        int hs;
        int x;
        int y;
    } Shelf;
    
    map <int, Block> Blocklist;
    vector<Shelf> Shelflist;
    bool Verbose;
    int ChipWidth;
    struct Sort_Order
    {
        bool operator() (const pair<int, pair<int, int>> v1, const pair<int, pair<int, int>> v2) 
        {
            if(v1.second.first > v2.second.first) return true;
            if (v1.second.first == v2.second.first) return v1.second.second > v2.second.second;
            return false;
        }
    };
    vector<int> listorder;
    void GetOrder(map<int, Block> &Blocklist);
    int SHELF_NF(map<int, Block> &Blocklist, int Chipwidth);
    int SHELF_FF(map<int, Block> &Blocklist, int Chipwidth);
    bool WhetherRotate(Block &block, int hs);
    void NewShelf(Block &block, int &hs, int &x, int &y);
    void AssignNewBlockFF(Block &block, Shelf &Sf);
    void AssignNewBlockNF(Block &block, int &hs, int &x, int &y);
    void run();
};
