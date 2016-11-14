/****************************************************************
 * Header file for the application class for the queueing
 * theory program.
 *
 * Author/copyright:  Duncan Buell
 * Date: 23 May 2016
 *
**/

#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include <random>
#include <vector>
#include <map>
using namespace std;

#include "../../Utilities/utils.h"

#include "onevoter.h"
#include "myrandom.h"

class Queue {
public:
 Queue();
 virtual ~Queue();

 int GetHowManyMachinesLowerBound();
 int GetHowManyMachinesUpperBound();
 int GetHowManyVoters();
 int GetNumberOfTimeSteps();
 int GetRandomNumberSeed();
 int GetVoteTimeMean();
 int GetVoteTimeDev();

 void InitializeVoters();
 void ReadInputData(Scanner& in_stream);
 void RunSimulation(ofstream& out_stream);

 string ToString() const;
 string ToStringMultimap(const string label, const multimap<int, OneVoter> the_map) const;

private:
 int how_many_voters_;
 int how_many_machines_lower_bound_;
 int how_many_machines_upper_bound_;
 int number_of_time_steps_;
 int random_number_seed_;
 int vote_time_mean_;
 int vote_time_dev_;

 vector<int> free_machines_;

 multimap<int, OneVoter> voters_backup_;
 multimap<int, OneVoter> voters_pending_;
 multimap<int, OneVoter> voters_voting_;
 multimap<int, OneVoter> voters_done_voting_;

 MyRandom random_;

 void ComputeMeanAndDev(double& mean, double& dev);
 void DoStatistics(int machine_count, ofstream& out_stream);
 void RunSimulation2(int how_many_machines);
};

#endif
