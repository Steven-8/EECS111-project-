#include <vector>
#include <pthread.h>
#include <cstring>
#include <string>
#include <cstdlib>
#include <iostream>

#include "p1_process.h"
#include "p1_threads.h"

using namespace std;

// This file implements the ParallelMergeSorter class definition found in p1_threads.h
// It is not required to use the defined classes/functions, but it may be helpful

// This struct is used to pass arguments into each thread (inside of thread_init)
// ctx is essentially the "this" keyword, but since we are in a static function, we cannot use "this"
//
// Feel free to modify this struct in any way
struct MergeSortArgs
{
  int thread_index;
  ParallelMergeSorter *ctx;

  MergeSortArgs(ParallelMergeSorter *ctx, int thread_index)
  {
    this->ctx = ctx;
    this->thread_index = thread_index;
  }
};

// Class constructor
ParallelMergeSorter::ParallelMergeSorter(vector<student> &original_list, int num_threads)
{
  this->threads = vector<pthread_t>(num_threads);
  this->sorted_list = vector<student>(original_list);
  this->num_threads = num_threads;
}

// This function will be called by each child process to perform multithreaded sorting
vector<student> ParallelMergeSorter::run_sort()
{
  for (int i = 0; i < num_threads; i++)
  {
    // We have to use the heap for this otherwise args will be destructed in each iteration,
    // and the thread will not have the correct args struct
    MergeSortArgs *args = new MergeSortArgs(this, i);

    pthread_create(&threads[i], NULL, &ParallelMergeSorter::thread_init, (void *)args);

    /*
     * Uncomment this code for testing sorting without threads
     *
     * thread_init((void *) args);
     */


    // Your implementation goes here, you will need to implement:
    // Creating worker threads
    //  - Each worker thread will use ParallelMergeSorter::thread_init as its start routine
    //
    //  - Since the args are taken in as a void *, you will need to use
    //  the MergeSortArgs struct to pass multiple parameters (pass a pointer to the struct)
    //
    //  - Don't forget to make sure all threads are done before merging their sorted sublists
  }

  for (int i = 0; i < num_threads; i++)
  {
    pthread_join(threads[i], NULL);
  }

  // Merge sorted sublists together
  this->merge_threads();

  return this->sorted_list;
}

// Standard merge sort implementation
void ParallelMergeSorter::merge_sort(int lower, int upper)
{
  // Your implementation goes here, you will need to implement:
  // Top-down merge sort

  if (lower < upper - 1)
  {
    int middle = (lower + upper) / 2;
    merge_sort(lower, middle);
    merge_sort(middle, upper);
    merge(lower, middle, upper);
  }
}

// Standard merge implementation for merge sort
void ParallelMergeSorter::merge(int lower, int middle, int upper)
{
  // Your implementation goes here, you will need to implement:
  // Merge for top-down merge sort
  //  - The merge results should go in temporary list, and once the merge is done, the values
  //  from the temporary list should be copied back into this->sorted_list
  int left_size = middle - lower;
  int right_size = upper - middle;

  vector<student> left_sorted;
  vector<student> right_sorted;

  for (int i = 0; i < left_size; i++)
  {
    left_sorted.push_back(sorted_list[lower + i]);
  }

  for (int i = 0; i < right_size; i++)
  {
    right_sorted.push_back(sorted_list[middle + i]);
  }

  int i = 0, j = 0, k = lower;
  while (i < left_size && j < right_size)
  {
    if (left_sorted[i].grade >= right_sorted[j].grade)
    {
      sorted_list[k++] = left_sorted[i++];
    }
    else
    {
      sorted_list[k++] = right_sorted[j++];
    }
  }

  while (i < left_size)
  {
    sorted_list[k++] = left_sorted[i++];
  }

  while (j < right_size)
  {
    sorted_list[k++] = right_sorted[j++];
  }
}

// This function will be used to merge the resulting sorted sublists together
void ParallelMergeSorter::merge_threads()
{
  int work_per_thread = sorted_list.size() / num_threads;

  while (num_threads > 1)
  {
    int i;
    for (i = 0; i < num_threads / 2; i++)
    {
      int lower = i * work_per_thread * 2;
      int middle = lower + work_per_thread;
      int upper = middle + work_per_thread;
/* 
      if (i == num_threads / 2 - 1)
      {
        upper = sorted_list.size();
      } */
      merge(lower, middle, upper);
    }

    // When the number of threads is odd, merge the last sublist with the second last sublist
    if (num_threads % 2 == 1)
    {
      int lower = i * work_per_thread * 2;
      int middle = lower + work_per_thread;
      int upper = sorted_list.size();
      merge(lower, middle, upper);
    }

    num_threads = (num_threads + 1) / 2;
    work_per_thread *= 2;
  }

  // Your implementation goes here, you will need to implement:
  // Merging the sorted sublists together
  //  - Each worker thread only sorts a subset of the entire list, therefore once all
  //  worker threads are done, we are left with multiple sorted sublists which then need to
  //  be merged once again to result in one total sorted list
}

// This function is the start routine for the created threads, it should perform merge sort on its assigned sublist
// Since this function is static (pthread_create must take a static function), we cannot access "this" and must use ctx instead
void *ParallelMergeSorter::thread_init(void *args)
{

  MergeSortArgs *sort_args = (MergeSortArgs *)args;
  int thread_index = sort_args->thread_index;
  ParallelMergeSorter *ctx = sort_args->ctx;

  int work_per_thread = ctx->sorted_list.size() / ctx->num_threads;
  int lower = thread_index * work_per_thread;
  int upper;

  if (thread_index == ctx->num_threads - 1)
  {
    upper = ctx->sorted_list.size();
  }
  else
  {
    upper = (thread_index + 1) * work_per_thread;
  }
 
  ctx->merge_sort(lower, upper);

  // Your implementation goes here, you will need to implement:
  // Getting the sublist to sort based on the thread index
  //  - The lower bound is thread_index * work_per_thread, the upper bound is (thread_index + 1) * work_per_thread
  //
  //  - The range of merge sort is typically [lower, upper), lower inclusive, upper exclusive
  //
  //  - Remember to make sure all elements are included in the sort, integer division rounds down
  //
  // Running merge sort
  //  - The provided functions assume a top-down implementation
  //
  //  - Many implementations of merge sort are online, feel free to refer to them (wikipedia is good)
  //
  //  - It may make sense to equivalate this function as the non recursive "helper function" that merge sort normally has

  // Free the heap allocation
  delete sort_args;
  return NULL;
}
