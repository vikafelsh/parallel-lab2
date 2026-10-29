//1.2.1

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>

using namespace std;

queue<int> dataQueue;
mutex m;
bool dataReady = false;

bool IsPrime(int number) {
    if (number < 2) return false;

    for (int i = 2; i <= number / i; i++) {
        if (number % i == 0) {
            return false;
        }
    }

    return true;
}

void DataPreparation() {
    int number;

    cout << "Enter integers (0 to finish): " << endl;

    while (cin >> number && number != 0) {
        lock_guard<mutex> lock(m);
        dataQueue.push(number);
    }

    {
        lock_guard<mutex> lock(m);
        dataReady = true;
    }
}

void DataProcessing() {
    unique_lock<mutex> lock(m);

    while (!dataReady) {
        lock.unlock();
        this_thread::sleep_for(chrono::milliseconds(100));
        lock.lock();
    }

    cout << "Prime numbers: ";

    while (!dataQueue.empty()) {
        int number = dataQueue.front();
        dataQueue.pop();

        if (IsPrime(number)) {
            cout << number << " ";
        }
    }

    cout << endl;
}

int main() {
    thread preparation(DataPreparation);
    thread processing(DataProcessing);

    preparation.detach();
    processing.join();

    return 0;
}

//1.2.2

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

mutex m;
condition_variable cv;
int value = 0;

void Waits(int id) {
    unique_lock<mutex> lock(m);

    cout << "Thread " << id << " is waiting..." << endl;

    cv.wait(lock, [] {
        return value == 1;
        });

    cout << "Thread " << id << " finished waiting." << endl;
}

void Awake() {
    this_thread::sleep_for(chrono::milliseconds(100));

    {
        lock_guard<mutex> lock(m);
        cout << "Awake: First notification!" << endl;
    }

    cv.notify_all();

    this_thread::sleep_for(chrono::milliseconds(100));

    {
        lock_guard<mutex> lock(m);
        value = 1;
        cout << "Awake: Second notification!" << endl;
    }

    cv.notify_all();
}

int main() {
    thread t1(Waits, 1);
    thread t2(Waits, 2);
    thread t3(Waits, 3);

    thread t4(Awake);

    t4.join();

    t1.join();
    t2.join();
    t3.join();

    return 0;
}

//1.2.3

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

mutex m;
condition_variable cv;
int i = 0;

void Waits(int number) {
    unique_lock<mutex> lock(m);

    cv.wait(lock, [] {
        return i == 1;
        });

    cout << "Message from Thread " << number << endl;
}

void Notify() {
    this_thread::sleep_for(chrono::milliseconds(100));

    {
        lock_guard<mutex> lock(m);
        i = 1;
        cout << "Notify: Sending signal..." << endl;
    }

    cv.notify_one();
}

int main() {
    thread Thread1(Waits, 1);
    thread Thread2(Waits, 2);
    thread Thread3(Waits, 3);

    thread notifyThread(Notify);

    notifyThread.join();

    Thread1.detach();
    Thread2.detach();
    Thread3.detach();

    this_thread::sleep_for(chrono::milliseconds(100));

    return 0;
}

//1.2.4

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

using namespace std;

queue<int> dataQueue;
mutex m;
condition_variable cv;
bool inputFinished = false;

bool IsPrime(int number) {
    if (number < 2) return false;

    for (int i = 2; i <= number / i; i++) {
        if (number % i == 0) {
            return false;
        }
    }

    return true;
}

void DataPreparation() {
    int number;

    cout << "Enter integers (0 to finish): " << endl;

    while (cin >> number && number != 0) {
        {
            lock_guard<mutex> lock(m);
            dataQueue.push(number);
        }

        cv.notify_one();
    }

    {
        lock_guard<mutex> lock(m);
        inputFinished = true;
    }

    cv.notify_one();
}

void DataProcessing() {
    while (true) {
        unique_lock<mutex> lock(m);

        cv.wait(lock, [] {
            return !dataQueue.empty() || inputFinished;
            });

        if (dataQueue.empty() && inputFinished) {
            break;
        }

        int number = dataQueue.front();
        dataQueue.pop();

        lock.unlock();

        if (IsPrime(number)) {
            cout << "Prime number: " << number << endl;
        }
    }
}

int main() {
    thread preparation(DataPreparation);
    thread processing(DataProcessing);

    preparation.detach();
    processing.join();

    return 0;
}

//1.2.5


#include <iostream>
#include <future>
#include <cmath>
#include <chrono>

using namespace std;

bool IsPrime(int number) {
    if (number < 2) return false;

    for (int i = 2; i <= number / i; i++) {
        if (number % i == 0)
            return false;
    }
    return true;
}

