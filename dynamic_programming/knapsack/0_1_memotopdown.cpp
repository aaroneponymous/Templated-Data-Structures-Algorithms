#include <iostream>
#include <vector>
#include <chrono>
#include <sstream>
#include <fstream>



template <typename T, typename V>
T topdown_knapsack(const std::vector<T>& weight, const std::vector<V>& val,
                   T capacity, int no_items, std::vector<std::vector<V>>& dp_array)
{
    // Ensure dp_array is of the correct size. Only initialize if not already done.
    if (dp_array.empty()) {
        // Corrected the inner vector type
        dp_array.resize(no_items + 1, std::vector<V>(capacity + 1, -1));
    }

    // Base Case - return 0 value
    if (no_items == 0 || capacity == 0) return 0;

    // Correct type casting of capacity to int for vector indexing
    int cap = static_cast<int>(capacity);

    // Capacity (Weight) 0 is also included
    if (dp_array[no_items][cap] != -1) return  dp_array[no_items][capacity];

    if (weight[no_items - 1] <= capacity)
    {
        return dp_array[no_items][cap] = std::max(val[no_items-1] + topdown_knapsack(weight, val, 
                    capacity - weight[no_items-1], no_items - 1, dp_array),
                    topdown_knapsack(weight, val, capacity, no_items - 1, dp_array));
    
    }
    else
    {
        return dp_array[no_items][cap] = topdown_knapsack(weight, val, capacity, no_items - 1, dp_array);
    }
}





int main(int argc, char* argv[])
{

	if (argc != 2)
	{
		std::cerr << "Provide Expected Arguments: " << argv[0] << " " 
				  << "<input_file" << std::endl;
		return 1;

	}

	std::string file = argv[1];
	std::ifstream input_file(file);

	if (!input_file.is_open())
	{
		std::cerr << "Error: Could not open file " << file << std::endl;
		return 1;
	}

	int n; // number of items in the knapsack
	int c; // capacity of the sack
	input_file >> n >> c;
    std::vector<int> weights;
    std::vector<int> values;
    // Initialize DP Array
    std::vector<std::vector<int>> dp_array;

	std::string line;
	while (getline(input_file, line))
	{
		if (line.empty())
		{
			continue;
		}
		
		std::stringstream line_stream(line);
		int weight, value;
		line_stream >> weight >> value;
		weights.push_back(weight);
		values.push_back(value);
	}

	
	using std::chrono::high_resolution_clock;
	using std::chrono::duration;
	auto start = high_resolution_clock::now();

	int highest = topdown_knapsack(weights, values, c, n, 
                                   dp_array);
	
	auto end = high_resolution_clock::now();
	duration<double, std::milli> time = end - start;
	
	std::cout << "The maximum value is " << highest << "." << std::endl;
	std::cout<<"Duration: "<<time.count() <<" miliseconds."<< std::endl;
}