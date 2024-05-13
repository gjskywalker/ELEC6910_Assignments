#include "Fiduccia_Mattheyses.h"

void Fiduccia_Mattheyses::random_seperate(vector<int> &A, vector<int> &B, vector<int> cells)
{
    random_shuffle(cells.begin(), cells.end());
    for(int i = 0; i < ceil(cells.size()/2); i++)
        A.push_back(cells[i]);
    for(int j=ceil(cells.size()/2); j<cells.size(); j++)
        B.push_back(cells[j]);

    if(verbose)
    {
        verbose_log << "Size of A:" << A.size() << " Cells in A: " << "\n";
        for(auto it : A)
            verbose_log << it << " ";
        verbose_log << "\n";
        verbose_log << "Size of B:" << B.size() << " Cells in B: " << "\n";
        for(auto it : B)
            verbose_log << it << " ";
        verbose_log << "\n"; 
    }  
}

int Fiduccia_Mattheyses::get_cutsize(vector<int> A, vector<int> B, unordered_map<int, vector<int>> nets_array, vector<int> &cut_nets)
{
    cut_nets.clear();
    int cut_size = 0;
    for(auto it : nets_array)
    {
        int tmp_a = 0;
        int tmp_b = 0;
        for(auto cell : it.second)
        {
            if(find(A.begin(), A.end(), cell) != A.end())
                tmp_a ++;
            else if(find(B.begin(), B.end(), cell) != B.end())
                tmp_b ++;
            if(tmp_a > 0 && tmp_b > 0)
            {
                cut_size ++;
                cut_nets.push_back(it.first);
                break;
            }
        }
    }
    if(verbose)
    {
        verbose_log << "cut nets num : " << cut_size << "\n";
        verbose_log << "cut nets :" << "\n";
        for(auto it : cut_nets)
            verbose_log << it << " ";
        verbose_log << "\n";
    }
    return cut_size;
}

void Fiduccia_Mattheyses::compute_init_cell_gain(vector<int> cells, vector<int> A, vector<int> B, unordered_map<int, vector<int>> nets_array, unordered_map<int, int> &A_cell_gain, unordered_map<int, int> &B_cell_gain, map<int, set<int>> &A_gain_cells, map<int, set<int>> &B_gain_cells, unordered_map<int, bool> lock_cells)
{
    for(auto it : A)
        A_cell_gain[it] = 0;
    for(auto ie : B)
        B_cell_gain[ie] = 0;
    nets_in_ab.clear();
    for(auto nets : nets_array)
    {
        vector<int> tmp_a, tmp_b;
        for(auto cells : nets.second)
        {
            if (find(A.begin(), A.end(), cells) != A.end())
                tmp_a.push_back(cells);
            else if (find(B.begin(), B.end(), cells) != B.end())
                tmp_b.push_back(cells);
        }
        nets_in_ab[nets.first].first = tmp_a;
        nets_in_ab[nets.first].second = tmp_b;
        if(tmp_a.size() == 1) {
            if(!lock_cells[tmp_a[0]])
                A_cell_gain[tmp_a[0]] ++;
        }
        else if (tmp_a.size() == 0)
        {
            for(int j : tmp_b)
            if(!lock_cells[j])
            {
                B_cell_gain[j] --;
            }
        }
        if(tmp_b.size() == 1) {
            if(!lock_cells[tmp_b[0]])
                B_cell_gain[tmp_b[0]] ++;
        }
        else if (tmp_b.size() == 0)
        {
            for(int j : tmp_a)
                if(!lock_cells[j])
                    A_cell_gain[j] --;
        }
    }

    for(auto cell : A_cell_gain)
        A_gain_cells[cell.second].insert(cell.first);
    for(auto cell : B_cell_gain)
        B_gain_cells[cell.second].insert(cell.first);

    if(verbose)
    {
        verbose_log << "The following is the gain for each cell :" << "\n";
        verbose_log << "Gain : Cells in A" << "\n";
        for(auto gain : A_gain_cells)
        {
            verbose_log << gain.first << " : ";
            for(auto it : gain.second)
                verbose_log << it << " ";
            verbose_log << "\n";
        }
        verbose_log << "Gain : Cells in B" << "\n";
        for(auto gain : B_gain_cells)
        {
            verbose_log << gain.first << " : ";
            for(auto it : gain.second)
                verbose_log << it << " ";
            verbose_log << "\n";
        }
    }
}

