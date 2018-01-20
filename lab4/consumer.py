from threading import Thread
from time import sleep
import random


class Consumer(Thread):

    def __init__(self, identifier, queue, event):
        Thread.__init__(self)
        self.identifier = identifier
        self.queue = queue
        self.event = event

    def run(self):
        while not self.event.is_set():
            Consumer.rand_sleep()
            self.pop()

    def pop(self):
        self.queue.pop(self.identifier)

    @staticmethod
    def rand_sleep():
        sleep(1.0 / random.Random().randint(10, 20))
