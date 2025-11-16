
#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <limits>
#include <algorithm>
#include <cassert>
#include <optional>
#include <unordered_map>

using namespace std;


// 1) БИНАРНАЯ КУЧА (Min-Heap)
class BinaryHeap {
private:
    // Внутренний массив для хранения элементов кучи
    vector<int> data;

    // Индексы в куче
    inline int parent(int i) const { return (i - 1) / 2; }
    inline int left(int i) const { return 2 * i + 1; }
    inline int right(int i) const { return 2 * i + 2; }

    // Восстановление свойства кучи снизу вверх (после вставки)
    void heapifyUp(int i) {
        while (i > 0 && data[parent(i)] > data[i]) {
            swap(data[i], data[parent(i)]);
            i = parent(i);
        }
    }

    // Восстановление свойства кучи сверху вниз (после удаления корня)
    void heapifyDown(int i) {
        int n = (int)data.size();
        while (true) {
            int l = left(i), r = right(i), smallest = i;
            if (l < n && data[l] < data[smallest]) smallest = l;
            if (r < n && data[r] < data[smallest]) smallest = r;
            if (smallest == i) break;
            swap(data[i], data[smallest]);
            i = smallest;
        }
    }

public:
    BinaryHeap() = default;

    // Вставка значения
    void insert(int val) {
        data.push_back(val);
        heapifyUp((int)data.size() - 1);
    }

    // Извлечение минимального элемента (корня). Возвращает optional<int>.
    optional<int> extractMin() {
        if (data.empty()) return nullopt;
        int res = data[0];
        data[0] = data.back();
        data.pop_back();
        if (!data.empty()) heapifyDown(0);
        return res;
    }

    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }
};

// 2) БИНОМИАЛЬНАЯ КУЧА (Binomial Heap, min-heap)


struct BinNode {
    int key;
    int degree;            // степень (количество детей)
    BinNode* parent;
    BinNode* child;        // указатель на список детей (связан как однонаправленный sibling)
    BinNode* sibling;      // указатель на следующего брата в списке корней/детей

    BinNode(int k)
        : key(k), degree(0), parent(nullptr), child(nullptr), sibling(nullptr) {}
};

// Слияние двух биномиальных деревьев той же степени: возвращает корень результата
static BinNode* binomialLink(BinNode* a, BinNode* b) {
    // Пусть a.key <= b.key — делаем b дочерним у a
    b->parent = a;
    b->sibling = a->child;
    a->child = b;
    a->degree += 1;
    return a;
}

class BinomialHeap {
private:
    // Голова списка корней (сортированного по неубыванию степени)
    BinNode* head = nullptr;

    // Слияние двух списков корней (по возрастанию degree) — вспомогательная часть merge
    static BinNode* mergeRootLists(BinNode* h1, BinNode* h2) {
        if (!h1) return h2;
        if (!h2) return h1;
        BinNode* head = nullptr;
        BinNode* tail = nullptr;

        if (h1->degree <= h2->degree) {
            head = tail = h1; h1 = h1->sibling;
        } else {
            head = tail = h2; h2 = h2->sibling;
        }
        while (h1 && h2) {
            if (h1->degree <= h2->degree) {
                tail->sibling = h1; tail = h1; h1 = h1->sibling;
            } else {
                tail->sibling = h2; tail = h2; h2 = h2->sibling;
            }
        }
        tail->sibling = h1 ? h1 : h2;
        return head;
    }

public:
    BinomialHeap() = default;

    // Слияние (merge) с другой кучей — объединяет списки корней и корректно связывает деревья
    void merge(BinomialHeap& other) {
        BinNode* newHead = mergeRootLists(this->head, other.head);
        this->head = nullptr;
        other.head = nullptr;
        if (!newHead) return;

        BinNode* prev = nullptr;
        BinNode* curr = newHead;
        BinNode* next = curr->sibling;

        while (next) {
            if (curr->degree != next->degree ||
                (next->sibling && next->sibling->degree == curr->degree)) {
                // просто сдвигаем окно
                prev = curr;
                curr = next;
            } else {
                // curr.degree == next.degree и либо next->sibling.degree != curr.degree
                if (curr->key <= next->key) {
                    // присоединяем next к curr
                    curr->sibling = next->sibling;
                    binomialLink(curr, next);
                } else {
                    // присоединяем curr к next
                    if (prev) prev->sibling = next;
                    else newHead = next;
                    binomialLink(next, curr);
                    curr = next;
                }
            }
            next = curr->sibling;
        }

        this->head = newHead;
    }

