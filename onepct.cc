#include "onepct.h"
/****************************************************************
* Implementation for the 'OnePct' class.
*
* Author/copyright:  Duncan Buell. All rights reserved.
* Commented by Alexander Reeser
* Date: 1 December 2016
*
**/

static const string kTag = "OnePct: ";

/****************************************************************
 * Constructor.
 * Creates an instance of OnePct by calling ReadData on the infile 
**/

OnePct::OnePct(Scanner& infile) {
  this->ReadData(infile);
}

/****************************************************************
* Accessors and mutators.
**/

/****************************************************************
 * Function GetExpectedVoters
 * Returns the number of voters expected to vote in the precinct
**/
int OnePct::GetExpectedVoters() const {
  return pct_expected_voters_;
}

/****************************************************************
 * Function GetPctNumber
 * Returns the number of the precinct
**/
int OnePct::GetPctNumber() const {
  return pct_number_;
}

/****************************************************************
* General functions.
**/

/****************************************************************
 * Function ComputeMeanAndDev
 * Written by Alexander Reeser {
 * This function is used to compute the average wait time of voters
 * in seconds and the standard deviation of voter wait times in 
 * seconds. First it computes the average wait time by summing the 
 * wait times of all the voters in the precinct then dividing that 
 * number by the number of expected voters in the precinct.
 * The standard deviation of wait times in seconds is then computed
 * by iterating through each of the voters in the precinct
 * then computing the addin for each one by subtracting the 
 * mean wait time in seconds from the current voter's wait time.
 * This number is then squared and added to sum_of_adjusted_times_seconds.
 * This is done for every voter in the precinct. Finally, the 
 * standard deviation of wait times is computed by square 
 * rooting the division of the sum_of_adjusted_times_seconds 
 * by the expected voter turnout in the precinct.
 * } endReeser
**/
void OnePct::ComputeMeanAndDev() {
  int sum_of_wait_times_seconds = 0;
  double sum_of_adjusted_times_seconds = 0.0;
  
  //Redundant Code: sum_of_wait_times_seconds is set to zero twice
  sum_of_wait_times_seconds = 0;
  multimap<int, OneVoter>::iterator iter_multimap;
  
  //Computes the average wait time
  for (iter_multimap = voters_done_voting_.begin();
       iter_multimap != voters_done_voting_.end(); ++iter_multimap) {
    OneVoter voter = iter_multimap->second;
    sum_of_wait_times_seconds += voter.GetTimeWaiting();
  }
  wait_mean_seconds_ = static_cast<double>(sum_of_wait_times_seconds)/
  static_cast<double>(pct_expected_voters_);
  
  //The standard deviation of wait times in seconds is computed here
  for (iter_multimap = voters_done_voting_.begin();
       iter_multimap != voters_done_voting_.end(); ++iter_multimap) {
    OneVoter voter = iter_multimap->second;
    double this_addin = static_cast<double>(voter.GetTimeWaiting())
                      - wait_mean_seconds_;

    sum_of_adjusted_times_seconds += (this_addin) * (this_addin);
  } //End for
  wait_dev_seconds_ = sqrt(sum_of_adjusted_times_seconds /
  static_cast<double>(pct_expected_voters_));
}

