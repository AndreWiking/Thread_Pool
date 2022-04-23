#include <iostream>
#include "ThreadPool/ThreadPool.h"

int sum(int a, int b) {
    return a + b;
}

void Test() {
    ThreadPool pool{2};

    std::future<int> f1 = pool.AddTask(sum, 4, 10);
    std::future<int> f2 = pool.AddTask([](){
        std::this_thread::sleep_for(std::chrono::seconds(4));
        std::cout << "Hi 2\n";

        std::future<int> fc = Current()->AddTask([](){
            std::cout << "child\n";
            throw std::runtime_error("exp");
            return 22;
        });
        try {
            int vc = fc.get();
            std::cout << "child ret " << vc << '\n';
        } catch(const std::exception& e) {
            std::cout << "Exception from the child: " << e.what() << '\n';
        }
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
}


int main() {

    Test();

    return 0;
}

