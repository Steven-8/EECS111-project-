#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <cstdio>
#include <cmath>
#include <unistd.h>
#include <sys/wait.h>

#include "p1_process.h"
#include "p1_threads.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

// This file implements the multi-processing logic for the project

void read_input_file(const string &input_file_name, vector<student> &students)
{
  ifstream input_file(input_file_name.c_str());
  if (input_file.is_open())
  {
    string line;
    getline(input_file, line);

    while (getline(input_file, line))
    {
      stringstream ss(line);
      string id_str, grade_str;
      getline(ss, id_str, ',');
      getline(ss, grade_str, ',');

      unsigned long id = strtoul(id_str.c_str(), NULL, 0);
      double grade = strtod(grade_str.c_str(), NULL);

      student new_student(id, grade);

      students.push_back(new_student);
    }

    input_file.close();
  }

  else
  {
    std::cerr << "Unable to open file" << std::endl;
  }
}

void write_output_files(const string &output_sorted_file_name, const string &output_stats_file_name, const vector<student> &sorted_students)
{
  ofstream sorted_output(output_sorted_file_name.c_str());
  ofstream stats_output(output_stats_file_name.c_str());

  if (sorted_output.is_open() && stats_output.is_open())
  {
    double sum = 0;
    stringstream sorted_buffer;
    int rank = 1;
    sorted_buffer << "Rank,"
                  << "Student ID,"
                  << "Grade" << endl;
    for (vector<student>::const_iterator it = sorted_students.begin(); it != sorted_students.end(); ++it)
    {
      sorted_buffer << rank << "," << it->id << "," << it->grade << endl;
      sum += it->grade;
      rank++;
    }

    sorted_output << sorted_buffer.str();

    double average = sum / sorted_students.size();
    double stddev_sum = 0;

    for (vector<student>::const_iterator it = sorted_students.begin(); it != sorted_students.end(); ++it)
    {
      stddev_sum += pow(it->grade - average, 2);
    }

    double stddev = sqrt(stddev_sum / sorted_students.size());

    double median;
    if (sorted_students.size() % 2 == 0)
    {
      median = (sorted_students[sorted_students.size() / 2 - 1].grade + sorted_students[sorted_students.size() / 2].grade) / 2;
    }
    else
    {
      median = sorted_students[sorted_students.size() / 2].grade;
    }

    stats_output << "Average,Median,Std. Dev" << endl;
    stats_output << std::fixed << std::setprecision(3) << average << "," << std::fixed << std::setprecision(3) << median << "," << std::fixed << std::setprecision(3) << stddev << endl;

    sorted_output.close();
    stats_output.close();
  }
  else
  {
    cerr << "Unable to open output files" << endl;
  }
}

// This function should be called in each child process right after forking
// The input vector should be a subset of the original files vector
void process_classes(vector<string> classes, int num_threads)
{
  printf("Child process is created. (pid: %d)\n", getpid());
  // Each process should use the sort function which you have defined
  // in the p1_threads.cpp for multithread sorting of the data.

  for (int i = 0; i < classes.size(); i++)
  {
    // get all the input/output file names here
    string class_name = classes[i];
    char buffer[40];
    sprintf(buffer, "input/%s.csv", class_name.c_str());
    string input_file_name(buffer);

    sprintf(buffer, "output/%s_sorted.csv", class_name.c_str());
    string output_sorted_file_name(buffer);

    sprintf(buffer, "output/%s_stats.csv", class_name.c_str());
    string output_stats_file_name(buffer);

    vector<student> students;

    // Your implementation goes here, you will need to implement:
    // File I/O
    //  - This means readinmakeg the input file, and creating a list of students,
    //  see p1_process.h for the definition of the student struct
    //
    //  - Also, once the sorting is done and the statistics are generated, this means
    //  creating the appropritate output files
    //
    // Multithreaded Sorting
    //  - See p1_thread.cpp and p1_thread.h
    //
    //  - The code to run the sorter has already been provided
    //
    // Generating Statistics
    //  - This can be done after sorting or during

    // Run multi threaded sort
    read_input_file(input_file_name, students);
    ParallelMergeSorter sorter(students, num_threads);
    vector<student> sorted = sorter.run_sort();

    write_output_files(output_sorted_file_name, output_stats_file_name, sorted);
  }

  // child process done, exit the program
  printf("Child process is terminated. (pid: %d)\n", getpid());
  exit(0);
}

void create_processes_and_sort(vector<string> class_names, int num_processes, int num_threads)
{
  vector<pid_t> child_pids;
  int classes_per_process = max(class_names.size() / num_processes, 1ul);

  // This code is provided to you to test your sorter, this code does not use any child processes
  // Remove this later on
  // process_classes(class_names, 1);

  // Your implementation goes here, you will need to implement:
  // Splitting up work
  //   - Split up the input vector of classes into num_processes sublists
  //
  //   - Make sure all classes are included, remember integer division rounds down
  //
  // Creating child processes
  //   - Each child process will handle one of the sublists from above via process_classes
  //
  // Waiting for all child processes to terminate

  vector<vector<string> > split_class_names(num_processes);
  for (size_t i = 0; i < class_names.size(); ++i)
  {
    split_class_names[i % num_processes].push_back(class_names[i]);
  }

  // Creating child processes
  for (int i = 0; i < num_processes; ++i)
  {
    pid_t pid = fork();

    if (pid < 0)
    {
      // Error occurred during fork
      perror("fork");
      exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
      // Child process
      process_classes(split_class_names[i], num_threads);
      exit(EXIT_SUCCESS);
    }
    else
    {
      // Parent process
      child_pids.push_back(pid);
    }
  }

  // Waiting for all child processes to terminate
  for (std::vector<pid_t>::iterator it = child_pids.begin(); it != child_pids.end(); ++it)
  {
    pid_t child_pid = *it;
    int status;
    waitpid(child_pid, &status, 0);
  }
}
