#include "onepct.h"
/****************************************************************
 * Implementation for the 'OnePct' class.
 *
 * Author/copyright:  Duncan Buell. All rights reserved.
 * Modified by Stephen Reaves
 * Date: 30 November 2016
 * 
 * This is the implementation file for one precinct in a simulation.
 * An instance of this class will be used to track voters in line,
 * voters who have already voted, voters currently voting, and other
 * data structures related to the availability of voting stations.
 *
 * The main purpose of this class is to create voters within the 
 * precinct and compute statistics based on those voters.
 * This class first reads data through the ReadData() function,
 * then creates the voters based on that data using the CreateVoters()
 * function.  Then this class will run the simulation, calculate
 * mean wait time and other statistics and finally output those statistics.
 *
**/

static const string kTag = "OnePct: ";

/****************************************************************
 * Constructor.
**/
OnePct::OnePct(Scanner& infile) {
  this->ReadData(infile);
}

/****************************************************************
 * Accessors and mutators.
**/

/****************************************************************
 * Accessor: GetExpectedVoters
 * Written by Stephen Reaves {
 * This simple accessor just returns 'pct_expected_voters_'.
 * } endReaves
**/
int OnePct::GetExpectedVoters() const {
  return pct_expected_voters_;
}

/****************************************************************
 * Accessor: GetPctNumber
 * Written by Stephen Reaves {
 * This simple accessor just returns 'pct_number_'.
 * } endReaves
**/
int OnePct::GetPctNumber() const {
  return pct_number_;
}

/****************************************************************
* General functions.
**/
/******************************************************************************
 * Function: ComputeMeanAndDev
 * Written by Stephen Reaves {
 * This function will compute both the mean and standard deviation of the wait
 * times.  
 *
 * The mean is found by adding up all the wait times and dividing that total
 * by the number of expected voters.
 *
 * The standard deviation can then be computed by finding the difference
 * between every time and the mean, squaring that number to get 'this_addin'.
 * Then this code will find 'this_addin' for all of the wait times and add all
 * of the 'this_addin's together.  This number divided by the number of
 * expected voters will give us the variance.  The standard deviation will then
 * be the square root of the variance.
 *
 *      std dev = sqrt((sum(mean - individual time)^2)/number of voters)
 * 
 * NOTE:: Standard Deviation can be calculated either by dividing by the number
 *        in the sample ('pct_expected_voters_' in this case) or by dividing
 *        by the number in the sample minus one.  For simplicity's sake, we
 *        divided by just the number in the sample.  For more reading on this
 *        search for "Bessel's Correction".
 *
 * } endReaves
**/
void OnePct::ComputeMeanAndDev() {
  int sum_of_wait_times_seconds = 0;
  double sum_of_adjusted_times_seconds = 0.0;
  sum_of_wait_times_seconds = 0;
// Start computing mean
  multimap<int, OneVoter>::iterator iter_multimap;
  for (iter_multimap = voters_done_voting_.begin();
       iter_multimap != voters_done_voting_.end(); ++iter_multimap) {
    OneVoter voter = iter_multimap->second;
    sum_of_wait_times_seconds += voter.GetTimeWaiting();
  }
  wait_mean_seconds_ = static_cast<double>(sum_of_wait_times_seconds)/
  static_cast<double>(pct_expected_voters_);
// Finish computing mean

// Start computing Standard Deviation
  sum_of_adjusted_times_seconds = 0.0;
  for (iter_multimap = voters_done_voting_.begin();
       iter_multimap != voters_done_voting_.end(); ++iter_multimap) {
    OneVoter voter = iter_multimap->second;
    double this_addin = static_cast<double>(voter.GetTimeWaiting())
                      - wait_mean_seconds_;

    sum_of_adjusted_times_seconds += (this_addin) * (this_addin);
  }
  wait_dev_seconds_ = sqrt(sum_of_adjusted_times_seconds /
  static_cast<double>(pct_expected_voters_));
// Finished computing Standart Deviation
}