void Fiduccia_Mattheyses::update_cell_gain(int mv_cell, unordered_map<int, vector<int>> cells_array, unordered_map<int, bool> lock_cells, unordered_map<int, pair<vector<int>, vector<int>>> &nets_in_ab, map<int, set<int>> &A_gain_cells, unordered_map<int, int> &A_cell_gain, map<int, set<int>> &B_gain_cells, unordered_map<int, int> &B_cell_gain)
{
    for(auto net : cells_array[mv_cell])
    {   
        if(nets_in_ab[net].first.size() >2 && nets_in_ab[net].second.size() >2)
            continue;
        else 
        {
            if(find(A.begin(), A.end(), mv_cell) != A.end())
            {
                if (nets_in_ab[net].first.size() == 2)
                {
                    if(nets_in_ab[net].second.size() == 0)
                    {
                        for(auto item : nets_in_ab[net].first)
                        {
                            if(!lock_cells[item])
                            {
                                // cout << "A1111111" <<"\n";
                                A_gain_cells[A_cell_gain[item]].erase(item);
                                A_cell_gain[item] += 2;
                                A_gain_cells[A_cell_gain[item]].insert(item);
                            }
                        }
                    }
                    else if(nets_in_ab[net].second.size() == 1)
                    {
                        for(auto item : nets_in_ab[net].first)
                        {
                            if(!lock_cells[item])
                            {
                                // cout << "A1111111" <<"\n";
                                A_gain_cells[A_cell_gain[item]].erase(item);
                                A_cell_gain[item]++ ;
                                A_gain_cells[A_cell_gain[item]].insert(item);
                            }
                        }
                        for(auto item : nets_in_ab[net].second)
                        {
                            if(!lock_cells[item])
                            {
                                // cout << "A1111111" <<"\n";
                                B_gain_cells[B_cell_gain[item]].erase(item);
                                B_cell_gain[item]-- ;
                                B_gain_cells[B_cell_gain[item]].insert(item);
                            }
                        }
                    }
                    else 
                    {
                        for(auto item : nets_in_ab[net].first)
                        {
                            if(!lock_cells[item])
                            {
                                // cout << "A1111111" <<"\n";
                                A_gain_cells[A_cell_gain[item]].erase(item);
                                A_cell_gain[item]++ ;
                                A_gain_cells[A_cell_gain[item]].insert(item);
                            }
                        }
                    }
                }
                else if (nets_in_ab[net].first.size() == 1)
                {
                    if (nets_in_ab[net].second.size() == 1)
                    for(auto item : nets_in_ab[net].second)
                    {
                        if(!lock_cells[item])
                        {
                            // cout << "B1111111" <<"\n";
                            B_gain_cells[B_cell_gain[item]].erase(item);
                            B_cell_gain[item] -= 2;
                            B_gain_cells[B_cell_gain[item]].insert(item);
                        }
                    }
                    else
                    {
                         for(auto item : nets_in_ab[net].second)
                        {
                            if(!lock_cells[item])
                            {
                                // cout << "B1111111" <<"\n";
                                B_gain_cells[B_cell_gain[item]].erase(item);
                                B_cell_gain[item]--;
                                B_gain_cells[B_cell_gain[item]].insert(item);
                            }
                        }
                    }
                }
                else
                {
                    if (nets_in_ab[net].second.size() == 0)
                    {
                        for(auto item : nets_in_ab[net].first)
                        {
                            if(!lock_cells[item])
                            {
                                // cout << "B1111111" <<"\n";
                                A_gain_cells[A_cell_gain[item]].erase(item);
                                A_cell_gain[item]++;
                                A_gain_cells[A_cell_gain[item]].insert(item);
                            }
                        }
                    }
                    else if (nets_in_ab[net].second.size() == 1)
                    {
                         for(auto item : nets_in_ab[net].second)
                        {
                            if(!lock_cells[item])
                            {
                                // cout << "B1111111" <<"\n";
                                B_gain_cells[B_cell_gain[item]].erase(item);
                                B_cell_gain[item]--;
                                B_gain_cells[B_cell_gain[item]].insert(item);
                            }
                        }
                    }
                }
            }
            else if(find(B.begin(), B.end(), mv_cell) != B.end())
            {   // move cell from B to A
                if (nets_in_ab[net].second.size() == 2)
                {
                    if(nets_in_ab[net].first.size() == 0)
                    {
                        for(auto item : nets_in_ab[net].second)
                        {
                            if(!lock_cells[item])
                            {
                                // cout << "A1111111" <<"\n";
                                B_gain_cells[B_cell_gain[item]].erase(item);
                                B_cell_gain[item] += 2;
                                B_gain_cells[B_cell_gain[item]].insert(item);
                            }
                        }
                    }
                    else if (nets_in_ab[net].first.size() == 1)
                    {
                        for(auto item : nets_in_ab[net].second)
                        {
                            if(!lock_cells[item])
                            {
                                // cout << "A1111111" <<"\n";
                                B_gain_cells[B_cell_gain[item]].erase(item);
                                B_cell_gain[item]++;
                                B_gain_cells[B_cell_gain[item]].insert(item);
                            }
                        }
                        for(auto item : nets_in_ab[net].first)
                        {
                            if(!lock_cells[item])
                            {
                                // cout << "A1111111" <<"\n";
                                A_gain_cells[A_cell_gain[item]].erase(item);
                                A_cell_gain[item]--;
                                A_gain_cells[A_cell_gain[item]].insert(item);
                            }
                        }
                    }
                    else
                    {
                        for(auto item : nets_in_ab[net].second)
                        {
                            if(!lock_cells[item])
                            {
                                // cout << "A1111111" <<"\n";
                                B_gain_cells[B_cell_gain[item]].erase(item);
                                B_cell_gain[item]++ ;
                                B_gain_cells[B_cell_gain[item]].insert(item);
                            }
                        }
                    }
                }
                else if (nets_in_ab[net].second.size() == 1)
                {
                    if (nets_in_ab[net].first.size() == 1)
                    {
                        for(auto item : nets_in_ab[net].first)
                        {
                            if(!lock_cells[item])
                            {
                                // cout << "B1111111" <<"\n";
                                A_gain_cells[A_cell_gain[item]].erase(item);
                                A_cell_gain[item] -= 2;
                                A_gain_cells[A_cell_gain[item]].insert(item);
                            }
                        }
                    }
                    else
                    {
                        for(auto item : nets_in_ab[net].first)
                        {
                            if(!lock_cells[item])
                            {
                                // cout << "B1111111" <<"\n";
                                A_gain_cells[A_cell_gain[item]].erase(item);
                                A_cell_gain[item]--;
                                A_gain_cells[A_cell_gain[item]].insert(item);
                            }
                        }
                    }
                }
                else
                {
                    if (nets_in_ab[net].first.size() == 0)
                    {
                        for(auto item : nets_in_ab[net].second)
                        {
                            if(!lock_cells[item])
                            {
                                // cout << "B1111111" <<"\n";
                                B_gain_cells[B_cell_gain[item]].erase(item);
                                B_cell_gain[item]++;
                                B_gain_cells[B_cell_gain[item]].insert(item);
                            }
                        }
                    }
                    else if(nets_in_ab[net].first.size() == 1)
                    {
                         for(auto item : nets_in_ab[net].first)
                        {
                            if(!lock_cells[item])
                            {
                                // cout << "B1111111" <<"\n";
                                A_gain_cells[A_cell_gain[item]].erase(item);
                                A_cell_gain[item]--;
                                A_gain_cells[A_cell_gain[item]].insert(item);
                            }
                        }
                    }
                }
            }
        }
    }

    vector<int> tmp_gain_A, tmp_gain_B;
    for(auto it : A_gain_cells)
        if(it.second.empty())
        {
            tmp_gain_A.push_back(it.first);
            // cout << "Removed Gain in A : " << it.first << "\n";
        }
    
    for(auto ie : B_gain_cells)
        if(ie.second.empty())
        {
            tmp_gain_B.push_back(ie.first);
            // cout << "Removed Gain in B : " << ie.first << "\n";
        }
    
    for(auto ga : tmp_gain_A)
        A_gain_cells.erase(ga);
        // cout << "Remove Gain in A :" << ga << "\n"; 

    for(auto gb : tmp_gain_B)
        B_gain_cells.erase(gb);
        // cout << "Remove Gain in B :" << gb << "\n";
}
void Fiduccia_Mattheyses::update_nets_in_ab(vector<int> A, vector<int> B)
{
    for(auto nets : nets_array)
    {
        vector<int> tmp_a, tmp_b;
        for(auto cells : nets.second)
        {
            if (find(A.begin(), A.end(), cells) != A.end())
                tmp_a.push_back(cells);
            else if (find(B.begin(), B.end(), cells) != B.end())
                tmp_b.push_back(cells);
        }
        nets_in_ab[nets.first].first = tmp_a;
        nets_in_ab[nets.first].second = tmp_b;
    }
}

