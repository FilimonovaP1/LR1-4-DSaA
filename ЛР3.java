
import java.util.*;


// 1) БИНАРНАЯ КУЧА (Min Binary Heap)

class BinaryHeap {
    private List<Integer> heap = new ArrayList<>();

    // Получение индексов родителя и детей
    private int parent(int i) { return (i - 1) / 2; }
    private int left(int i) { return 2 * i + 1; }
    private int right(int i) { return 2 * i + 2; }

    // Просеивание вверх (после вставки)
    private void heapifyUp(int i) {
        while (i > 0 && heap.get(parent(i)) > heap.get(i)) {
            Collections.swap(heap, i, parent(i));
            i = parent(i);
        }
    }

    // Просеивание вниз (после удаления минимума)
    private void heapifyDown(int i) {
        int n = heap.size();
        while (true) {
            int l = left(i), r = right(i), smallest = i;
            if (l < n && heap.get(l) < heap.get(smallest)) smallest = l;
            if (r < n && heap.get(r) < heap.get(smallest)) smallest = r;
            if (smallest == i) break;
            Collections.swap(heap, i, smallest);
            i = smallest;
        }
    }

    // Вставка значения
    public void insert(int val) {
        heap.add(val);
        heapifyUp(heap.size() - 1);
    }

    // Извлечение минимального элемента
    public Integer extractMin() {
        if (heap.isEmpty()) return null;
        int root = heap.get(0);
        heap.set(0, heap.get(heap.size() - 1));
        heap.remove(heap.size() - 1);
        if (!heap.isEmpty()) heapifyDown(0);
        return root;
    }

    public boolean isEmpty() { return heap.isEmpty(); }
}

// 2) БИНОМИАЛЬНАЯ КУЧА (Binomial Heap)

class BinomialHeap {
    // Узел биномиальной кучи
    static class Node {
        int key;
        int degree;
        Node parent, child, sibling;

        Node(int k) {
            key = k;
            degree = 0;
        }
    }

    private Node head = null;

    // Слияние двух корневых списков по возрастанию степени
    private Node mergeRootLists(Node h1, Node h2) {
        if (h1 == null) return h2;
        if (h2 == null) return h1;

        Node head;
        Node tail;

        if (h1.degree <= h2.degree) {
            head = tail = h1;
            h1 = h1.sibling;
        } else {
            head = tail = h2;
            h2 = h2.sibling;
        }

        while (h1 != null && h2 != null) {
            if (h1.degree <= h2.degree) {
                tail.sibling = h1;
                tail = h1;
                h1 = h1.sibling;
            } else {
                tail.sibling = h2;
                tail = h2;
                h2 = h2.sibling;
            }
        }
        tail.sibling = (h1 != null ? h1 : h2);
        return head;
    }

    // Делает одно дерево дочерним узлом другого
    private void link(Node a, Node b) {
        b.parent = a;
        b.sibling = a.child;
        a.child = b;
        a.degree++;
    }

    // Основное слияние куч
    public void merge(BinomialHeap other) {
        this.head = mergeRootLists(this.head, other.head);
        other.head = null;

        if (this.head == null) return;

        Node prev = null;
        Node curr = this.head;
        Node next = curr.sibling;

        while (next != null) {
            if (curr.degree != next.degree ||
                (next.sibling != null && next.sibling.degree == curr.degree)) {
                prev = curr;
                curr = next;
            } else {
                if (curr.key <= next.key) {
                    curr.sibling = next.sibling;
                    link(curr, next);
                } else {
                    if (prev == null) this.head = next;
                    else prev.sibling = next;
                    link(next, curr);
                    curr = next;
                }
            }
            next = curr.sibling;
        }
    }

    // Вставка ключа (как куча из одного элемента)
    public void insert(int key) {
        BinomialHeap temp = new BinomialHeap();
        temp.head = new Node(key);
        merge(temp);
    }

    // Поиск минимума
    private Node findMinNode() {
        if (head == null) return null;
        Node y = head;
        Node x = head;
        int min = x.key;
        while (x != null) {
            if (x.key < min) {
                min = x.key;
                y = x;
            }
            x = x.sibling;
        }
        return y;
    }

    // Извлечение минимума
    public Integer extractMin() {
        if (head == null) return null;

        Node minNode = findMinNode();
        Node prevMin = null;
        Node curr = head;

        // Находим предыдущий узел минимума
        while (curr != minNode) {
            prevMin = curr;
            curr = curr.sibling;
        }

        // Удаляем minNode из списка корней
        if (prevMin == null) head = minNode.sibling;
        else prevMin.sibling = minNode.sibling;

        // Разворачиваем список детей
        Node child = minNode.child;
        Node rev = null;
        while (child != null) {
            Node next = child.sibling;
            child.sibling = rev;
            child.parent = null;
            rev = child;
            child = next;
        }

        // Создаём временную кучу
        BinomialHeap temp = new BinomialHeap();
        temp.head = rev;

        // Сливаем обратно
        merge(temp);

        return minNode.key;
    }

