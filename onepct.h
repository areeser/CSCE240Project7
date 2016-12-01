/****************************************************************
 * Header for the 'OnePct' class
 *
 * Author/copyright:  Duncan Buell
 * Modified by: Joe Regalbuto
 * Date: 28 November 2016
 *
 * This is a header file for one voter precinct in a simulation.
 * An instance of a precinct contains member variables to detail
 * general precinct information and expected voter turnouts.
 * A precinct also contains four private multi-maps that track
 * voters in line, voters currently voting, voters who have
 * voted, and other data structures related to the numbers of
 * available and total voting stations.
 *
 * General functions include a ToStringVoterMap() to format
 * output for various multi-maps. The ReadData() function
 * reads data for the setup of a precinct and relating to the
 * accompanying simulation. ReadSimulation() allows this
 * precinct to be included in a simulation based on the seed
 * that has been randomly generated.
 *
 * Other member variables are used in computing this precinct's
 * average wait time for each voter and the standard deviation
 * of waiting times. Also contains dummy constant variables.
 *
 * A 'precinct' also contains private functions to computer the
 * mean and standard deviation, create voters within the precinct,
 * and compute the other statistics, stored in member variables.
 *
**/

#ifndef ONEPCT_H
#define ONEPCT_H

#include <cmath>
#include <map>
#include <set>
#include <vector>

#include "../Utilities/utils.h"
#include "../Utilities/scanner.h"
#include "../Utilities/scanline.h"

using namespace std;

#include "configuration.h"
#include "myrandom.h"
#include "onevoter.h"

static const double kDummyDouble = -88.88;
static const int kDummyInt = -999;
static const string kDummyString = "dummystring";

class OnePct {
public:
/****************************************************************
 * Constructors and destructors for the class.
**/
 OnePct() = default;
 OnePct(Scanner& infile);
 virtual ~OnePct() = default;

/****************************************************************
 * Accessors to return the expected number of voters for a
 * precinct and a precint's number.
**/
  int GetExpectedVoters() const;
  int GetPctNumber() const;

/****************************************************************
 * General functions.
**/
  void ReadData(Scanner& infile);
  void RunSimulationPct(const Configuration& config, MyRandom& random, ofstream& out_stream);

  string ToString();
  string ToStringVoterMap(string label, multimap<int, OneVoter> themap);

private:
  int    pct_expected_voters_ = kDummyInt;
  int    pct_expected_per_hour_ = kDummyInt;
  double pct_minority_ = kDummyDouble;
  string pct_name_ = kDummyString;
  int    pct_number_ = kDummyInt;
  double pct_turnout_ = kDummyDouble;
  int    pct_stations_ = kDummyInt;
  int    pct_num_voters_ = kDummyInt;
  double wait_dev_seconds_;
  double wait_mean_seconds_;
  set<int> stations_to_histo_;
  vector<int> free_stations_;

  //multimaps used to dynamically store voters
  multimap<int, OneVoter> voters_backup_;
  multimap<int, OneVoter> voters_done_voting_;
  multimap<int, OneVoter> voters_pending_;
  multimap<int, OneVoter> voters_voting_;

/****************************************************************
 * General private functions.
**/
  void CreateVoters(const Configuration& config, MyRandom& random,
                    ofstream& out_stream);
  int DoStatistics(int iteration, const Configuration& config, int station_count,
                   map<int, int>& map_for_histo, ofstream& out_stream);

  void ComputeMeanAndDev();
  void RunSimulationPct2(int stations);

};

#endif // ONEPCT_H