/****************************************************************
 * Function: CreateVoters
 * Written by Stephen Reaves {
 * This function responsible for poplulating the multimaps with voters on whom
 * the simulation will be run.  This function does so in two parts, one for the
 * number of voters at time zero (when the polls open), and one for the voters
 * that come in as the day progresses.
 *
 * The first part simply creates instances of the 'OneVoter' class one by
 * one.  Each voter will have the necessary times for it's member variables
 * based on the configuration file and a randomly generated time for the
 * 'duration' of the vote.
 * 
 * The second part is responsible for creating the voters that are not
 * present as soon as the precint opens, but rather trickle in throughout the
 * day.  This part is handled with nested for loops.  The outer loop simulates
 * the hours in the day as they pass while the inner loop creates voters
 * similarly to how the first part creates voters.
 * } endReaves
**/
void OnePct::CreateVoters(const Configuration& config, MyRandom& random,
                          ofstream& out_stream) {
// Setup
  int duration = 0;
  int arrival = 0;
  int sequence = 0;
  double percent = 0.0;
  string outstring = "XX";

  voters_backup_.clear();
  sequence = 0;

// Part 1: Voters at time zero
  percent = config.arrival_zero_;
  int voters_at_zero = round((percent / 100.0) * pct_expected_voters_);
  arrival = 0;
  for (int voter = 0; voter < voters_at_zero; ++voter) {
    int durationsub = random.RandomUniformInt(0, config.GetMaxServiceSubscript());
    duration = config.actual_service_times_.at(durationsub);
    OneVoter one_voter(sequence, arrival, duration);
    voters_backup_.insert(std::pair<int, OneVoter>(arrival, one_voter));
    ++sequence;
  }

// Part 2: Voters that come in as the day progresses.
  for (int hour = 0; hour < config.election_day_length_hours_; ++hour) {
    percent = config.arrival_fractions_.at(hour);
    int voters_this_hour = round((percent / 100.0) * pct_expected_voters_);
    if (0 == hour%2) 
      ++voters_this_hour;

    int arrival = hour*3600; //convert time from hours to seconds
    for(int voter = 0; voter < voters_this_hour; ++voter) {
      double lambda = static_cast<double>(voters_this_hour / 3600.0);
      int interarrival = random.RandomExponentialInt(lambda);
      arrival += interarrival;
      int durationsub = random.RandomUniformInt(0, config.GetMaxServiceSubscript());
      duration = config.actual_service_times_.at(durationsub);

      OneVoter one_voter(sequence, arrival, duration);
      voters_backup_.insert(std::pair<int, OneVoter>(arrival, one_voter));
      ++sequence;
    }
  }
}

/******************************************************************************
 * Function: DoStatistics
 * Written by Stephen Reaves {
 * This function gathers the data necessary to compute the statistics.
 * Naturally, it then computes the statistics.  This function checks to see if
 * the wait times are too long ("too long" is defined by the config file), then
 * dumps the necessary information into the ComputeMeanAndDev() function.
 * Finally, this function wraps all of the statistics in an 'outstring' to send
 * to the ToString() function.
 *
 * RETURNS: toolongcount -> How long the voter had to wait over the alotted
 *                          time. Zero if the voter did not have to wait too
 *                          long.
 *
 * } endReaves
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

/******************************************************************************
 * Function: ReadData
 * Written by Stephen Reaves
 * This function simply gets all of the necessary 'OnePct' variables from an 
 * 'infile'.  This function also inserts three stations into the set 
 * 'stations_to_histo_'.
 * } endReaves
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

/******************************************************************************
 * Function: RunSimulationPct
 * Written by Stephen Reaves {
 * This function is a wrapper for the 'RunSimulationPct2' fucntion, which does
 * most of the work in this file. This function takes the multimaps of voters 
 * and simulates each voter voting.  This function calls the 'CreateVoters' 
 * function to create the voters, then calls 'RunSimulationPct2' to simulate
 * the vote.  This function will then call 'DoStatistics' to compute the 
 * statistics.  Finally, this function will then call the 'ToString' fucntion
 * to string-ify the results and send them to the outstream.
 * 
 * PARAMS: config -> the configuration file that everything is based on.
 *         random -> a psuedo-random number to get psuedo-real results. 
 *         out_stream -> an ofstream to send the output to.
 * } endReaves
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
      this->CreateVoters(config, random, out_stream);

      voters_pending_ = voters_backup_;
      voters_voting_.clear();
      voters_done_voting_.clear();

      this->RunSimulationPct2(stations_count);
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

/******************************************************************************
 * Function: RunSimulationPct2
 * Written by Stephen Reaves {
 * This function simulates the actual voting process for an individual voter.
 * Each voter starts in 'voters_pending_', then a voting station is assigned to
 * the voter and they are moved into 'voters_voting_'.  After the voter is done
 * voting, they are moved to 'voters_done_voting' and the station is freed.
 * By note allowing multiple voters in one station, this function also handles
 * timing of the stations.
 *
 * The function is responsible for deciding if there is any extra wait time
 * added to a voter due to a lack of stations, and if that time is longer than
 * what is deemed acceptable by the configuration file.
 * } endReaves
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
 * Function: ToString()
 * Written by Stephen Reaves {
 * This is function will string-ify the 'OnePct' class with the necessary
 * formatting.
 * } endReaves
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
 * Function: ToStringVoterMap
 * Written by Stephen Reaves {
 * This function will string-ify a multimap with the necessary formatting.
 *
 * PARAMS: string label -> Some identifying name for the multimap.
 *         multimap<int, OneVoter> themap -> the map to be string-ified.
 * } endReaves
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
} // string OnePct::ToStringVoterMap()
