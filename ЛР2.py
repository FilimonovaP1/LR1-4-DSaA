# file: data_structures.py
from collections import deque
import queue
import heapq
from dataclasses import dataclass, field
from typing import Any

# 1) Мультисписок (simple Node with next and child) и flatten
class MNode:
    def __init__(self, val):
        self.val = val
        self.next = None
        self.child = None

def flatten(head):
    if not head: return head
    st = []
    cur = head
    while cur:
        if cur.child:
            if cur.next:
                st.append(cur.next)
            cur.next = cur.child
            cur.child = None
        if not cur.next and st:
            cur.next = st.pop()
        cur = cur.next
    return head

def print_list(head):
    while head:
        print(head.val, end=' ')
        head = head.next
    print()

# 2) deque
dq = deque()
dq.append(2)
dq.appendleft(1)
dq.append(3)
print("Deque:", dq, "front:", dq[0], "back:", dq[-1])
dq.popleft()

# 3) queue (thread-safe), FIFO
q = queue.Queue()
q.put(10)
q.put(20)
print("Queue front:", q.get())  # 10

# 4) priority queue primitive (min-heap with heapq)
ints = []
heapq.heappush(ints, 5)
heapq.heappush(ints, 1)
heapq.heappush(ints, 7)
print("Heap min:", ints[0], "pops:", [heapq.heappop(ints) for _ in range(len(ints))])

# 5) priority queue with custom objects
@dataclass(order=True)
class Task:
    priority: int
    name: str = field(compare=False)

tasks = []
heapq.heappush(tasks, Task(2, "low"))
heapq.heappush(tasks, Task(10, "high"))
top = heapq.heappop(tasks)
print("Top task:", top.name, top.priority)

# Demonstrate multi-list usage
if __name__ == "__main__":
    a = MNode(1); b = MNode(2); c = MNode(3)
    x = MNode(4); y = MNode(5)
    a.next = b; b.next = c
    b.child = x; x.next = y
    print("Before flatten (top-level):", end=' ')
    t = a
    while t:
        print(t.val, end=' ')
        t = t.next
    print()
    flatten(a)
    print("After flatten:", end=' ')
    print_list(a)
