#include "updater.h"
#include "system.h"
#include <thread>
#include <chrono>
void Updater::ProcessesUpdater(System *system)
{
    while (true)
    {
        system->Processes();
        for (auto &proc : system->processes_)
        {
            proc.Update();
        }
        system->total_processes = system->TotalProcesses();
        system->memory_Utilization = system->MemoryUtilization();
        system->memory_Shared = system->MemoryShared();
        system->memory_Swap = system->MemorySwap();
        system->memory_Buffer = system->MemoryBuffer();
        system->cpu1m = system->cpu_.Cpumean1m();
        system->cpu5m = system->cpu_.Cpumean5m();
        system->cpu_usage = system->cpu_.Utilization();

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}
