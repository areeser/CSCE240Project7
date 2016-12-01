#include "configuration.h"
/****************************************************************
 * Implementation for the 'Configuration' class.
 *
 * Author/copyright:  Duncan Buell. All rights reserved.
 * Modified by: Group 7 
 * Date: 1 December 2016
 *
 * This configuration sets up the voter simulation and passes a
 * scanner. ReadConfiguration() reads the input data and assigns 
 * the values for election day length, number of iterations, and 
 * the time it took to vote. The actual service times are then
 * pushed back onto the vector actual_service_times_.
 *
 **/

/* File tag for output purposes. (Consider using __FILE__?) */
static const string kTag = "CONFIG: ";

/****************************************************************
 * Accessors and mutators.
 **/
/****************************************************************
 * Return the maximum accessible subscript (_not_ the size) of
 * the service times.
 **/
int Configuration::GetMaxServiceSubscript() const {
  return static_cast<int>(actual_service_times_.size()) - 1;
}

/****************************************************************
 * General functions.
 **/
/****************************************************************
 **/
/****************************************************************
  * Function: ReadConfiguration
  * Takes in a Scanner
  *
  * Written by Alexander Reeser {
  *
  * The function reads a line which is opened with scanline.
  * The first int in the line is the seed. The second int is the
  * number of hours the voting stations are open. From this it
  * computes the number of seconds the stations are open. The
  * third int in the line is the average time someone takes to
  * vote. The fourth int is the minimum number of possible voters
  * in the simulation and the fifth is the maximum number of
  * voters in the simulation. The sixth int is the number of
  * someone can wait at the polls before the wait time is too long.
  * Seventh and final int is the number of times the simulation
  * will run.
  *
  * The next line of the file is then read. The first double
  * in the line is a zero arrival rate. A loop then iterates
  * through each hour the polls are open, reading in the
  * percentage of people who voted during that hour and pushing
  * them to the vector arrival_fractions_.
  *
  * Next, the file dataallsorted is opened with the Scanner
  * service_times_file. All the integers in the file are then
  * pushed onto the actual_service_times_ vector. These are the
  * times it takes costomers to vote.
  * } endAlexander
  * TODO are we supposed to error check the input? -- mbozzi
  **/
void Configuration::ReadConfiguration(Scanner& instream) {
  string line;
  ScanLine scanline;

  /* Each field read in here corresponds to the format of the configuration
     file, which is text-oriented.  See ./xconfig100zero.txt for an example
     configuration. */
  line = instream.NextLine();
  scanline.OpenString(line);

  /* Random number seed. */
  seed_ = scanline.NextInt();
  /* How long is one election day? */
  election_day_length_hours_   = scanline.NextInt();
  election_day_length_seconds_ = election_day_length_hours_ * 3600;

  /* Average (mean) time for one voter to vote */
  time_to_vote_mean_seconds_   = scanline.NextInt();

  min_expected_to_simulate_    = scanline.NextInt();
  max_expected_to_simulate_ = scanline.NextInt();
  wait_time_minutes_that_is_too_long_ = scanline.NextInt();

  /* run N simulations. */
  number_of_iterations_ = scanline.NextInt();

  /* Pull the second line from the configuration file. */
  line = instream.NextLine();
  scanline.OpenString(line);

  {
    /* All of the data read in this block should sum to ~100.0
       Careful w/ floating-point precision issues - see the single-line
       comments for an example way to test for approximate accuracy. */
    // double sum = 0.0;
    /* total percentage of voters who were waiting when the polls opened. */
    arrival_zero_ = scanline.NextDouble();
    // sum += arrival_zero_;
    /* Percentage of voters who arrive each hour, out of all voters. */
    for (int sub = 0; sub < election_day_length_hours_; ++sub) {
      double input = scanline.NextDouble();
      // sum += input;
      arrival_fractions_.push_back(input);
    }

    // constexpr auto epsilon = 1.0e-7; /* some relatively small value */
    /* Does everything sum to about 100%?  Use this hack for floating-point equality.
       (There's more correct ways to do this, but they are very complex). */
    // assert(std::abs (sum - 100.0) <= epsilon);
  }

  Scanner service_times_file;
  service_times_file.OpenFile("dataallsorted.txt");
  while (service_times_file.HasNext()) {
    int thetime = service_times_file.NextInt();
    actual_service_times_.push_back(thetime);
  }
}

/****************************************************************
 * Function: ToString
 * Returns: the string s and formats it nicely for user
 * 
 **/
string Configuration::ToString()
{
  string s = "\n";
  s += kTag;
  s += "RN seed:              ";
  s += Utils::Format(seed_, 8) + "\n";
  s += kTag;
  s += "Election Day length:  ";
  s += Utils::Format(election_day_length_seconds_, 8) + " =";
  s += Utils::Format(election_day_length_seconds_/3600.0, 8, 2) + " (";
  s += Utils::Format(election_day_length_hours_, 8, 2) + ") hours\n";
  s += kTag;
  s += "Time to vote mean:    ";
  s += Utils::Format(time_to_vote_mean_seconds_, 8) + " =";
  s += Utils::Format(time_to_vote_mean_seconds_/60.0, 8, 2) + " minutes\n";
  s += kTag;
  s += "Min and max expected voters for this simulation:     ";
  s += Utils::Format(min_expected_to_simulate_, 8);
  s += Utils::Format(max_expected_to_simulate_, 8) + "\n";
  s += "Wait time (minutes) that is 'too long': ";
  s += Utils::Format(wait_time_minutes_that_is_too_long_, 8) + "\n";
  s += "Number of iterations to perform: ";
  s += Utils::Format(number_of_iterations_, 4) + "\n";
  s += "Max service time subscript: ";
  s += Utils::Format(GetMaxServiceSubscript(), 6) + "\n";
  int offset = 6;
  s += kTag;
  s += Utils::Format(0, 2) + "-" + Utils::Format(0, 2);
  s += " : " + Utils::Format(arrival_zero_, 7, 2) + "\n";
  for (UINT sub = 0; sub < arrival_fractions_.size(); ++sub) {
    s += kTag;
    s += Utils::Format(offset+sub, 2) + "-" + Utils::Format(offset+sub+1, 2);
    s += " : " + Utils::Format(arrival_fractions_.at(sub), 7, 2) + "\n";
  }
  s += "\n";
  return s;
}
