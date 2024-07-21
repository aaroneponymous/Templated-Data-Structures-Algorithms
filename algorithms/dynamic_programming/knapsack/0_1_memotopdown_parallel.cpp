#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <numeric>

class ThreadPool
{
public:
	ThreadPool(size_t numThreads)
	{
		for (size_t i = 0; i < numThreads; ++i)
		{
			workers.emplace_back([this]
								 {
                while (true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->queueMutex);
                        this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });

                        if (this->stop && this->tasks.empty())
                            return;

                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
                } });
		}
	}

	template <class F, class... Args>
	auto enqueue(F &&f, Args &&...args)
		-> std::future<typename std::result_of<F(Args...)>::type>
	{
		using return_type = typename std::result_of<F(Args...)>::type;

		auto task = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...));

		std::future<return_type> res = task->get_future();
		{
			std::unique_lock<std::mutex> lock(queueMutex);

			// don't allow enqueueing after stopping the pool
			if (stop)
				throw std::runtime_error("enqueue on stopped ThreadPool");

			tasks.emplace([task]()
						  { (*task)(); });
		}
		condition.notify_one();
		return res;
	}

	~ThreadPool()
	{
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			stop = true;
		}
		condition.notify_all();
		for (std::thread &worker : workers)
			worker.join();
	}

private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;

	std::mutex queueMutex;
	std::condition_variable condition;
	bool stop = false;
};

int knapsack_chunk(int start_w, int end_w, int i, const std::vector<int> &wt, const std::vector<int> &val, std::vector<std::vector<int>> &dp)
{
	for (int w = start_w; w <= end_w; ++w)
	{
		if (wt[i - 1] <= w)
		{
			dp[i][w] = std::max(val[i - 1] + dp[i - 1][w - wt[i - 1]], dp[i - 1][w]);
		}
		else
		{
			dp[i][w] = dp[i - 1][w];
		}
	}
	return 0; // Return type needs to match the future, actual value is directly written to dp.
}

int parallel_knapsack(int n, int W, const std::vector<int> &wt, const std::vector<int> &val, ThreadPool &pool)
{
	std::vector<std::vector<int>> dp(n + 1, std::vector<int>(W + 1, 0));
	std::vector<std::future<int>> futures;

	// Initialize base cases for dp here if needed.
	for (int i = 1; i <= n; ++i)
	{
		const int chunk_size = std::max(static_cast<int>(W / std::thread::hardware_concurrency()), 1);
		for (int w = 1; w <= W; w += chunk_size)
		{
			int end_w = std::min(w + chunk_size - 1, W);
			futures.emplace_back(pool.enqueue(knapsack_chunk, w, end_w, i, std::ref(wt), std::ref(val), std::ref(dp)));
		}

		// Wait for all chunks of the current row to be completed before proceeding to the next row.
		for (auto &future : futures)
		{
			future.get(); // Ensure each chunk is completed.
		}
		futures.clear(); // Clear futures for the next iteration.
	}

	return dp[n][W];
}

// Function to read input file and extract values
bool readInputFile(const std::string& filename, int& n, int& capacity, std::vector<int>& weights, std::vector<int>& values) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open input file: " << filename << std::endl;
        return false;
    }

    // Read n and capacity
    if (!(file >> n >> capacity)) {
        std::cerr << "Error: Failed to read n and capacity from input file: " << filename << std::endl;
        return false;
    }

    // Read weights and values
    weights.resize(n);
    values.resize(n);
    for (int i = 0; i < n; ++i) {
        if (!(file >> weights[i] >> values[i])) {
            std::cerr << "Error: Failed to read weight and value for item " << i+1 << " from input file: " << filename << std::endl;
            return false;
        }
    }

    return true;
}

double average(const std::vector<double>& times) {
    double total = std::accumulate(times.begin(), times.end(), 0.0);
    return total / times.size();
}


int main() {
    std::vector<std::string> input_files = {"inputs/1.txt", "inputs/2.txt", "inputs/3.txt", "inputs/4.txt", "inputs/5.txt"};
    const std::vector<int> thread_counts = {1, 2, 4, 8, 16};
    const int repetitions = 10;

    std::ofstream parallel_output("parallel_results.txt");

    // Run parallel version
    for (const auto& file : input_files) {
        for (const auto& threads : thread_counts) {
            std::vector<double> parallel_times;
            parallel_output << "File: " << file << " Threads: " << threads << std::endl;
            for (int i = 0; i < repetitions; ++i) {
                int n, W;
                std::vector<int> weights, values;

                // Read input file
                if (!readInputFile(file, n, W, weights, values)) {
                    continue; // Skip to next input file if reading failed
                }

                ThreadPool pool(threads);
                auto start = std::chrono::high_resolution_clock::now();
                
                int highestValue = parallel_knapsack(n, W, weights, values, pool); // Use the thread pool as an argument

                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> duration = end - start;
                double time_ms = duration.count();
                parallel_times.push_back(time_ms);
                parallel_output << "Value: " << highestValue << "   Time: " << time_ms << " ms" << std::endl;
            }

            // Calculate average execution time
            double parallel_avg_time = average(parallel_times);

            // Output parallel results
            parallel_output << "Average Execution Time: " << parallel_avg_time << " ms" << std::endl << std::endl;
        }
    }

    parallel_output.close();
    return 0;
}