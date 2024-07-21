
template <typename Comparable>
class BinaryHeap
{
public:
    explicit BinaryHeap(int capacity = 100);
    explicit BinaryHeap(const vector<Comparable> &items);

    bool isEmpty() const;
    const Comparable& findMin() const;

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
        if (isEmpty()) throw UnderflowException{ };

        array[1] = std::move(array[current_size_--]);
        percolateDown(1);
    }

    /**
     * Remove the minimum item and place it in minItem
     * Throws UnderflowException if empty
     */
    void deleteMin(Comparable &minItem);
    void makeEmpty();

private:
    int current_size_;        // number of elements in heap
    vector<Comparable> array; // the heap array

    void buildHeap();
    // Hole:
    void percolateDown(int hole);
};
