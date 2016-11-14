#include "onevoter.h"

/******************************************************************************
 *3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
 * Class 'OneVoter' for the queueing theory computation.
 *
 * Author: Duncan A. Buell
 * Date last modified: 11 July 2016
**/

/******************************************************************************
 * Constructor
**/
OneVoter::OneVoter() : sequence_number_(kDummySequenceNumber),
                       time_arrival_(kDummyTimeArrival),
                       time_start_voting_(kDummyTimeStart),
                       time_vote_duration_(kDummyTimeDuration),
                       machine_number_(kDummyMachineNumber),
                       time_done_voting_(kDummyTimeDone),
                       time_waiting_(kDummyTimeWaiting) {
// empty by design
}

/******************************************************************************
 * Constructor
**/
OneVoter::OneVoter(int sequence, int arrival, int duration) {
}

/******************************************************************************
 * Destructor
**/
OneVoter::~OneVoter() {
}

/******************************************************************************
 * Accessors and Mutators
**/

/******************************************************************************
 * Accessor for 'machine_number_'
**/
int OneVoter::GetMachineNumber() {
}

/******************************************************************************
 * Accessor for 'sequence_number_'
**/
int OneVoter::GetSequenceNumber() {
}

/******************************************************************************
 * Accessor for 'time_arrival_'
**/
int OneVoter::GetTimeArrival() {
}

/******************************************************************************
 * Accessor for 'time_done_voting_'
**/
int OneVoter::GetTimeDoneVoting() {
}

/******************************************************************************
 * Accessor for 'time_waiting_'
**/
int OneVoter::GetTimeWaiting() {
}

/******************************************************************************
 * General functions.
**/

/******************************************************************************
 * Function 'AssignMachine' to assign this voter to a machine.
**/
void OneVoter::AssignMachine(int machine_number, int start_time) {
}

/******************************************************************************
 * Function 'DoneVoting' to close out the record for this voter.
**/
void OneVoter::DoneVoting() {
}

/******************************************************************************
 * Function 'ToString'.
**/
string OneVoter::ToString() const {
  string s = "";

  return s;
}
