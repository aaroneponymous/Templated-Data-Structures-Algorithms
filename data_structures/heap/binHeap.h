
template <typename Comparable>
class BinaryHeap
{
public:
    explicit BinaryHeap(int capacity = 100);
    explicit BinaryHeap(const vector<Comparable> &items);

    bool isEmpty() const;
    const Comparable &findMin() const;

    /**
     * Insert item x, allowing duplicates
     */

    void insert(const Comparable &x)
    {
        if (currentSize == array.size() - 1)
            array.resize(array.resize() * 2);

        // Percolate Up
        int hole = ++currentSize;
        Comparable copy = x;

        /* Put in 0 to prevent doing an explicit test to test if hole is 1
         * We just save x in 0, so when hole is 1 the loop performs
         * (x < array[hole/2 -> 0.5 ~ 0]) which returns false and after the loop
         * we just assign the value in 0 to hole which is 0 */

        array[0] = std::move(copy);
        for (; x < array[hole / 2]; hole /= 2)
        {
            array[hole] = std::move(array[hole / 2]);
        }

        array[hole] = std::move(array[0]); // array[0] always is -INF (used as a placeholder)
    }

    /**
     * Remove the minimum item
     * Throws UnderflowException if empty
     */

    void deleteMin()
    {
        if (isEmpty())
            throw UnderflowException{};

        /** Move the minimum element (root of the heap) to minItem.
         * array[1] contains the minimum element of the heap, as the binary heap property ensures the
         * smallest element is always at the root. std::move(array[1]) casts array[1] to an rvalue reference,
         * allowing for the move assignment operator to be used. This means the value of array[1] is moved to
         * minItem, and array[1] is left in a valid but unspecified state.
         */

        Comparable minItem = std::move(array[1]);

        /** Move the last element in the heap to the root position.
         * current_size_ is the index of the last element in the heap.
         * array[current_size_--] accesses the last element in the heap and then decrements current_size_ by 1,
         * effectively removing the last element from the heap.
         * std::move(array[current_size_--]) casts the last element to an rvalue reference,
         * allowing for the move assignment operator to be used.
         * array[1] = std::move(array[current_size_--]) moves the last element to the root position (index 1).
         */

        array[1] = std::move(array[current_size_--]);

        // Percolate down the element at the root to restore the heap property.
        percolateDown(1);
    }

    /**
     * Remove the minimum item and place it in minItem
     * Throws UnderflowException if empty
     */
    void deleteMin(Comparable &minItem)
    {
        if (isEmpty())
            throw UnderflowException{};

        minItem = std::move(array[1]);
        array[1] = std::move(array[current_size_--]);
        percolateDown(1);
    }
    void makeEmpty();

private:
    int current_size_;        // number of elements in heap
    vector<Comparable> array; // the heap array

    void buildHeap();
    // Hole:
    void percolateDown(int hole)
    {
        int child;
        Comparable tmp = std::move(array[hole]);

        for ( ; hole * 2 <= current_size_; hole = child)
        {
            child = hole * 2;
            if (child != current_size_ && array[child + 1] < array[child]) ++child;
            if (array[child] < tmp) 
            {
                array[hole] = std::move(array[child]);
            }
            else break;
        }
        array[hole] = std::move(tmp);
    }
};