/****************************************************************
 * Function CreateVoters
 * Written by Alexander Reeser {
 * This function populates the voters_backup_ map with all the voters
 * of the day. This is done by taking the percentage of voters expected
 * to vote in a given hour and multiplying that number by the total number
 * of voters to get the number of voters to vote in that hour. This number 
 * is used to calculate the average number of voters to arrive each second
 * which is used to compute a RandomExponentialInt. This int corresponds
 * to the time of the next voter's arrival in seconds. The time it takes the
 * voter to vote is simulated with a RandomUniformInt using the size of
 * the actual_service_times_ vector which is returned by the 
 * GetMaxServiceSubscript function, as a seed. Finally, a voter is
 * created using the sequence (voter number), arrival (arrival time),
 * and duration (time it take the voter to vote) which is then added to the 
 * voters_backup_ map.
 * } endReeser 
**/
void OnePct::CreateVoters(const Configuration& config, MyRandom& random,
                          ofstream& out_stream) {
  int duration = 0;
  int arrival = 0;
  int sequence = 0;
  double percent = 0.0;
  string outstring = "XX";

  voters_backup_.clear();
  sequence = 0;

  percent = config.arrival_zero_;
  int voters_at_zero = round((percent / 100.0) * pct_expected_voters_);
  arrival = 0;
  
  //voters_at_zero is always zero.
  for (int voter = 0; voter < voters_at_zero; ++voter) {
    int durationsub = random.RandomUniformInt(0, config.GetMaxServiceSubscript());
    duration = config.actual_service_times_.at(durationsub);
    OneVoter one_voter(sequence, arrival, duration);
    voters_backup_.insert(std::pair<int, OneVoter>(arrival, one_voter));
    ++sequence;
  }

  //Runs once for every hour the polls are open.
  for (int hour = 0; hour < config.election_day_length_hours_; ++hour) {
    //Gets the percentage of voters voting at that hour.
    percent = config.arrival_fractions_.at(hour);
    //Aproximates the number of voters in an hour
    int voters_this_hour = round((percent / 100.0) * pct_expected_voters_);
    //Adds one to the number of voters in an hour half the time in order
    //to offset the error caused by integers rounding down.
    if (0 == hour%2) 
      ++voters_this_hour;
    
    //Integer used to find the time of a voter's arrival in seconds
    int arrival = hour*3600;
    for(int voter = 0; voter < voters_this_hour; ++voter) {
      //Finds the average number of voters arriving every second.
      //This number is used to calculate a RandomExponentialInt
      //which is used to simulate the time the next voter will arrive.
      double lambda = static_cast<double>(voters_this_hour / 3600.0);
      int interarrival = random.RandomExponentialInt(lambda);
      arrival += interarrival;
      
      //Gets a random voting duration from the actual_service_times_
      //Integer vector by getting a random number with GetMaxServiceSubscript
      int durationsub = random.RandomUniformInt(0, config.GetMaxServiceSubscript());
      duration = config.actual_service_times_.at(durationsub);

      //Creates a voter using the sequence (voter number), arrival (arrival time),
      //and duration (time it take the voter to vote) then adds it to the 
      //voters_backup_ map.
      OneVoter one_voter(sequence, arrival, duration);
      voters_backup_.insert(std::pair<int, OneVoter>(arrival, one_voter));
      ++sequence;
    }
  }
}

/******************************************************************************
**/
int OnePct::DoStatistics(int iteration, const Configuration& config,
                         int station_count, map<int, int>& map_for_histo,
                         ofstream& out_stream) {
  string outstring = "\n";
  map<int, int> wait_time_minutes_map;

/////////////////////////////////////////////////////////////////////////////
  multimap<int, OneVoter>::iterator iter_multimap;
  for (iter_multimap = this->voters_done_voting_.begin();
       iter_multimap != this->voters_done_voting_.end(); ++iter_multimap) {
    OneVoter voter = iter_multimap->second;
    int wait_time_minutes = voter.GetTimeWaiting() / 60; // secs to mins

    ++(wait_time_minutes_map[wait_time_minutes]);
    ++(map_for_histo[wait_time_minutes]);
  }

/////////////////////////////////////////////////////////////////////////////
  int toolongcount = 0;
  int toolongcountplus10 = 0;
  int toolongcountplus20 = 0;
  for (auto iter = wait_time_minutes_map.rbegin();
            iter != wait_time_minutes_map.rend(); ++iter) {
    int waittime = iter->first;
    int waitcount = iter->second;
    if (waittime > config.wait_time_minutes_that_is_too_long_)
      toolongcount += waitcount;
    if (waittime > config.wait_time_minutes_that_is_too_long_+10)
      toolongcountplus10 += waitcount;
    if (waittime > config.wait_time_minutes_that_is_too_long_+20)
      toolongcountplus20 += waitcount;
  }

/////////////////////////////////////////////////////////////////////////////
  ComputeMeanAndDev();
  outstring = "";
  outstring += kTag + Utils::Format(iteration, 3) + " "
            + Utils::Format(pct_number_, 4) + " "
            + Utils::Format(pct_name_, 25, "left")
            + Utils::Format(pct_expected_voters_, 6)
            + Utils::Format(station_count, 4)
            + " stations, mean/dev wait (mins) "
            + Utils::Format(wait_mean_seconds_/60.0, 8, 2) + " "
            + Utils::Format(wait_dev_seconds_/60.0, 8, 2) + " toolong "
            + Utils::Format(toolongcount, 6) + " "
            + Utils::Format(100.0*toolongcount/(double)pct_expected_voters_, 6, 2)
            + Utils::Format(toolongcountplus10, 6) + " "
            + Utils::Format(100.0*toolongcountplus10/(double)pct_expected_voters_, 6, 2)
            + Utils::Format(toolongcountplus20, 6) + " "
            + Utils::Format(100.0*toolongcountplus20/(double)pct_expected_voters_, 6, 2)
            + "\n";

  Utils::Output(outstring, out_stream, Utils::log_stream);

  wait_time_minutes_map.clear();

  return toolongcount;
}