    // Вставка одного ключа: создаём временную кучу из одного узла и сливаем
    void insert(int key) {
        BinomialHeap temp;
        temp.head = new BinNode(key);
        merge(temp);
    }

    // Поиск минимального ключа (без удаления)
    optional<int> getMin() const {
        if (!head) return nullopt;
        BinNode* cur = head;
        BinNode* best = cur;
        while (cur) {
            if (cur->key < best->key) best = cur;
            cur = cur->sibling;
        }
        return best ? optional<int>(best->key) : nullopt;
    }

    // Извлечение минимума: находим корень с минимальным ключом, удаляем его из корневого списка,
    // создаём новую кучу из его детей (обратный порядок) и сливаем.
    optional<int> extractMin() {
        if (!head) return nullopt;
        // найти минимальный корень и его предка
        BinNode* prevMin = nullptr;
        BinNode* minNode = head;
        BinNode* prev = nullptr;
        BinNode* cur = head;
        int minKey = cur->key;
        while (cur) {
            if (cur->key < minKey) {
                minKey = cur->key;
                prevMin = prev;
                minNode = cur;
            }
            prev = cur;
            cur = cur->sibling;
        }

        // удаляем minNode из списка корней
        if (!prevMin) head = minNode->sibling;
        else prevMin->sibling = minNode->sibling;

        // переворачиваем список детей minNode и формируем новую кучу
        BinNode* child = minNode->child;
        BinNode* rev = nullptr;
        while (child) {
            BinNode* next = child->sibling;
            child->sibling = rev;
            child->parent = nullptr;
            rev = child;
            child = next;
        }

        BinomialHeap temp;
        temp.head = rev;
        // освобождение minNode (для простоты: delete). В реальном коде стоит аккуратно всё деаллочить.
        delete minNode;

        // слияние текущей кучи и temp
        merge(temp);

        return optional<int>(minKey);
    }

    bool empty() const { return head == nullptr; }
};


// 3) КУЧА ФИБОНАЧИ (Fibonacci Heap, min-heap)


struct FibNode {
    int key;
    int degree;
    bool mark;
    FibNode* parent;
    FibNode* child;
    FibNode* left;
    FibNode* right;

    FibNode(int k)
        : key(k), degree(0), mark(false), parent(nullptr), child(nullptr) {
        left = right = this;
    }
};

class FibonacciHeap {
private:
    FibNode* minNode = nullptr;
    size_t n = 0;

    // Добавить узел x в корневой список (справа от minNode)
    void addToRootList(FibNode* x) {
        if (!minNode) {
            minNode = x;
            x->left = x->right = x;
        } else {
            x->right = minNode->right;
            x->left = minNode;
            minNode->right->left = x;
            minNode->right = x;
            if (x->key < minNode->key) minNode = x;
        }
    }

    // Удалить x из списка (корневого или детского)
    void removeFromList(FibNode* x) {
        x->left->right = x->right;
        x->right->left = x->left;
    }

    // Сделать y дочерним у x (используется в consolidate)
    void link(FibNode* y, FibNode* x) {
        // удалить y из корневого списка
        removeFromList(y);
        // присоединить к списку детей x
        y->parent = x;
        y->mark = false;
        if (!x->child) {
            x->child = y;
            y->left = y->right = y;
        } else {
            y->right = x->child->right;
            y->left = x->child;
            x->child->right->left = y;
            x->child->right = y;
        }
        x->degree++;
    }

