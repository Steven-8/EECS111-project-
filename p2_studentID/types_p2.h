#ifndef __TYPES_P2_H
#define __TYPES_P2_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <unistd.h>

#define EMPTY        0
#define WOMENPRESENT 1
#define MENPRESENT   2


class Person
{

	int gender; // 0: male 1: female
	std::string str_gender;
	struct timeval t_create;
	struct timeval t_start;
	struct timeval t_end;
	int time_to_stay_ms;
	
	pthread_t tid;

	unsigned long order;
	unsigned long use_order;

public:
	Person();

	void set_gender(int data);
	int get_gender(void);

	void set_order(unsigned long data);
	unsigned long get_order(void);

	void set_use_order(unsigned long data);
	unsigned long get_use_order(void);

	void set_time(long data);
	int get_time(void);
	int ready_to_leave(void);

	void start(void);
	void complete(void);

	void set_tid(pthread_t tid) { this->tid = tid; }
	pthread_t get_tid() const { return tid; }

};


// Class for the restroom
// You may need to add more class member variables and functions
class Restroom {
	int status; // 0 - Empty, 1 - WomenPresent, 2 - MenPresent
	int men_present;
	int women_present;
	std::vector<Person> queue;
	unsigned long order_counter;
	// You need to define the data structure to
    // save the information of people using the restroom
	// You can probebly use Standard Template Library (STL) vector


public:
	Restroom() : status(0), men_present(0), women_present(0), order_counter(1){
/* 		status = EMPTY;
		men_present = EMPTY;
		women_present = EMPTY;
 */
	}

	// You need to use this function to print the Restroom's status
	void enter_restroom(Person &p);
	void leave_restroom(Person &p);
	void print_status(void);
	void add_person(Person &p);
	void remove_person(int);
	std::vector<Person>& get_queue(void);
	// Call by reference
	// This is just an example. You can implement any function you need
	int get_status(void);
	void set_status(int status);
	int get_womenPresent(void);
	int get_menPresent(void);
	unsigned long get_next_order() {return order_counter++;};
};










#endif