void Fiduccia_Mattheyses::move(vector<int> &A, vector<int> &B, map<int, set<int>> &A_gain_cells, unordered_map<int, int> &A_cell_gain, map<int, set<int>> &B_gain_cells, unordered_map<int, int> &B_cell_gain, unordered_map<int, vector<int>> &nets_array, unordered_map<int, vector<int>> &cells_array, unordered_map<int, bool> &lock_cells)
{   
    // Since there may be multi cells have the maximum gain, and we gonna try all these choices
    int count = 0;
    while(count < iteration_limit)
    {
        /*
            There are several cases, when the search process will stop.
            if there is no cell gain > 0, which means we achieve the idea result;
            else if the search process time above the TIME LIMIT, we will also stop and give the current best case;
            else if the search process iteration above the cycle limit, we will also stop and give the current best case;
        */
        count++;
        random_shuffle(A.begin(), A.end());
        random_shuffle(B.begin(), B.end());
        compute_init_cell_gain(cells, A, B, nets_array, A_cell_gain, B_cell_gain, A_gain_cells, B_gain_cells, lock_cells);
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0 ; i < cell_num; i++)
        {
           auto end = std::chrono::high_resolution_clock::now();
           auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
           int durationInt = static_cast<int>(duration.count());
           if(durationInt > TIME_LIMIT)
                break;
           int counter = 0;
           int mv_cell;
           if(A_gain_cells.empty())
                goto mvb2a;
           else if(B_gain_cells.empty())
                goto mva2b;
           if(A_gain_cells.rbegin()->first >= B_gain_cells.rbegin()->first)
           {
              mva2b:
            //   cout << "Move to A part " << "\n";
              if(A_gain_cells.empty())
                break;
              mv_cell = *(A_gain_cells.rbegin()->second.begin());
            //   cout << "Move Cell in A : " << mv_cell << "\n";
              lock_cells[mv_cell] = true;
              cut_num -= A_cell_gain[mv_cell];
              A_gain_cells[A_cell_gain[mv_cell]].erase(mv_cell);
              A_cell_gain.erase(mv_cell);
              update_cell_gain(mv_cell, cells_array, lock_cells, nets_in_ab, A_gain_cells, A_cell_gain, B_gain_cells, B_cell_gain);  
              A.erase(find(A.begin(),A.end(),mv_cell));
              B.push_back(mv_cell);
              update_nets_in_ab(A, B);
           }
           else
           {
              mvb2a:
            //   cout << "Move to B part " << "\n";
              if(B_gain_cells.empty())
                break;
              mv_cell = *(B_gain_cells.rbegin()->second.begin());
            //   cout << "Move Cell in B : " << mv_cell << "\n";
              lock_cells[mv_cell] = true;
              cut_num -= B_cell_gain[mv_cell];
              B_gain_cells[B_cell_gain[mv_cell]].erase(mv_cell);
              B_cell_gain.erase(mv_cell);
              update_cell_gain(mv_cell, cells_array, lock_cells, nets_in_ab, A_gain_cells, A_cell_gain, B_gain_cells, B_cell_gain);
              B.erase(find(B.begin(),B.end(),mv_cell));
              A.push_back(mv_cell);
              update_nets_in_ab(A, B);
           }
           if(abs(int(A.size() - B.size())) <= balance_constrain && abs(int(A.size() - B.size())) % 2 == balance_flag)
           {
              if(cut_num < best_cut_num)
              {
                Best_A = A;
                Best_B = B;
                best_cut_num = cut_num;
              }
           }
           if(verbose)
            {
                verbose_log << "mv cell : " << mv_cell << "\n";
                verbose_log << "Currently cut num : " << cut_num << "\n";
                verbose_log << "Currently cut num (ground truth) : " << get_cutsize(A, B, nets_array, cut_nets) << "\n";
                verbose_log << "New A :" << "\n";
                for(auto it : A)
                    verbose_log << it << " ";
                verbose_log << "\n";
                verbose_log << "New B :" << "\n";
                for(auto it : B)
                    verbose_log << it << " ";
                verbose_log << "\n";
                if(!A_gain_cells.empty()) 
                {
                    verbose_log << "New Cell Gain in A :" << "\n";
                    for(auto it : A_gain_cells)
                    {
                        verbose_log << it.first << " : ";
                        for(auto cell : it.second)
                            verbose_log << cell << " ";
                        verbose_log << "\n";
                    }
                }
                if(!B_gain_cells.empty()) 
                {
                    verbose_log << "New Cell Gain in B :" << "\n";
                    for(auto it : B_gain_cells)
                    {
                        verbose_log << it.first << " : ";
                        for(auto cell : it.second)
                            verbose_log << cell << " ";
                        verbose_log << "\n";
                    }
                }
            }
        }
        if(verbose_log)
        {
            verbose_log << "Done for "<< count <<" iterations" << "\n";
            verbose_log << "Currently the best A is : " << "\n";
            for(auto it : Best_A)
                verbose_log << it << " ";
            verbose_log << "\n";
            verbose_log << "Currently the best B is : " << "\n";
            for(auto ie : Best_B)
                verbose_log << ie << " ";
            verbose_log << "\n";
        }
        A = Best_A;
        B = Best_B;
        cut_num = get_cutsize(A, B, nets_array, cut_nets);
        
        //reset
        for(auto& it : lock_cells)
            it.second = false;
        for(auto& it : A_cell_gain)
            it.second = 0;
        for(auto& it : B_cell_gain)
            it.second = 0;
        for(auto& it : A_gain_cells)
            it.second.clear();
        for(auto& it : B_gain_cells)
            it.second.clear();
    }
    cout << "Done all iterations !" << "\n";
    cout << "Best cut num : " << cut_num << "\n";
}