int FindPrime(int n) {
    int count = 0;
    int number = 1;

    while (count < n) {
        number++;

        if (IsPrime(number))
            count++;
    }

    return number;
}

void Test(int n, launch mode) {
    auto result = async(mode, FindPrime, n);

    int choice;

    cout << "\nChoose function:\n";
    cout << "1 - Square root\n";
    cout << "2 - Sine\n";
    cout << "3 - Natural logarithm\n";
    cout << "Your choice: ";
    cin >> choice;

    if (choice == 1)
        cout << "sqrt(" << n << ") = " << sqrt(n) << endl;
    else if (choice == 2)
        cout << "sin(" << n << ") = " << sin(n) << endl;
    else if (choice == 3)
        cout << "ln(" << n << ") = " << log(n) << endl;
    else
        cout << "Invalid choice!" << endl;

    auto start = chrono::steady_clock::now();

    int prime = result.get();

    auto end = chrono::steady_clock::now();

    auto time = chrono::duration<double>(end - start);

    cout << "Prime number: " << prime << endl;
    cout << "Waiting time: " << time.count()
        << " seconds\n";
}

int main() {
    int n;

    cout << "Enter prime number position: ";
    cin >> n;

    if (!cin || n <= 0) {
        cout << "Invalid input!" << endl;
        return 1;
    }

    cout << "\n--- DEFERRED ---\n";
    Test(n, launch::deferred);

    cout << "\n--- ASYNC ---\n";
    Test(n, launch::async);

    return 0;
}

//1.2.6

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <deque>
#include <vector>

using namespace std;

deque<packaged_task<int(int)>> tasks;
deque<int> numbers;
vector<future<int>> results;

mutex m;
condition_variable cv;
bool finished = false;

bool IsPrime(int number) {
    if (number < 2) return false;

    for (int i = 2; i <= number / i; i++) {
        if (number % i == 0)
            return false;
    }

    return true;
}

int FindPrime(int n) {
    int count = 0;
    int number = 1;

    while (count < n) {
        number++;

        if (IsPrime(number))
            count++;
    }

    return number;
}

void Worker() {
    while (true) {
        packaged_task<int(int)> task;
        int n;

        {
            unique_lock<mutex> lock(m);

            cv.wait(lock, [] {
                return !tasks.empty() || finished;
                });

            if (tasks.empty() && finished)
                break;

            task = move(tasks.front());
            tasks.pop_front();

            n = numbers.front();
            numbers.pop_front();
        }

        task(n);
    }
}

int main() {
    thread worker(Worker);

    int n;

    cout << "Enter prime number positions (0 to stop):\n";

    while (cin >> n && n != 0) {
        if (n < 0) {
            cout << "Enter a positive number!\n";
            continue;
        }

        packaged_task<int(int)> task(FindPrime);
        results.push_back(task.get_future());

        {
            lock_guard<mutex> lock(m);
            tasks.push_back(move(task));
            numbers.push_back(n);
        }

        cv.notify_one();
    }

    {
        lock_guard<mutex> lock(m);
        finished = true;
    }

    cv.notify_one();

    worker.join();

    cout << "\nResults:\n";

    for (auto& result : results) {
        cout << result.get() << endl;
    }

    return 0;
}

//1.2.7

#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <cmath>

using namespace std;

bool IsPrime(int number) {
    if (number < 2) return false;

    for (int i = 2; i <= number / i; i++) {
        if (number % i == 0)
            return false;
    }

    return true;
}

int FindPrime(int n) {
    int count = 0;
    int number = 1;

    while (count < n) {
        number++;

        if (IsPrime(number))
            count++;
    }

    return number;
}

void Thread1(int n, promise<int> p1,
    promise<bool> signal, promise<int> p2) {

    p1.set_value(FindPrime(n));

    signal.set_value(true);

    p2.set_value(FindPrime(n * 10));
}

void Thread2(int n, future<bool> signal) {
    if (signal.get()) {
        this_thread::sleep_for(chrono::seconds(2));
        cout << "Square root of " << n << " = "
            << sqrt(n) << endl;
    }
}

int main() {
    int n;

    cout << "Enter prime number position: ";
    cin >> n;

    if (!cin || n <= 0 || n > 1000000) {
        cout << "Invalid input!" << endl;
        return 1;
    }

    promise<int> p1, p2;
    promise<bool> signal;

    future<int> f1 = p1.get_future();
    future<int> f2 = p2.get_future();
    future<bool> fSignal = signal.get_future();

    thread t1(Thread1, n, move(p1),
        move(signal), move(p2));

    thread t2(Thread2, n, move(fSignal));

    cout << "Prime number " << n << ": "
        << f1.get() << endl;

    cout << "Prime number " << n * 10 << ": "
        << f2.get() << endl;

    t1.join();
    t2.join();

    return 0;
}
