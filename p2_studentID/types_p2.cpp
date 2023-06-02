#include "types_p2.h"
#include "utils.h"

void Person::set_gender(int data) { gender = data; }
int Person::get_gender(void) { return gender; }

void Person::set_order(unsigned long data) { order = data; }
unsigned long Person::get_order(void) { return order; }

void Person::set_use_order(unsigned long data) { use_order = data; }
unsigned long Person::get_use_order(void) { return use_order; }

void Person::set_time(long data) { time_to_stay_ms = data; }
int Person::get_time(void){
	return time_to_stay_ms;
}

int Person::ready_to_leave(void)
{
	struct timeval t_curr;
	gettimeofday(&t_curr, NULL);

	if (get_elasped_time(t_start, t_curr) >= time_to_stay_ms)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void Person::start(void)
{
	gettimeofday(&t_start, NULL);
	printf("(%lu)th person enters the restroom: \n", order);
	printf(" - (%lu) milliseconds after the creation\n", get_elasped_time(t_create, t_start));
}

void Person::complete(void)
{
	gettimeofday(&t_end, NULL);
	printf("(%lu)th person comes out of the restroom: \n", order);
	printf(" - (%lu) milliseconds after the creation\n", get_elasped_time(t_create, t_end));
	printf(" - (%lu) milliseconds after using the restroom\n", get_elasped_time(t_start, t_end));
}

Person::Person()
{
	gettimeofday(&t_create, NULL);
}

// You need to use this function to print the Restroom's status
void Restroom::print_status(void)
{
	std::string s = (status == 0) ? "Empty" : ((status == 1) ? "WomenPresent" : "MenPresent");
	std::cout << "Restroom Status: " << status << " Men: " << men_present << "Women: " << women_present << std::endl;
	printf("Print restroom status\n");
}

// Call by reference
// This is just an example. You can implement any function you need
int Restroom::get_status(void)
{
	return this->status;
}

void Restroom::set_status(int status)
{
	this->status = status;
}

int Restroom::get_menPresent(void)
{
	return this->men_present;
}

int Restroom::get_womenPresent(void)
{
	return this->women_present;
}

void Restroom::enter_restroom(Person &p)
{
	if (p.get_gender() == 0)
	{
		status = WOMENPRESENT;
	}
	else
	{
		status = MENPRESENT;
	}
}

void Restroom::leave_restroom(Person &p)
{
	if (p.get_gender() == 0)
	{
		men_present--;
	}
	else
	{
		women_present--;
	}

	if (men_present == 0 && women_present == 0)
	{
		status = EMPTY;
	}

}

void Restroom::remove_person(int gender)
{
	if (!queue.empty())
	{
		for (std::vector<Person>::iterator it = queue.begin(); it != queue.end(); ++it)
		{
			if (it->get_gender() == gender)
			{
				queue.erase(it);
				break;
			}
		}
	}
}

void Restroom::add_person(Person &p)
{
	if (p.get_gender() == 0)
	{
		women_present++;
	}
	else
	{
		men_present++;
	}
	queue.push_back(p);
}
std::vector<Person> &Restroom::get_queue(void)
{
	return this->queue;
}