/****************************************************************
 * Header for the 'Simulation' class. 
 * A simulation creates precincts 
 * and voters within those precincts and simulates
 * voting times and other relevant details (waiting time, voting 
 * station, precinct details). 
 *
 * Author/copyright:  Duncan Buell
 * Modified by: Project 7 Group
 * Date: 1 December 2016
 *
 **/

#ifndef SIMULATION_H
#define SIMULATION_H

#include <map>

#include "../Utilities/utils.h"
#include "../Utilities/scanner.h"
#include "../Utilities/scanline.h"

using namespace std;

#include "configuration.h"
#include "onepct.h"

class Simulation
{
public:
  /****************************************************************
   * Constructors and destructors for the class.
   **/
  Simulation() = default;
  virtual ~Simulation() = default;

  /****************************************************************
   * General functions to begin the simulation based on a random 
   * seed paramter and using the configuration class and ofstream
   * ToString() and ToStringPcts() format output for precincts
   **/
  void ReadPrecincts(Scanner& infile);
  void RunSimulation(const Configuration& config,
                     MyRandom& random, ofstream& out_stream);
  string ToString();
  string ToStringPcts();

private:
  /****************************************************************
   * Variables, a map of all voter precincts. 
   **/
  map<int, OnePct> pcts_;

  /****************************************************************
   * Private functions.
   **/
};

#endif // SIMULATION_H
