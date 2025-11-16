
# 1. БИНАРНАЯ КУЧА (Binary Heap)


class BinaryHeap:
    """Простая реализация минимальной бинарной кучи."""

    def __init__(self):
        self.data = []  # внутренний массив кучи

    def _parent(self, i): return (i - 1) // 2
    def _left(self, i): return 2 * i + 1
    def _right(self, i): return 2 * i + 2

    def _heapify_up(self, i):
        """Восстановление свойства кучи при вставке элемента вверх."""
        while i != 0 and self.data[self._parent(i)] > self.data[i]:
            # Меняем местами с родителем
            self.data[i], self.data[self._parent(i)] = self.data[self._parent(i)], self.data[i]
            i = self._parent(i)

    def _heapify_down(self, i):
        """Восстановление свойства кучи при удалении, спуская вниз."""
        smallest = i
        l, r = self._left(i), self._right(i)

        # Проверяем левый ребёнок
        if l < len(self.data) and self.data[l] < self.data[smallest]:
            smallest = l
        # Проверяем правый ребёнок
        if r < len(self.data) and self.data[r] < self.data[smallest]:
            smallest = r

        if smallest != i:
            # Меняем элементы
            self.data[i], self.data[smallest] = self.data[smallest], self.data[i]
            self._heapify_down(smallest)

    def insert(self, value):
        """Вставка нового значения."""
        self.data.append(value)
        self._heapify_up(len(self.data) - 1)

    def extract_min(self):
        """Удаление и возврат минимального элемента."""
        if not self.data:
            return None
        root = self.data[0]
        self.data[0] = self.data[-1]
        self.data.pop()
        if self.data:
            self._heapify_down(0)
        return root

# 2. БИНОМИАЛЬНАЯ КУЧА (Binomial Heap)

class BinomialNode:
    """Узел биномиальной кучи."""
    def __init__(self, key):
        self.key = key
        self.degree = 0
        self.parent = None
        self.child = None
        self.sibling = None


def merge_trees(t1, t2):
    """Слияние двух биномиальных деревьев одинаковой степени."""
    if t1.key > t2.key:
        t1, t2 = t2, t1  # t1 должен быть корнем с меньшим ключом
    t2.parent = t1
    t2.sibling = t1.child
    t1.child = t2
    t1.degree += 1
    return t1


class BinomialHeap:
    """Реализация биномиальной кучи (минимальной)."""

    def __init__(self):
        self.head = None

    def _merge_roots(self, h1, h2):
        """Слияние списков корней двух куч по степени."""
        if not h1:
            return h2
        if not h2:
            return h1

        if h1.degree <= h2.degree:
            head = h1
            h1 = h1.sibling
        else:
            head = h2
            h2 = h2.sibling

        tail = head

        while h1 and h2:
            if h1.degree <= h2.degree:
                tail.sibling = h1
                h1 = h1.sibling
            else:
                tail.sibling = h2
                h2 = h2.sibling
            tail = tail.sibling

        tail.sibling = h1 if h1 else h2
        return head

    def merge(self, other):
        """Слияние с другой биномиальной кучей."""
        new_head = self._merge_roots(self.head, other.head)
        if not new_head:
            return

        prev = None
        curr = new_head
        next = curr.sibling

        while next:
            # Условия: либо степени разные, либо 3 одинаковых подряд
            if curr.degree != next.degree or (next.sibling and next.sibling.degree == curr.degree):
                prev = curr
                curr = next
            else:
                if curr.key <= next.key:
                    curr.sibling = next.sibling
                    merge_trees(curr, next)
                else:
                    if prev:
                        prev.sibling = next
                    else:
                        new_head = next
                    merge_trees(next, curr)
                    curr = next
            next = curr.sibling

        self.head = new_head

    def insert(self, key):
        """Вставка нового элемента."""
        temp = BinomialHeap()
        temp.head = BinomialNode(key)
        self.merge(temp)

    def get_min(self):
        """Возврат минимального ключа без удаления."""
        if not self.head:
            return None
        x = self.head
        mn = x
        while x:
            if x.key < mn.key:
                mn = x
            x = x.sibling
        return mn.key


