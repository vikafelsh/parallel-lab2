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
