from threading import Event

from consumer import Consumer
from producer import producer_factory
from buffer import Buffer

# Task conditions
BUFFER_SIZE = 9
A_CAN_WRITE = 5
ANY_CAN_READ = 3

buffer = Buffer(BUFFER_SIZE, A_CAN_WRITE, ANY_CAN_READ)
thread_family = list()
event = Event()

# init producers & consumers
thread_family.append(producer_factory('A', buffer, event))
thread_family.append(producer_factory('B', buffer, event))

thread_family.append(Consumer(1, buffer, event))
thread_family.append(Consumer(2, buffer, event))
for thready in thread_family:
    thready.start()

# time to sleep
input("")
event.set()
for thready in thread_family:
    thready.join()





