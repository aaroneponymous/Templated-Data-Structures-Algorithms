#include <iostream>
#include <queue>
#include <vector>
#include <memory>
#include <algorithm>

// Customer Class
class Customer {
public:
    int arrival_time;
    int service_time;

    Customer(int arrival, int service)
        : arrival_time(arrival), service_time(service) {}
};

// Event Type Enum
enum class EventType { ARRIVAL, DEPARTURE };

// Event Class
class Event {
public:
    EventType type;
    int time;
    std::shared_ptr<Customer> customer;

    Event(EventType type, int time, std::shared_ptr<Customer> customer = nullptr)
        : type(type), time(time), customer(customer) {}

    bool operator<(const Event& other) const {
        return time > other.time; // Reverse for priority queue (min-heap)
    }
};

// Simulation Class
class Simulation {
public:
    Simulation(int num_tellers)
        : num_tellers(num_tellers), current_time(0), tellers(num_tellers, nullptr) {}

    void schedule_event(const Event& event) {
        event_queue.push(event);
    }

    void process_event(const Event& event) {
        current_time = event.time;

        if (event.type == EventType::ARRIVAL) {
            process_arrival(event.customer);
        } else if (event.type == EventType::DEPARTURE) {
            process_departure(event.customer);
        }
    }

    void process_arrival(const std::shared_ptr<Customer>& customer) {
        statistics.num_customers++;
        int available_teller = find_available_teller();

        if (available_teller != -1) {
            assign_teller(available_teller, customer);
        } else {
            waiting_queue.push(customer);
            statistics.max_line_length = std::max(statistics.max_line_length, static_cast<int>(waiting_queue.size()));
        }
    }

    void process_departure(const std::shared_ptr<Customer>& customer) {
        release_teller(customer);

        if (!waiting_queue.empty()) {
            auto next_customer = waiting_queue.front();
            waiting_queue.pop();
            assign_teller(find_available_teller(), next_customer);
        }
    }

    int find_available_teller() {
        for (int i = 0; i < num_tellers; ++i) {
            if (!tellers[i]) {
                return i;
            }
        }
        return -1;
    }

    void assign_teller(int teller_index, const std::shared_ptr<Customer>& customer) {
        tellers[teller_index] = customer;
        int departure_time = current_time + customer->service_time;
        schedule_event(Event(EventType::DEPARTURE, departure_time, customer));
        statistics.total_wait_time += (current_time - customer->arrival_time);
    }

    void release_teller(const std::shared_ptr<Customer>& customer) {
        for (int i = 0; i < num_tellers; ++i) {
            if (tellers[i] == customer) {
                tellers[i] = nullptr;
                break;
            }
        }
    }

    void run_simulation(const std::vector<std::shared_ptr<Customer>>& customers) {
        for (const auto& customer : customers) {
            schedule_event(Event(EventType::ARRIVAL, customer->arrival_time, customer));
        }

        while (!event_queue.empty()) {
            auto event = event_queue.top();
            event_queue.pop();
            process_event(event);
        }

        print_statistics();
    }

    void print_statistics() {
        double avg_wait_time = static_cast<double>(statistics.total_wait_time) / statistics.num_customers;
        std::cout << "Average wait time: " << avg_wait_time << " ticks\n";
        std::cout << "Max line length: " << statistics.max_line_length << " customers\n";
    }

private:
    int num_tellers;
    int current_time;
    std::vector<std::shared_ptr<Customer>> tellers;
    std::priority_queue<Event> event_queue;
    std::queue<std::shared_ptr<Customer>> waiting_queue;

    struct Statistics {
        int total_wait_time = 0;
        int num_customers = 0;
        int max_line_length = 0;
    } statistics;
};

// Main Function
int main() {
    std::vector<std::shared_ptr<Customer>> customers = {
        std::make_shared<Customer>(0, 5),
        std::make_shared<Customer>(1, 3),
        std::make_shared<Customer>(4, 2),
        std::make_shared<Customer>(6, 1)
    };

    Simulation simulation(2);
    simulation.run_simulation(customers);

    return 0;
}
