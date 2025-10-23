#include "base/trace.hh"
#include "venus/random_number_gen.hh"

// #include "debug/random_number_gen.hh"
#include <iostream>
namespace gem5
{
random_number_gen::random_number_gen(const random_number_genParams &params) : SimObject(params), 
nextTickEvent([this]{generateNumber();},name()),
cycle_count(0) {
    std::cout << "this is a test random number generate sequential function to simulate packet gen!\n" << std::endl;
    // DPRINTF(RandomNumberGenerator,"overwrite generatenumber by packetgen in venus\n");
}

    // 注册函数
void random_number_gen::init() {
        schedule(nextTickEvent, curTick() + randomCycleTime()); // 首次调度
    }

void random_number_gen::generateNumber() {
        int increase = randomCycleTime();
        cycle_count = cycle_count + increase;
        int randomNumber = generateRandomNumber();
        std::cout << "Cycle: " << cycle_count << ", Random Number: " << randomNumber << std::endl;


        // 重新调度下一个事件
        schedule(nextTickEvent, curTick() + increase);
    }

// private:
int random_number_gen::randomCycleTime() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, 10);
        return dist(gen);
    }

int random_number_gen::generateRandomNumber() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 100);
        return dist(gen);
    }
}