    public boolean isEmpty() { return head == null; }
}

// 3) КУЧА ФИБОНАЧЧИ (Fibonacci Heap)

class FibonacciHeap {
    static class Node {
        int key;
        int degree = 0;
        boolean mark = false;
        Node parent = null, child = null, left = this, right = this;

        Node(int k) { key = k; }
    }

    private Node min = null;
    private int n = 0;

    // Добавить узел в корневой список
    private void addToRootList(Node x) {
        if (min == null) {
            min = x;
        } else {
            x.right = min.right;
            x.left = min;
            min.right.left = x;
            min.right = x;
            if (x.key < min.key) min = x;
        }
    }

    // Удалить узел из списка
    private void removeFromList(Node x) {
        x.left.right = x.right;
        x.right.left = x.left;
    }

    // Сделать y дочерним узлом x
    private void link(Node y, Node x) {
        removeFromList(y);
        y.parent = x;
        y.mark = false;

        if (x.child == null) {
            x.child = y;
            y.left = y.right = y;
        } else {
            y.right = x.child.right;
            y.left = x.child;
            x.child.right.left = y;
            x.child.right = y;
        }

        x.degree++;
    }

    // Консолидация: объединение деревьев одинаковой степени
    private void consolidate() {
        int D = (int)(Math.log(n) / Math.log(2)) + 5;
        Node[] A = new Node[D];

        List<Node> roots = new ArrayList<>();
        if (min != null) {
            Node cur = min;
            do {
                roots.add(cur);
                cur = cur.right;
            } while (cur != min);
        }

        for (Node w : roots) {
            Node x = w;
            int d = x.degree;
            while (A[d] != null) {
                Node y = A[d];
                if (x.key > y.key) {
                    Node t = x; x = y; y = t;
                }
                link(y, x);
                A[d] = null;
                d++;
            }
            A[d] = x;
        }

        min = null;
        for (Node a : A) {
            if (a == null) continue;
            a.left = a.right = a;
            if (min == null) min = a;
            else {
                a.right = min.right;
                a.left = min;
                min.right.left = a;
                min.right = a;
                if (a.key < min.key) min = a;
            }
        }
    }

    // Вставка
    public void insert(int key) {
        Node x = new Node(key);
        addToRootList(x);
        n++;
    }

    // Извлечение минимума
    public Integer extractMin() {
        Node z = min;
        if (z == null) return null;

        if (z.child != null) {
            List<Node> children = new ArrayList<>();
            Node c = z.child;
            do {
                children.add(c);
                c = c.right;
            } while (c != z.child);

            for (Node x : children) {
                removeFromList(x);
                addToRootList(x);
                x.parent = null;
            }
        }

        removeFromList(z);

        if (z == z.right) {
            min = null;
        } else {
            min = z.right;
            consolidate();
        }

        n--;
        return z.key;
    }

    public boolean isEmpty() { return min == null; }
}

// 4) ХЕШ-ТАБЛИЦА (цепочки)

class HashTable<K, V> {
    private static class Entry<K,V> {
        K key; V value;
        Entry(K k, V v) { key = k; value = v; }
    }

    private List<LinkedList<Entry<K,V>>> table;
    private int capacity;

    public HashTable(int cap) {
        capacity = cap;
        table = new ArrayList<>(cap);
        for (int i = 0; i < cap; i++)
            table.add(new LinkedList<>());
    }

    private int index(K key) {
        return Math.abs(key.hashCode()) % capacity;
    }

    public void put(K key, V value) {
        int idx = index(key);
        for (Entry<K,V> e : table.get(idx)) {
            if (e.key.equals(key)) {
                e.value = value;
                return;
            }
        }
        table.get(idx).add(new Entry<>(key, value));
    }

    public V get(K key) {
        int idx = index(key);
        for (Entry<K,V> e : table.get(idx)) {
            if (e.key.equals(key)) return e.value;
        }
        return null;
    }

    public boolean remove(K key) {
        int idx = index(key);
        Iterator<Entry<K,V>> it = table.get(idx).iterator();
        while (it.hasNext()) {
            if (it.next().key.equals(key)) {
                it.remove();
                return true;
            }
        }
        return false;
    }
}
