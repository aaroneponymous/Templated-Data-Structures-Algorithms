#include <iostream>

// [x]: Quicksort Template Implementation
// NOTE: Swap Templated Function
template <typename T>
void swap(T* a, T* b)
{
    T temp = *a;
    *a = *b;
    *b = temp;
}


// NOTE: Partition Function
template <typename T>
int partition(T arr[], int low_index, int high_index)
{
    int mid_point = low_index + (high_index - low_index)/2;
    T pivot = arr[mid_point];

    bool done = false;

    while (!done)
    {
        while (arr[low_index] < pivot)
        {
            low_index++;
        }

        while (pivot < arr[high_index])
        {
            high_index--;
        }

        if (low_index >= high_index)
        {
            done = true;
        }
        else
        {
            // COMMENT: Pass by Reference& to swap values
            swap(&arr[low_index], &arr[high_index]);
            low_index++;
            high_index--;
        }
    }

    return high_index;

}

// NOTE: Quicksort Function
template <typename T>
void quicksort(T arr[], int low_index, int high_index)
{
    if (low_index >= high_index)
    {
        return;
    }

    int low_endpoint = partition(arr, low_index, high_index);
    partition(arr, low_index, low_endpoint);
    partition(arr, low_endpoint + 1, high_index);
}



int main()
{
    int arr[] = { 13, 1, 8, 3, 5, 2, 1 };
    int n = sizeof(arr) / sizeof(arr[0]);
    quicksort(arr, 0, n - 1);
    for (auto const element: arr)
    {
        std::cout << element << " ";
    }

    std::cout << std::endl;

    return 0;
}

