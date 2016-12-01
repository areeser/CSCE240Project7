#include "simulation.h"
/****************************************************************
 * Implementation for the 'Simulation' class.
 *
 * This voter simulation created voters for a number of voter
 * precincts on a simulated election day. This class contains
 * ReadPrecincts() to store all precincts in the pcts_ map.
 * Also contains RunSimulation() which goes through each precint
 * to store voter data and compute statistics for output. 
 * ToString() to format and output data for all precincts. 
 *
 * Author/copyright:  Duncan Buell
 * Modified by: Project 7 Group
 * Date: 1 December 2016
 *
 **/

static const string kTag = "SIM: ";

/****************************************************************
 * Accessors and mutators.
 **/

/****************************************************************
 * General functions.
 **/
/****************************************************************
 * Function: ReadPrecincts
 * Written by Alexander Reeser {
 * While the infile has more data, the function creates a new pct,
 * calls ReadData with the new pct, then stores that pct in the map
 * of pcts_. 
 * } endReeser
 **/
void Simulation::ReadPrecincts(Scanner& infile) {
  while (infile.HasNext()) {
    OnePct new_pct;
    new_pct.ReadData(infile);
    pcts_[new_pct.GetPctNumber()] = new_pct;
  } // while (infile.HasNext()) {
} // void Simulation::ReadPrecincts(Scanner& infile) {

/****************************************************************
 * Function RunSimulation
 * Written by Alexander Reeser {
 * The simulation iterates through the pcts_ map, setting the 
 * OnePct named pct to the OnePct stored in the current map location.
 * If the number of expected voters in the pct is less than the
 * min_expected_to_simulate_ or greater than the max_expected_to_simulate_
 * continue is called. Otherwise, the pct is converted to a string 
 * and sent to the Output. The pct count is then increased by one.
 * After the loop ends, the number of pcts in the simulation is
 * sent to the Output.
 * } endReeser
 **/
void Simulation::RunSimulation(const Configuration &config, MyRandom &random,
                               ofstream &out_stream) {
  string outstring = "XX";
  int pct_count_this_batch = 0;
  for (auto iterPct = pcts_.begin(); iterPct != pcts_.end(); ++iterPct) {
    OnePct pct = iterPct->second;

    int expected_voters = pct.GetExpectedVoters();
    if ((expected_voters <= config.min_expected_to_simulate_) ||
        (expected_voters > config.max_expected_to_simulate_))
      continue;

    outstring = kTag + "RunSimulation for pct " + "\n";
    outstring += kTag + pct.ToString() + "\n";
    Utils::Output(outstring, out_stream, Utils::log_stream);

    ++pct_count_this_batch;
    pct.RunSimulationPct(config, random, out_stream);

    //    break; // we only run one pct right now
  } // for(auto iterPct = pcts_.begin(); iterPct != pcts_.end(); ++iterPct)

  outstring = kTag + "PRECINCT COUNT THIS BATCH " +
              Utils::Format(pct_count_this_batch, 4) + "\n";
  //  Utils::Output(outstring, out_stream);
  Utils::Output(outstring, out_stream, Utils::log_stream);
  //  out_stream << outstring << endl;
  //  out_stream.flush();
  //  Utils::log_stream << outstring << endl;
  //  Utils::log_stream.flush();

} // void Simulation::RunSimulation()

/****************************************************************
 * Function ToString
 * Returns: a string containing all the pct information in the 
 *          pcts_ map.
 **/
string Simulation::ToString()
{
  string s = "";

  for (auto iterPct = pcts_.begin(); iterPct != pcts_.end(); ++iterPct) {
    s += kTag + (iterPct->second).ToString() + "\n";
  }

  return s;
} // string Simulation::ToString()