/****************************************************************
**/
void OnePct::ReadData(Scanner& infile) {
  if (infile.HasNext()) {
    pct_number_ = infile.NextInt();
    pct_name_ = infile.Next();
    pct_turnout_ = infile.NextDouble();
    pct_num_voters_ = infile.NextInt();
    pct_expected_voters_ = infile.NextInt();
    pct_expected_per_hour_ = infile.NextInt();
    pct_stations_ = infile.NextInt();
    pct_minority_ = infile.NextDouble();

    int stat1 = infile.NextInt();
    int stat2 = infile.NextInt();
    int stat3 = infile.NextInt();
    stations_to_histo_.insert(stat1);
    stations_to_histo_.insert(stat2);
    stations_to_histo_.insert(stat3);

  }
} // void OnePct::ReadData(Scanner& infile)

/****************************************************************
 * Function RunSimulationPct
 * Written by Alexander Reeser {
 * This function starts by computing the range of stations for
 * which the simulation will run. The min_station_count is 
 * calculated by taking the expected voters in the precinct and
 * multiplying that by the average time it takes one person to vote
 * then dividing by the amount of time the polls are open. This
 * number must be at least one, if it is not, it is set to one.
 * The max_station_count is the min_station_count plus the 
 * number of hours the polls are open. 
 * } endReeser 
**/
void OnePct::RunSimulationPct(const Configuration& config,
                       MyRandom& random, ofstream& out_stream) {
  string outstring = "XX";

  int min_station_count = pct_expected_voters_ * config.time_to_vote_mean_seconds_;
  min_station_count = min_station_count / (config.election_day_length_hours_*3600);
  if (min_station_count <= 0)
    min_station_count = 1;

  int max_station_count = min_station_count + config.election_day_length_hours_;

  bool done_with_this_count = false;
  //The rest of the function is contained in this for loop
  for (int stations_count = min_station_count;
           stations_count <= max_station_count; ++stations_count) {

    if (done_with_this_count)
      break;

    done_with_this_count = true;

    map<int, int> map_for_histo;

    outstring = kTag + this->ToString() + "\n";
    Utils::Output(outstring, out_stream, Utils::log_stream);
    
    for (int iteration = 0;
         iteration < config.number_of_iterations_; ++iteration) {
      //Calls CreateVoters
      this->CreateVoters(config, random, out_stream);

      voters_pending_ = voters_backup_;
      voters_voting_.clear();
      voters_done_voting_.clear();

      //Calls RunSimulationPct2 
      this->RunSimulationPct2(stations_count);
      
      //Calls DoStatistics
      int number_too_long = DoStatistics(iteration, config, stations_count,
                                         map_for_histo, out_stream);
      if (number_too_long > 0) {
        done_with_this_count = false;
      }
    }

    voters_voting_.clear();
    voters_done_voting_.clear();

    outstring = kTag + "toolong space filler\n";
    Utils::Output(outstring, out_stream, Utils::log_stream);

    if (stations_to_histo_.count(stations_count) > 0) {
      outstring = "\n" + kTag + "HISTO " + this->ToString() + "\n";
      outstring += kTag + "HISTO STATIONS "
                + Utils::Format(stations_count, 4) + "\n";
      Utils::Output(outstring, out_stream, Utils::log_stream);

      int time_lower = (map_for_histo.begin())->first;
      int time_upper = (map_for_histo.rbegin())->first;

      int voters_per_star = 1;
      if (map_for_histo[time_lower] > 50) {
        voters_per_star = map_for_histo[time_lower]/(50 * config.number_of_iterations_);
        if (voters_per_star <= 0)
          voters_per_star = 1;
      }

      for (int time = time_lower; time <= time_upper; ++time) {
        int count = map_for_histo[time];

        double count_double = static_cast<double>(count) /
        static_cast<double>(config.number_of_iterations_);

        int count_divided_ceiling = static_cast<int>(ceil(count_double/voters_per_star));
        string stars = string(count_divided_ceiling, '*');

        outstring = kTag + "HISTO " + Utils::Format(time, 6) + ": "
                         + Utils::Format(count_double, 7, 2) + ": ";
        outstring += stars + "\n";
        Utils::Output(outstring, out_stream, Utils::log_stream);
      }
      outstring = "HISTO\n\n";
      Utils::Output(outstring, out_stream, Utils::log_stream);
    }
  }
}

