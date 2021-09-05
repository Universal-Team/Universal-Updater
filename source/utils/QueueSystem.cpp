#include "QueueSystem.hpp"

std::deque<QueueEntry> QueueSystem::Queue;

void QueueSystem::Draw() {
	if (Queue.size() > 0) Queue[0].Draw();
}


void QueueSystem::Handler() {
	while(Queue.size()) {
		Queue[0].Handler();
		Queue.pop_front();
	}
}

void QueueSystem::Remove(size_t Idx) {
	if(Idx < Queue.size()) {
		if(Idx == 0) {
			Queue[0].Cancel();
		} else {
			Queue.erase(Queue.begin() + Idx);
		}
	}
}
