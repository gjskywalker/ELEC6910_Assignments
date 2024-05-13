#include <vector>
#include <set>
#include <math.h>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <sys/stat.h>
#include <unordered_map>
#include <algorithm>
#include <cstdlib>
#include <fstream>

using namespace std;
#define TIME_LIMIT 300

class Fiduccia_Mattheyses
{
    public:
        Fiduccia_Mattheyses(string inputfile_name, int iteration_num ,int seed, int verbose_or_not)
        {
            verbose = verbose_or_not;
            if(verbose)
                verbose_log.open("verbose_log");

            cut_num = 0;
            begin_time = time(0);
            if(seed == 0)
                srand(begin_time);
            else if(seed > 0)
                srand(seed);
            ifstream inputfile(inputfile_name);
            inputfile >> cell_num;
            net_num = cell_num;
            
            if(iteration_num)
                iteration_limit = iteration_num;
            else 
                iteration_limit = cell_num;

            best_cut_num = cell_num;
            string line;
            // To get "\n" in line 0;
            getline(inputfile, line);
            getline(inputfile, line);
            istringstream iss0(line);
            int icell;
            while(iss0 >> icell)
                isolate_cells.push_back(icell);

            int net;
            int cell;
            balance_constrain = isolate_cells.size();
            balance_flag = balance_constrain % 2;
            while(getline(inputfile, line))
            {
                istringstream iss1(line);
                iss1 >> net;
                while(iss1 >> cell)
                    nets_array[net].push_back(cell);
            }

            for(auto it : nets_array)
                for(auto cell : it.second)
                    cells_array[cell].push_back(it.first);
            
            for(int i=1; i<=cell_num; i++)
                if(find(isolate_cells.begin(), isolate_cells.end(), i) == isolate_cells.end())
                    cells.push_back(i);
            
            for(auto it : cells)
                lock_cells[it] = false;
            
            output_file_name = inputfile_name.replace(inputfile_name.find(".txt"), inputfile_name.length(),"");
        }
        ~Fiduccia_Mattheyses()
        {
            if(verbose)
                verbose_log.close();
            vertices_assign.close();
            edge_being_cut.close();

            cells.clear();
            lock_cells.clear();
            isolate_cells.clear();
            nets_array.clear();
            cells_array.clear();
            A.clear();
            B.clear();
            Best_A.clear();
            Best_B.clear();
            cut_nets.clear();
            nets_in_ab.clear();
        }
    // Basic information of the hypergraph
    bool balance_flag;
    int cell_num;
    int net_num;
    int cut_num;
    int verbose;
    int iteration_limit;
    int begin_time;
    int balance_constrain;
    int best_cut_num;
    string output_file_name;
    ofstream verbose_log, vertices_assign, edge_being_cut;
    vector<int> isolate_cells;
    vector<int> cells;
    unordered_map<int, bool> lock_cells;
    // Include net --> cells mapping info;
    unordered_map<int, vector<int>> nets_array;
    // Include cell --> nets mapping info;
    unordered_map<int, vector<int>> cells_array;

    // To stands for the bi-partition
    vector<int> A, Best_A;
    vector<int> B, Best_B;

    // Store Cut nets;
    vector<int> cut_nets;
    unordered_map<int, pair<vector<int>, vector<int>>> nets_in_ab;

    // Store Cell gain;
    unordered_map<int, int> A_cell_gain;
    map<int, set<int>> A_gain_cells;
    unordered_map<int, int> B_cell_gain;
    map<int, set<int>> B_gain_cells;

    void random_seperate(vector<int> &A, vector<int> &B, vector<int> cells);

    int get_cutsize(vector<int> A, vector<int> B, unordered_map<int, vector<int>> nets_array, vector<int> &cut_nets);

    void compute_init_cell_gain(vector<int> cells, vector<int> A, vector<int> B, unordered_map<int, vector<int>> nets_array, unordered_map<int, int> &A_cell_gain, unordered_map<int, int> &B_cell_gain, map<int, set<int>> &A_gain_cells, map<int, set<int>> &B_gain_cells, unordered_map<int, bool> lock_cells);

    void move(vector<int> &A, vector<int> &B, map<int, set<int>> &A_gain_cells, unordered_map<int, int> &A_cell_gain, map<int, set<int>> &B_gain_cells, unordered_map<int, int> &B_cell_gain, unordered_map<int, vector<int>> &nets_array, unordered_map<int, vector<int>> &cells_array, unordered_map<int, bool> &lock_cells);

    void update_cell_gain(int mv_cell, unordered_map<int, vector<int>> cells_array, unordered_map<int, bool> lock_cells, unordered_map<int, pair<vector<int>, vector<int>>> &nets_in_ab, map<int, set<int>> &A_gain_cells, unordered_map<int, int> &A_cell_gain, map<int, set<int>> &B_gain_cells, unordered_map<int, int> &B_cell_gain);

    void update_nets_in_ab(vector<int> A, vector<int> B);
    void run();

    void output_result();
};