void Fiduccia_Mattheyses::output_result()
{
    const char* folder_name = output_file_name.c_str();
    int result = mkdir(folder_name, 0777);
    vertices_assign.open(output_file_name+"/vertices_assignment.txt", ios::out);
    edge_being_cut.open(output_file_name+"/edges_being_cut.txt", ios::out);
    int balance_size = 0;
    if(A.size() >= B.size())
    {
        balance_size = A.size() - B.size();
        for(int i = 0; i < balance_size; i++)
            B.push_back(isolate_cells[i]);

    }
    else
    {
        balance_size = B.size() - A.size();
        for(int i = 0; i < balance_size; i++)
            A.push_back(isolate_cells[i]);
    }
    int tmp = isolate_cells.size() - balance_size;
    for(int i = balance_size; i < balance_size + tmp/2; i++)
        A.push_back(isolate_cells[i]);
    for(int i = balance_size + tmp/2; i < balance_size; i++)
        B.push_back(isolate_cells[i]);
    for(int i=1; i<=cell_num; i++)
        if(find(A.begin(), A.end(), i) != A.end())
            vertices_assign << i << " 1" << "\n";
        else
            vertices_assign << i << " 0" << "\n";
    for(auto edge : cut_nets)
        edge_being_cut << edge << "\n";
}

void Fiduccia_Mattheyses::run()
{
    random_seperate(A, B, cells); 
    cut_num = get_cutsize(A, B, nets_array, cut_nets);
    move(A, B, A_gain_cells, A_cell_gain, B_gain_cells, B_cell_gain, nets_array, cells_array, lock_cells);
    output_result();
}

int main(int argc, char** argv)
{
    auto start = std::chrono::high_resolution_clock::now();
    string filename;
    filename = argv[1];
    int iteration_num = 0;
    iteration_num = stoi(argv[2]);
    int seed = 0;
    seed = stoi(argv[3]);
    int verbose;
    verbose = stoi(argv[4]);
    Fiduccia_Mattheyses FM(filename, iteration_num, seed, verbose);
    FM.run();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time Cost : " << duration.count() << " s" << std::endl;
    return 0;
}