/****************************************************************
*
**/
void OnePct::RunSimulationPct2(int stations_count) {

  free_stations_.clear();
  for (int i = 0; i < stations_count; ++i) {
    free_stations_.push_back(i);
  }

  voters_voting_.clear();
  voters_done_voting_.clear();

  int second = 0;
  bool done = false;
  while (!done) {

    for (auto iter = voters_voting_.begin(); iter != voters_voting_.end(); ++iter) {
      if (second == iter->first) {
        OneVoter one_voter = iter->second;

        int which_station = one_voter.GetStationNumber();
        free_stations_.push_back(which_station);
        voters_done_voting_.insert(std::pair<int, OneVoter>(second, one_voter));
      }
    }
    voters_voting_.erase(second);

    vector<map<int, OneVoter>::iterator > voters_pending_to_erase_by_iterator;
      for (auto iter = voters_pending_.begin(); iter != voters_pending_.end(); ++iter) {
        if (second >= iter->first) {       // if they have already arrived
          if (free_stations_.size() > 0) { // and there are free stations
            OneVoter next_voter = iter->second;
            if (next_voter.GetTimeArrival() <= second) {
              int which_station = free_stations_.at(0);
              free_stations_.erase(free_stations_.begin());
              next_voter.AssignStation(which_station, second);
              int leave_time = next_voter.GetTimeDoneVoting();
              voters_voting_.insert(std::pair<int, OneVoter>(leave_time, next_voter));
              voters_pending_to_erase_by_iterator.push_back(iter);

// This was commented out 6 October 2016
//            Utils::log_stream << kTag << "ASSIGNED    "
//                              << Utils::Format(second, 5) << ": "
//                              << next_voter.ToString() << "\n";

/*
Utils::log_stream << kTag << "PENDING, VOTING, DONE    "
<< Utils::Format((int)voters_pending_.size(), 5) << ": "
<< Utils::Format((int)voters_voting_.size(), 5) << ": "
<< Utils::Format((int)voters_done_voting_.size(), 5) << endl;
*/
            } // if (next_voter.GetTimeArrival() <= second) {
          } // if (free_stations_.size() > 0) {
        }
        else { // if (second == iter->first) {
          break; // we have walked in time past current time to arrivals in the future
        }
      } // for (auto iter = voters_pending_.begin(); iter != voters_pending_.end(); ++iter) {

      for (auto iter = voters_pending_to_erase_by_iterator.begin();
                iter != voters_pending_to_erase_by_iterator.end(); ++iter) {
        voters_pending_.erase(*iter);
      }
      ++second;
//    if (second > 500) break;
      done = true;
      if ((voters_pending_.size() > 0) || (voters_voting_.size() > 0)) {
        done = false;
      }
    } // while (!done) {

  } // void Simulation::RunSimulationPct2()

/****************************************************************
**/
  string OnePct::ToString() {
  string s = "";

  s += Utils::Format(pct_number_, 4);
  s += " " + Utils::Format(pct_name_, 25, "left");
  s += Utils::Format(pct_turnout_, 8, 2);
  s += Utils::Format(pct_num_voters_, 8);
  s += Utils::Format(pct_expected_voters_, 8);
  s += Utils::Format(pct_expected_per_hour_, 8);
  s += Utils::Format(pct_stations_, 3);
  s += Utils::Format(pct_minority_, 8, 2);

  s += " HH ";
  for (auto iter = stations_to_histo_.begin();
            iter != stations_to_histo_.end(); ++iter) {
    s += Utils::Format(*iter, 4);
  }
  s += " HH";

  return s;
} // string OnePct::ToString()

/****************************************************************
**/
string OnePct::ToStringVoterMap(string label,
               multimap<int, OneVoter> themap) {
  string s = "";

  s += "\n" + label + " WITH " + Utils::Format((int)themap.size(), 6)
                    + " ENTRIES\n";
  s += OneVoter::ToStringHeader() + "\n";
  for (auto iter = themap.begin(); iter != themap.end(); ++iter) {
    s += (iter->second).ToString() + "\n";
  }

  return s;
} // string OnePct::ToString()
