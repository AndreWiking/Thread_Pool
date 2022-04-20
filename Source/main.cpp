#include <iostream>
#include "ThreadPool.h"

int main() {

    ThreadPool pool{2};

    std::future<int> f1 = pool.AddTask([](){
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "Hi 1\n";
        return 1;
    });
    std::future<int> f2 = pool.AddTask([](){
        std::this_thread::sleep_for(std::chrono::seconds(6));
        std::cout << "Hi 2\n";
        return 2;
    });
    std::future<int> f3 = pool.AddTask([](){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Hi 3\n";
        return 3;
    });

    int v1 = f1.get();
    std::cout << "res1 " << v1 << '\n';
    int v2 = f2.get();
    std::cout << "res2 " << v2 << '\n';
    int v3 = f3.get();
    std::cout << "res3 " << v3 << '\n';

    pool.Join();
    pool.Stop();

    return 0;
}

