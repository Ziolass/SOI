from threading import Lock, Condition


class Monitor:
    def __init__(self):
        lock = Lock()
        self.full = Condition(lock)
        self.empty = Condition(lock)
        self.canRead = Condition(lock)
        self.A_canWrite = Condition(lock)
        self.lock = lock
        self.enter = lock.acquire
        self.leave = lock.release