# 3. КУЧА ФИБОНАЧЧИ (Fibonacci Heap)

import math

class FibNode:
    """Узел кучи Фибоначчи."""
    def __init__(self, key):
        self.key = key
        self.degree = 0
        self.mark = False
        self.parent = None
        self.child = None
        self.left = self
        self.right = self


class FibonacciHeap:
    """Реализация минимальной кучи Фибоначчи."""

    def __init__(self):
        self.min = None  # указатель на минимальный узел
        self.n = 0       # количество элементов

    def insert(self, key):
        """Вставка нового элемента."""
        node = FibNode(key)
        if not self.min:
            self.min = node
        else:
            # Вставка в корневой список
            node.right = self.min.right
            node.left = self.min
            self.min.right.left = node
            self.min.right = node
            if node.key < self.min.key:
                self.min = node
        self.n += 1
        return node

    def _link(self, y, x):
        """Присоединение дерева y к узлу x (увеличение степени x)."""
        # Удаляем y из корневого списка
        y.left.right = y.right
        y.right.left = y.left

        y.parent = x
        if not x.child:
            x.child = y
            y.right = y.left = y
        else:
            y.right = x.child.right
            y.left = x.child
            x.child.right.left = y
            x.child.right = y

        x.degree += 1
        y.mark = False

    def extract_min(self):
        """Удаление и возврат минимального элемента."""
        z = self.min
        if z:
            # Переносим детей z в корневой список
            if z.child:
                children = []
                c = z.child
                while True:
                    children.append(c)
                    c = c.right
                    if c == z.child:
                        break

                for x in children:
                    # Отсоединяем от ребёнка
                    x.left.right = x.right
                    x.right.left = x.left

                    # Добавляем в корневой список
                    x.left = self.min
                    x.right = self.min.right
                    self.min.right.left = x
                    self.min.right = x
                    x.parent = None

            # Удаляем z из корневого списка
            z.left.right = z.right
            z.right.left = z.left

            if z == z.right:
                self.min = None
            else:
                self.min = z.right
                self._consolidate()
            self.n -= 1

        return z.key if z else None

    def _consolidate(self):
        """Фаза консолидации после удаления минимального элемента."""
        A = [None] * int(math.log(self.n, 2) + 3)
        nodes = []
        x = self.min

        # Собираем узлы корневого списка
        while True:
            nodes.append(x)
            x = x.right
            if x == self.min:
                break

        for w in nodes:
            x = w
            d = x.degree
            while A[d]:
                y = A[d]
                if y.key < x.key:
                    x, y = y, x
                self._link(y, x)
                A[d] = None
                d += 1
            A[d] = x

        # Поиск нового минимума
        self.min = None
        for i in A:
            if i:
                if not self.min or i.key < self.min.key:
                    self.min = i


# 4. ХЕШ-Таблица (Hash Table) с цепочками


class HashTable:
    """Хеш-таблица на основе цепочек (списки для коллизий)."""

    def __init__(self, size=16):
        self.size = size
        self.table = [[] for _ in range(size)]

    def _hash(self, key):
        """Хеш-функция: Python hash мод размер."""
        return hash(key) % self.size

    def put(self, key, value):
        """Сохранение значения по ключу."""
        idx = self._hash(key)
        for i, (k, v) in enumerate(self.table[idx]):
            if k == key:
                # Обновляем существующий ключ
                self.table[idx][i] = (key, value)
                return
        # Иначе добавляем новую пару
        self.table[idx].append((key, value))

    def get(self, key):
        """Получение значения по ключу."""
        idx = self._hash(key)
        for k, v in self.table[idx]:
            if k == key:
                return v
        return None

    def remove(self, key):
        """Удаление элемента."""
        idx = self._hash(key)
        for i, (k, v) in enumerate(self.table[idx]):
            if k == key:
                del self.table[idx][i]
                return True
        return False



