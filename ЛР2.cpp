// file: data_structures.cpp
#include <bits/stdc++.h>
using namespace std;

/*
1) Мультисписок (многоуровневый связный список)
Node имеет next и child; функция flatten превращает в одноуровневый список (DFS).
*/
struct MNode {
    int val;
    MNode* next;
    MNode* child;
    MNode(int v): val(v), next(nullptr), child(nullptr) {}
};

MNode* flatten(MNode* head) {
    if(!head) return head;
    MNode* cur = head;
    stack<MNode*> st;
    while(cur) {
        if(cur->child) {
            if(cur->next) st.push(cur->next);
            cur->next = cur->child;
            cur->child = nullptr;
        }
        if(!cur->next && !st.empty()) {
            cur->next = st.top(); st.pop();
        }
        cur = cur->next;
    }
    return head;
}

// helper to print
void printList(MNode* head) {
    while(head) {
        cout << head->val << " ";
        head = head->next;
    }
    cout << "\n";
}

/*
2) Deque, Queue, Priority Queue (primitive) and Priority Queue (custom struct)
*/
void cpp_stl_examples() {
    // Deque (двусторонняя)
    deque<int> dq;
    dq.push_back(2);
    dq.push_front(1);
    dq.push_back(3);
    cout << "Deque front: " << dq.front() << ", back: " << dq.back() << "\n";
    dq.pop_front(); // remove 1

    // Queue (FIFO)
    queue<int> q;
    q.push(10);
    q.push(20);
    cout << "Queue front: " << q.front() << "\n";
    q.pop();

    // Priority queue (max-heap, primitive)
    priority_queue<int> pqmax;
    pqmax.push(5); pqmax.push(1); pqmax.push(7);
    cout << "PQ max top: " << pqmax.top() << "\n"; // 7

    // Priority queue (min-heap) using greater<>
    priority_queue<int, vector<int>, greater<int>> pqmin;
    pqmin.push(5); pqmin.push(1); pqmin.push(7);
    cout << "PQ min top: " << pqmin.top() << "\n"; // 1

    // Priority queue with custom struct
    struct Task {
        int priority;
        string name;
    };
    struct Cmp {
        bool operator()(Task const& a, Task const& b) const {
            return a.priority < b.priority; // bigger priority first (max-heap)
        }
    };
    priority_queue<Task, vector<Task>, Cmp> task_pq;
    task_pq.push({2, "low"});
    task_pq.push({10, "high"});
    cout << "Top task: " << task_pq.top().name << " (p=" << task_pq.top().priority << ")\n";
}

int main() {
    // --- Демонстрация мультисписка ---
    MNode* n1 = new MNode(1);
    MNode* n2 = new MNode(2);
    MNode* n3 = new MNode(3);
    MNode* n4 = new MNode(4);
    MNode* n5 = new MNode(5);

    // level 0: 1 -> 2 -> 3
    n1->next = n2; n2->next = n3;
    // child of 2: 4 -> 5
    n2->child = n4; n4->next = n5;

    cout << "Before flatten: ";
    // print only top-level
    MNode* tmp = n1;
    while(tmp) { cout << tmp->val << " "; tmp = tmp->next; }
    cout << "\n";

    MNode* head = flatten(n1);
    cout << "After flatten: "; printList(head);

    // --- STL examples ---
    cpp_stl_examples();
    return 0;
}
