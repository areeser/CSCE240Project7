#include "main.h"

static const string kTag = "MAIN: ";

/*******************************************************************************
12345678901234567890123456789012345678901234567890123456789012345678901234567890
 * Main program for Homework 6, the simulation.
 *
 * Author/copyright:  Duncan Buell. All rights reserved.
 * Date: 11 July 2016
 *
**/

int main(int argc, char *argv[]) {
  string in_filename = "";
  string out_filename = "";
  string log_filename = "";
  ofstream out_stream;

  Scanner in_stream;

  Queue queue;

  Utils::CheckArgs(3, argc, argv, "infilename outfilename logfilename");
  in_filename = static_cast<string>(argv[1]); // DO NOT USE OLD CAST!
  out_filename = static_cast<string>(argv[2]);
  log_filename = static_cast<string>(argv[3]);

  Utils::LogFileOpen(log_filename);

  Utils::log_stream << "Beginning execution\n";
  Utils::log_stream.flush();

  Utils::FileOpen(out_stream, out_filename);

  Utils::log_stream << "infile  '" << in_filename << "'\n";
  Utils::log_stream << "outfile '" << out_filename << "'\n";
  Utils::log_stream << "logfile '" << log_filename << "'\n";

  in_stream.OpenFile(in_filename);

  // We are going to read all the data for the 'queue' class.
  queue.ReadInputData(in_stream);

  // And then we echo that data here.
  out_stream << endl;
  out_stream << kTag << "Random number seed: "
                     << queue.GetRandomNumberSeed() << endl;

  out_stream << kTag << "# Voters: " << queue.GetHowManyVoters() << endl;
  out_stream << kTag << "# Time steps: "
                     << queue.GetNumberOfTimeSteps() << endl;
  out_stream << kTag << "# Machines: "
                     << queue.GetHowManyMachinesLowerBound() << " to "
                     << queue.GetHowManyMachinesUpperBound() << endl;
  out_stream << kTag << "Mean & dev vote times: "
                     << queue.GetVoteTimeMean() << " "
                     << queue.GetVoteTimeDev() << endl << endl;
  out_stream.flush();

  // Initialize the 'backup' list of voters.
  // We could have buried this in the 'RunSimulation' function, but
  // it also feels like something defined by the config data, so maybe
  // belongs for testing here at this level.
  queue.InitializeVoters();

  // Finally, we run the simulation.
  queue.RunSimulation(out_stream);

  out_stream.flush();

  return 0;
}

