#include "FlatMemoryAllocator.h"
#include <iostream>

FlatMemoryAllocator::FlatMemoryAllocator()  {
}

void FlatMemoryAllocator::initialize(ConfigurationManager* configManager) {
    configManager = configManager;
    memoryBlocks.push_back({ 0, (int) configManager->getMaxOverallMemory(), true});
}

bool FlatMemoryAllocator::allocate(Process process) {
    for (auto& block : memoryBlocks) {
        if (block.isFree && block.size >= process.getMemorySize()) {
            block.isFree = false;
            processMemoryMap[process.getID()] = block.start;
            if (block.size > process.getMemorySize()) {
                memoryBlocks.push_back({ block.start + ((int) process.getMemorySize()), block.size - ((int) process.getMemorySize()), true});
            }
            block.size = process.getMemorySize();
            return true;
        }
    }
    return false; // no sufficient free block found
}

void FlatMemoryAllocator::deallocate(int pid) {
    auto it = processMemoryMap.find(pid);
    if (it != processMemoryMap.end()) {
        for (auto& block : memoryBlocks) {
            if (block.start == it->second) {
                block.isFree = true;
                processMemoryMap.erase(it);
                break;
            }
        }
    }
}

void FlatMemoryAllocator::swapOutRandomProcess() {
    if (!processMemoryMap.empty()) {
        srand(time(nullptr));
        int randIndex = rand() % processMemoryMap.size();
        auto it = processMemoryMap.begin();
        std::advance(it, randIndex);
        std::cout << "Swapping out process with PID: " << it->first << std::endl;
        deallocate(it->first);
    }
}

