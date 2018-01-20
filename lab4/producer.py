from threading import Thread
from time import sleep
import random


class Producer(Thread):

    def __init__(self, queue, event):
        Thread.__init__(self)
        self.queue = queue
        self.event = event

    def run(self):
        raise NotImplementedError

    def push(self):
        raise NotImplementedError

    @staticmethod
    def rand_sleep():
        sleep(1.0 / random.Random().randint(1, 2))


class ProducerB(Producer):

    def __init__(self, queue, event):
        Producer.__init__(self, queue, event)

    def run(self):
        while True:
            Producer.rand_sleep()
            self.push()

    def push(self):
        self.queue.push('B')


class ProducerA(Producer):

    def __init__(self, queue, event):
        Producer.__init__(self, queue, event)

    def run(self):
        while not self.event.is_set():
            Producer.rand_sleep()
            self.push()

    def push(self):
        self.queue.push('A')


def producer_factory(producer_type, queue, event):
    if producer_type == 'A':
        return ProducerA(queue, event)
    else:
        return ProducerB(queue, event)