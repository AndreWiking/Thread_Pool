#include <iostream>
#include "ThreadPool/ThreadPool.h"

void Test1() {
    ThreadPool pool{2};

    std::future<int> f1 = pool.AddTask<int>([](){
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "Hi 1\n";
        return 1;
    });
    std::future<int> f2 = pool.AddTask<int>([](){
        std::this_thread::sleep_for(std::chrono::seconds(4));
        std::cout << "Hi 2\n";

        std::future<int> fc = Current()->AddTask<int>([](){
            std::cout << "child\n";
            //throw std::runtime_error("Example exp");
            throw 44; // fix int exp
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
    std::future<int> f3 = pool.AddTask<int>([](){
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



void Test2() {
    ThreadPool pool{2};

    std::future<int> f1 = pool.AddTask<int>([]()->int {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "Hi 1\n";
        return 1;
    });

    int v1 = f1.get();
    std::cout << "res1 " << v1 << '\n';

    pool.Join();
    pool.Stop();
}

int main() {

    Test1();

    return 0;
}

