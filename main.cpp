//
//  main.cpp
//  K1
//
//  Created by Aldas Pakėnas on 2021-10-20.
//

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>

using namespace std;

bool finished = false;

class Monitor{
private:
    mutex m;
    condition_variable cv;
    string data;

public:
    Monitor() {
        data = "*";
    }
    
    bool Last3A() {
        if (data.size() >= 4){
            int counter = 0;
            for (long i = data.size() - 1; counter < 3; i--){
                if(data[i] != 'A')
                    return false;
                counter++;
            }
            return true;
        }
        return false;
    }
    
    bool addLetter(char c) {
        bool result = false;
        
        {
            unique_lock<mutex> Lock(m);
            while (!Last3A() && c != 'A' && !finished)
                cv.wait(Lock);
            
            if(finished)
                return false;
            
            data += c;
            result = true;
        }
        cv.notify_all();
        return result;
    }
    
    string getData(){
        return data;
    }
};

void Add(Monitor& workMonitor, char c){
    int counter = 0;
    while(!finished){
        if(workMonitor.addLetter(c))
            counter++;
        if(counter >= 15)
            finished = true;
    }
}

int main(int argc, const char * argv[]) {
    
    Monitor workMonitor;
    
    vector<thread> threads;
    threads.reserve(3);
    
    for (int i = 0; i < 3; i++)
        threads.emplace_back(Add, ref(workMonitor), 'A' + i);
    
    while(!finished)
        cout << workMonitor.getData() << endl;
    
    for (auto& thread : threads){
        if(thread.joinable())
            thread.join();
    }
    
    cout << workMonitor.getData() << endl;
    
    return 0;
}