    // Консолидация: объединение деревьев одинаковой степени
    void consolidate() {
        if (n == 0) return;
        // максимальная возможная степень <= floor(log2(n))
        size_t D = (size_t)(std::log2(max<size_t>(1, n)) + 2);
        vector<FibNode*> A(D, nullptr);

        // Соберём все корневые узлы в список (чтобы безопасно модифицировать)
        vector<FibNode*> roots;
        if (minNode) {
            FibNode* cur = minNode;
            do {
                roots.push_back(cur);
                cur = cur->right;
            } while (cur != minNode);
        }

        for (FibNode* w : roots) {
            FibNode* x = w;
            size_t d = x->degree;
            while (d >= A.size()) {
                A.resize(A.size() * 2, nullptr);
            }
            while (A[d]) {
                FibNode* y = A[d];
                if (x->key > y->key) swap(x, y);
                link(y, x);
                A[d] = nullptr;
                d++;
                if (d >= A.size()) A.resize(A.size() * 2, nullptr);
            }
            A[d] = x;
        }

        // восстановим корневой список и найдём новый minNode
        minNode = nullptr;
        for (FibNode* node : A) {
            if (!node) continue;
            // отрежем связи влево/вправо — сделаем каждый node отдельным циклом
            node->left = node->right = node;
            if (!minNode) {
                minNode = node;
            } else {
                // добавить node в корневой список
                node->right = minNode->right;
                node->left = minNode;
                minNode->right->left = node;
                minNode->right = node;
                if (node->key < minNode->key) minNode = node;
            }
        }
    }

public:
    FibonacciHeap() = default;

    // Вставка нового ключа в кучу
    FibNode* insert(int key) {
        FibNode* node = new FibNode(key);
        addToRootList(node);
        n++;
        return node;
    }

    // Извлечение минимума: возвращаем ключ или nullopt если пусто
    optional<int> extractMin() {
        FibNode* z = minNode;
        if (!z) return nullopt;

        // Перенести всех детей z в корневой список
        if (z->child) {
            // Соберём детей в вектор чтобы безопасно итерировать
            vector<FibNode*> children;
            FibNode* c = z->child;
            do {
                children.push_back(c);
                c = c->right;
            } while (c != z->child);

            for (FibNode* x : children) {
                // удаляем связь x <-> siblings (в рамках child list)
                removeFromList(x);
                // добавляем x в корневой список
                addToRootList(x);
                x->parent = nullptr;
            }
            z->child = nullptr;
        }

        // Удаляем z из корневого списка
        removeFromList(z);
        if (z == z->right) {
            minNode = nullptr;
        } else {
            minNode = z->right;
            consolidate();
        }

        int ret = z->key;
        delete z; // удалить узел (в реальном коде можно хранить и переиспользовать)
        n--;
        return optional<int>(ret);
    }

    bool empty() const { return minNode == nullptr; }
    size_t size() const { return n; }
};

// 4) ХЕШ-ТАБЛИЦА (цепочки) — простая реализация

template<typename K, typename V>
class HashTable {
private:
    vector<list<pair<K,V>>> table;
    size_t capacity;

    size_t indexFor(const K& key) const {
        // Используем std::hash и берем мод capacity
        return std::hash<K>{}(key) % capacity;
    }

public:
    explicit HashTable(size_t cap = 16) : table(cap), capacity(cap) {}

    // Вставка или обновление пары (key, value)
    void put(const K& key, const V& value) {
        size_t idx = indexFor(key);
        for (auto &p : table[idx]) {
            if (p.first == key) { p.second = value; return; }
        }
        table[idx].emplace_back(key, value);
    }

    // Получение значения по ключу, optional (nullopt если нет)
    optional<V> get(const K& key) const {
        size_t idx = indexFor(key);
        for (const auto &p : table[idx]) {
            if (p.first == key) return optional<V>(p.second);
        }
        return nullopt;
    }

    // Удаление по ключу, возвращает true если удалили
    bool remove(const K& key) {
        size_t idx = indexFor(key);
        auto &lst = table[idx];
        for (auto it = lst.begin(); it != lst.end(); ++it) {
            if (it->first == key) {
                lst.erase(it);
                return true;
            }
        }
        return false;
    }
};

