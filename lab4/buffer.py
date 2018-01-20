from monitor import Monitor


class Buffer:

    def __init__(self, size, A_write_cond, any_can_read):
        self.size = size
        self.A_write_con = A_write_cond
        self.any_can_read = any_can_read
        self.queue = list()
        self.monitor = Monitor()

    def print_buffer_status(self):

        print(' ', 3*'='*len(self.queue))
        print(
            len(self.queue),
            ''.join('[{}]'.format(x) for x in self.queue))
        print(' ', 3*'='*len(self.queue))

    def push(self, letter):
        print("\tProducer{} wants to enter the monitor...".format(letter))
        self.monitor.enter()
        print("\tProducer{} entered the monitor.".format(letter))

        if letter == 'A':
            while len(self.queue) <= self.A_write_con:
                print(
                    "\tProducer A can't add to buffer, number of elements =< {}.".format(self.A_write_con))
                self.monitor.A_canWrite.wait()
                print(
                    "\tProducer A was notified: number of elements in buffer > {}".format(self.A_write_con)
                )

        if len(self.queue) == self.size:
            print("\tBuffer full, Producer{} is waiting...".format(letter))
            self.monitor.full.wait()
            print("\tEmpty space in buffer, consumer notifier Producer{}".format(letter))

        self.queue.append(letter)
        print("\tProducer{}: added '{}' to buffer.".format(letter, letter))
        self.print_buffer_status()
        self.monitor.empty.notify()

        if len(self.queue) > self.A_write_con:
            self.monitor.A_canWrite.notify()
        if len(self.queue) > self.any_can_read:
            self.monitor.canRead.notify()

        self.monitor.leave()
        print("\tProducer{} left the monitor.".format(letter))

    def pop(self, consumerId):
        print("Consumer{} wants to enter the monitor".format(consumerId))
        self.monitor.enter()
        print("Consumer{} entered the monitor".format(consumerId))

        while len(self.queue) <= self.any_can_read:
            print("Too few elements in the queue, Consumer{} is waiting...".format(consumerId))
            self.monitor.canRead.wait()
            print("Consumer{} was notified.".format(consumerId))

        returnable = self.queue.pop(0)
        print("Consumer{} consumed {}.".format(consumerId, returnable))
        self.print_buffer_status()
        self.monitor.full.notify()

        self.monitor.leave()
        print("Consumer{} left the monitor".format(consumerId))
        return returnable
