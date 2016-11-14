/****************************************************************
 * Header file for the class 'OneVoter' for the queueing
 * theory program.
 *
 * Author/copyright:  Duncan Buell
 * Date: 23 May 2016
 *
**/

#ifndef ONEVOTER_H
#define ONEVOTER_H

#include <iostream>
#include <random>
using namespace std;

#include "../../Utilities/utils.h" // used for formatting

#include "myrandom.h"

const int kDummyTimeArrival    = -11;
const int kDummyMachineNumber  = -22;
const int kDummySequenceNumber = -33;
const int kDummyTimeDone       = -44;
const int kDummyTimeDuration   = -55;
const int kDummyTimeStart      = -66;
const int kDummyTimeWaiting    = -77;

class OneVoter {
public:
 OneVoter();
 OneVoter(int sequence, int arrival, int duration);
 virtual ~OneVoter();

 int GetMachineNumber();
 int GetSequenceNumber();
 int GetTimeArrival();
 int GetTimeDoneVoting();
 int GetTimeStartVoting();
 int GetTimeVoteDuration();
 int GetTimeWaiting();

 void AssignMachine(int machine_number, int start_time);
 void DoneVoting();

 string ToString() const;

private:
 int machine_number_ = kDummyMachineNumber;
 int sequence_number_ = kDummySequenceNumber;
 int time_arrival_ = kDummyTimeArrival;
 int time_done_voting_ = kDummyTimeDone;
 int time_start_voting_ = kDummyTimeStart;
 int time_vote_duration_ = kDummyTimeDuration;
 int time_waiting_ = kDummyTimeWaiting;

};

#endif
