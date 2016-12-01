/****************************************************************
 * Header for the 'OneVoter' class.
 * A voter has a sequence, arrival time, and time spent voting as
 * parameters in the constructor. It also contains accessors for
 * various private variables.
 * 
 * General functions assign a voter station and format the
 * output for the user, the details for a voter. Voters are
 * are created until the simulation stops it and each voter is
 * a part of a voter precinct (onepct.cc). 
 *
 * Author/copyright:  Duncan Buell
 * Modified by Group 7
 * Date: 1 December 2016
 *
**/

#ifndef ONEVOTER_H
#define ONEVOTER_H

#include "../Utilities/utils.h"

using namespace std;

static int kDummyVoterInt = -333;

class OneVoter {
public:
/****************************************************************
 * Constructors and destructors for the class. 
**/
 OneVoter() = default;
 OneVoter(int sequence, int arrival_seconds, int duration_seconds);
 //each voter has a unique sequence, arrival time (in seconds), 
 //and time spent voting (duration) 
 
 virtual ~OneVoter() = default;

/****************************************************************
 * Accessors for voter details 
**/
 int GetStationNumber() const;
 int GetTimeArrival() const;
 int GetTimeDoneVoting() const;
 int GetTimeWaiting() const;

/****************************************************************
 * General functions to assign the voter to a station and check
 * if the voter has finished voting. Also has a ToString() and 
 * GetTimeInQ() that returns the amount of time the voter spent
 * waiting in line. The ToStringHeader() formats the output
 * without starting a new line for use in a table. 
**/
 void AssignStation(int station_number, int start_time_seconds);
 void DoneVoting();
 int GetTimeInQ() const;

 string ToString();
 static string ToStringHeader();

private:
 int sequence_ = kDummyVoterInt;
 int time_arrival_seconds_ = kDummyVoterInt;
 int time_done_voting_seconds_ = kDummyVoterInt;
 int time_start_voting_seconds_ = kDummyVoterInt;
 int time_vote_duration_seconds_ = kDummyVoterInt;
 int time_waiting_seconds_ = kDummyVoterInt;
 int which_station_ = kDummyVoterInt;
 //sets member variable values

/****************************************************************
 * General private functions to convert voter times from seconds
 * into a readable format for the user as they will be in 1000s
 * of seconds. These are used in the ToString().
**/
 string ConvertTime(int time_in_seconds) const;
 string GetTOD(int time) const;
};

#endif // ONEVOTER_H
