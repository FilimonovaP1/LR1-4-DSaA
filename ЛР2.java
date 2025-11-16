// file: DataStructures.java
import java.util.*;
import java.util.concurrent.PriorityBlockingQueue;

public class DataStructures {

    // 1) Мультисписок (Node с next и child) и flatten
    static class MNode {
        int val;
        MNode next;
        MNode child;
        MNode(int v) { val = v; next = null; child = null; }
    }

    static MNode flatten(MNode head) {
        if (head == null) return head;
        Deque<MNode> stack = new ArrayDeque<>();
        MNode cur = head;
        while (cur != null) {
            if (cur.child != null) {
                if (cur.next != null) stack.push(cur.next);
                cur.next = cur.child;
                cur.child = null;
            }
            if (cur.next == null && !stack.isEmpty()) {
                cur.next = stack.pop();
            }
            cur = cur.next;
        }
        return head;
    }

    static void printList(MNode head) {
        while (head != null) {
            System.out.print(head.val + " ");
            head = head.next;
        }
        System.out.println();
    }

    public static void main(String[] args) {
        // Deque
        ArrayDeque<Integer> dq = new ArrayDeque<>();
        dq.addLast(2); dq.addFirst(1); dq.addLast(3);
        System.out.println("Deque front: " + dq.peekFirst() + ", back: " + dq.peekLast());
        dq.removeFirst();

        // Queue (FIFO) using LinkedList
        Queue<Integer> q = new LinkedList<>();
        q.add(10); q.add(20);
        System.out.println("Queue front: " + q.peek());
        q.remove();

        // PriorityQueue primitive (default in Java — min-heap)
        PriorityQueue<Integer> pqMin = new PriorityQueue<>();
        pqMin.add(5); pqMin.add(1); pqMin.add(7);
        System.out.println("PQ min top: " + pqMin.peek());

        // Max-heap using Comparator.reverseOrder()
        PriorityQueue<Integer> pqMax = new PriorityQueue<>(Comparator.reverseOrder());
        pqMax.add(5); pqMax.add(1); pqMax.add(7);
        System.out.println("PQ max top: " + pqMax.peek());

        // PriorityQueue with custom class
        class Task {
            int priority;
            String name;
            Task(int p, String n) { priority = p; name = n; }
        }
        // Comparator: larger priority first
        PriorityQueue<Task> taskPQ = new PriorityQueue<>( (a,b) -> Integer.compare(b.priority, a.priority) );
        taskPQ.add(new Task(2, "low"));
        taskPQ.add(new Task(10, "high"));
        Task top = taskPQ.peek();
        System.out.println("Top task: " + top.name + " (p=" + top.priority + ")");

        // Demo multi-list
        MNode n1 = new MNode(1);
        MNode n2 = new MNode(2);
        MNode n3 = new MNode(3);
        MNode n4 = new MNode(4);
        MNode n5 = new MNode(5);
        n1.next = n2; n2.next = n3;
        n2.child = n4; n4.next = n5;
        System.out.print("Before flatten: ");
        MNode t = n1;
        while (t != null) { System.out.print(t.val + " "); t = t.next; }
        System.out.println();
        flatten(n1);
        System.out.print("After flatten: "); printList(n1);
    }
}
