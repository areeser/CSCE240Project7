<<<<<<< HEAD
sdf;oiajsdf ;oijasdf ija
=======
#include "configuration.h"
/****************************************************************
 * Implementation for the 'Configuration' class.
 *
 * Author/copyright:  Duncan Buell. All rights reserved.
 * Date: 6 October 2016
 **/

static const string kTag = "CONFIG: ";

/****************************************************************
 * Constructor.
 **/
Configuration::Configuration() {
}

/****************************************************************
 * Destructor.
 **/
Configuration::~Configuration() {
}

/****************************************************************
 * Accessors and mutators.
 **/
/****************************************************************
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
  * TODO service_times_file and actual_service_times_
  **/
void Configuration::ReadConfiguration(Scanner& instream) {
  /*
 
   */
  string line;
  ScanLine scanline;

  line = instream.NextLine();
  scanline.OpenString(line);
  seed_ = scanline.NextInt();
  election_day_length_hours_ = scanline.NextInt();
  election_day_length_seconds_ = election_day_length_hours_ * 3600;
  time_to_vote_mean_seconds_ = scanline.NextInt();
  min_expected_to_simulate_ = scanline.NextInt();
  max_expected_to_simulate_ = scanline.NextInt();
  wait_time_minutes_that_is_too_long_ = scanline.NextInt();
  number_of_iterations_ = scanline.NextInt();

  line = instream.NextLine();
  scanline.OpenString(line);
  arrival_zero_ = scanline.NextDouble();
  for (int sub = 0; sub < election_day_length_hours_; ++sub) {
    double input = scanline.NextDouble();
    arrival_fractions_.push_back(input);
  }

  Scanner service_times_file;
  service_times_file.OpenFile("dataallsorted.txt");
  while (service_times_file.HasNext()) {
    int thetime = service_times_file.NextInt();
    actual_service_times_.push_back(thetime);
  }
}

/****************************************************************
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
>>>>>>> refs/remotes/origin/master