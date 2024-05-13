#include "ShelfFloorplan.h"

void ShelfFloorplan::GetOrder(map<int, Block> &Blocklist)
{
    vector <pair<int, pair<int, int>>> tmp_vct;
    for(int i  = 0; i < Blocklist.size(); i++)
        tmp_vct.push_back(make_pair(i, make_pair(Blocklist[i].width, Blocklist[i].height)));
    sort(tmp_vct.begin(), tmp_vct.end(), Sort_Order());
    for(int i = 0; i < tmp_vct.size(); i++)
        listorder.push_back(tmp_vct[i].first);
}
bool ShelfFloorplan::WhetherRotate(Block &block, int hs)
{
    if((hs <= block.width && block.width <= block.height) || (block.width <= hs && hs <= block.height) || (block.height <= block.width && block.width <= hs))
    {
        block.rotate  = ! block.rotate;
        swap(block.height, block.width);
        return true;
    }
    else 
    {   
        return false;
    }
}
void ShelfFloorplan::NewShelf(Block &block, int &hs, int &x, int &y)
{
    if(block.width <= block.height)
        swap(block.height, block.width);
    x = 0;
    y = y + hs;
    block.x = x;
    block.y = y;
    block.rotate = 1;
    x = x + block.width;
    hs = block.height;
}
void ShelfFloorplan::AssignNewBlockNF(Block &block, int &hs, int &x, int &y)
{
    if(ChipWidth < x + block.width)
    {
        NewShelf(block, hs, x, y);
    }
    else
    {
        block.x = x;
        x = x + block.width;
        block.y = y;
        if(hs < block.height)
            hs = block.height;
    }
}
void ShelfFloorplan::AssignNewBlockFF(Block &block, Shelf &Sf)
{
    if(ChipWidth < Sf.x + block.width)
    {
        Shelf newSf;
        if(block.width <= block.height)
            swap(block.height, block.width);
        newSf.y = Sf.y + Sf.hs;
        block.x = 0;
        block.y = newSf.y;
        block.rotate = 1;
        newSf.x = block.width;
        newSf.hs = block.height;
        Shelflist.push_back(newSf);
    }
    else
    {
        block.x = Sf.x;
        Sf.x = Sf.x + block.width;
        block.y = Sf.y;
        if(Sf.hs < block.height)
            Sf.hs = block.height;
    }
}
int ShelfFloorplan::SHELF_FF(map<int, Block> &Blocklist, int Chipwidth)
{
    Shelf Sf;
    Sf.hs = 0;
    Sf.x = 0;
    Sf.y = 0;
    Shelflist.push_back(Sf);
    for(auto &lo : listorder)
    {
        // Closed Shelves
        int i;
        bool fit = false;
        for(i = 0; i < Shelflist.size() - 1; i++)
        {   
            if(min(Blocklist[lo].height, Blocklist[lo].width) > Shelflist[i].hs)
                continue;
            else 
            {
                WhetherRotate(Blocklist[lo], Shelflist[i].hs);
                if(Shelflist[i].x + Blocklist[lo].width > ChipWidth)
                    continue;
                else
                {
                    Blocklist[lo].x = Shelflist[i].x;
                    Shelflist[i].x += Blocklist[lo].width;
                    Blocklist[lo].y = Shelflist[i].y;
                    fit = true;
                }
            }
        }
        if(!fit)
        {
            // Open Shelf
            WhetherRotate(Blocklist[lo], Shelflist[i].hs);
            AssignNewBlockFF(Blocklist[lo], Shelflist[i]);
        }
    }
    auto Sf_end = Shelflist.rbegin();
    int final_height = Sf_end->hs + Sf_end->y;
    return final_height;
}
int ShelfFloorplan::SHELF_NF(map<int, Block> &Blocklist, int Chipwidth)
{
    int x = 0;
    int y = 0;
    int hs = 0;
    for(auto &it : Blocklist)
    {
        WhetherRotate(it.second, hs);
        AssignNewBlockNF(it.second, hs, x, y);
    }
    int final_height = y + hs;
    return final_height;
}
void ShelfFloorplan::run()
{   
    GetOrder(Blocklist);
    int final_height;
    final_height = SHELF_FF(Blocklist, ChipWidth);
    if(Verbose)
        FloorplanLog << ChipWidth << " " << final_height << "\n";
    if(Verbose)
        for(auto it : Blocklist)
            FloorplanLog << it.first << " : " << it.second.x << " " << it.second.y << " " << it.second.width << " " << it.second.height << " " << it.second.rotate << "\n";
    for(auto it : Blocklist)
        outputfile << it.first << " : " << it.second.x << " " << it.second.y << " " << it.second.rotate << "\n";
}
int main(int argc, char** argv)
{
    string filename = argv[1];
    bool verbose = argv[2];
    ShelfFloorplan shelfloorplan(filename, verbose);
    shelfloorplan.run();
    return 0;
}