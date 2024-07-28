#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <climits>
#include <memory>

class HierarchicalMutex {
    std::mutex m;
    unsigned long const hierarchy_level;
    unsigned long previous_hierarchy_level;
    static thread_local unsigned long this_thread_hierarchy_level;

    void check_for_hierarchy_violation() {
        if (this_thread_hierarchy_level <= hierarchy_level) {
            throw std::logic_error("Mutex hierarchy violated");
        }
    }

    void update_hierarchy_level() {
        previous_hierarchy_level = this_thread_hierarchy_level;
        this_thread_hierarchy_level = hierarchy_level;
    }

public:
    explicit HierarchicalMutex(unsigned long level) : hierarchy_level(level), previous_hierarchy_level(0) {}

    void lock() {
        check_for_hierarchy_violation();
        m.lock();
        update_hierarchy_level();
    }

    void unlock() {
        this_thread_hierarchy_level = previous_hierarchy_level;
        m.unlock();
    }

    bool try_lock() {
        check_for_hierarchy_violation();
        if (!m.try_lock()) return false;
        update_hierarchy_level();
        return true;
    }
};

thread_local unsigned long HierarchicalMutex::this_thread_hierarchy_level(ULONG_MAX);

class NQueensSolver {
    int N;
    std::vector<int> board;
    std::vector<std::unique_ptr<HierarchicalMutex>> row_mutexes;
    std::atomic<int> solutions_count;

public:
    NQueensSolver(int n) : N(n), board(n, -1), row_mutexes(n), solutions_count(0) {
        for (int i = 0; i < n; ++i) {
            row_mutexes[i] = std::make_unique<HierarchicalMutex>(i);
        }
    }

    void solve() {
        std::vector<std::thread> threads;
        for (int i = 0; i < N; ++i) {
            threads.emplace_back(&NQueensSolver::place_queen, this, 0, i);
        }
        for (auto& t : threads) {
            t.join();
        }
        std::cout << "Total solutions: " << solutions_count.load() << std::endl;
    }

private:
    void place_queen(int row, int col) {
        if (row == N) {
            solutions_count++;
            return;
        }

        if (!is_safe(row, col)) return;

        std::lock_guard<HierarchicalMutex> lock(*row_mutexes[row]);
        board[row] = col;

        for (int i = 0; i < N; ++i) {
            place_queen(row + 1, i);
        }

        board[row] = -1;
    }

    bool is_safe(int row, int col) {
        for (int i = 0; i < row; ++i) {
            if (board[i] == col || abs(board[i] - col) == abs(i - row)) {
                return false;
            }
        }
        return true;
    }
};

int main() {
    int N = 8; // Change this value to solve for different sizes of the board
    NQueensSolver solver(N);
    solver.solve();
    return 0;
}
