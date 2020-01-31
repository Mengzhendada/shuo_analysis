#include <ROOT/RDataFrame.hxx>
#include <array>
#include <iostream>

void good_current() {
  // number of parallel threads
  constexpr const unsigned numThreads = 12;
  ROOT::EnableImplicitMT(numThreads);

  // get dataframes
  ROOT::RDataFrame d_scaler{"TSP",
                            "ROOTfiles/full/"
                            "coin_replay_production_7419_-1.root"};
  ROOT::RDataFrame d_events{"T",
                            "ROOTfiles/full/"
                            "coin_replay_production_7419_-1.root"};

  // get your max current
  auto h_current =
      d_scaler.Histo1D({"current", "", 1000, 10, 100}, "P.BCM4B.scalerCurrent");
  const double target_current =
      h_current->GetBinCenter(h_current->GetMaximumBin());
  const double current_tolerance = 3;
  h_current->DrawClone();
  gPad->Print("current.pdf");
  std::cout << "Most prevalent current in this run is: " << target_current
            << "uA\n";
  std::cout << "Setting tolerance to to +/- " << current_tolerance << "uA\n";
  // define your flags for desired runs
  auto d_scaler_good = d_scaler.Define(
      "has_good_current",
      [=](double current) {
        return (fabs(current - target_current) < current_tolerance);
      },
      {"P.BCM4B.scalerCurrent"});
  // get your columns as a pointer to a vector
  auto has_good_current_list = d_scaler_good.Take<bool>("has_good_current");
  auto scaler_event_list = d_scaler_good.Take<double>("evNumber");

  // Now make trigger the loop over the scaler data before we continue
  // by trying to access a variable
  std::cout << "We processed: " << has_good_current_list->size()
            << " scaler events\n";

  // OK, now we will process all results in parallel in (up to) 12 threads,
  // so we need to keep count of 12 indices that address our scaler data
  std::array<unsigned, numThreads> scaler_idx = {0};
  int i = 0;
  // let's define a function that returns true if the event has a good current,
  // using our index array. The first function parameter is the "slot" (thread)
  // that this chunk of the file is being processed in.
  // the lambda syntax [&] means we capture all objects in the current scope by
  // reference
  auto has_good_current = [&](unsigned slot, unsigned int eventNum) {
    unsigned idx = scaler_idx[slot];
    // lookup the correct position in the array, if we are not at the end
    // yet
    while (idx < (scaler_event_list->size() - 1) &&
           eventNum > scaler_event_list->at(idx)) {
      ++idx;
      ++i;
      cout<<"i:"<<i<<endl;
    }
    scaler_idx[slot] = idx;
    // found the correct index!
    return (has_good_current_list->at(idx) == true);
  };
  

  auto d_events_good =
      d_events.DefineSlot("good_current_flag", has_good_current, {"fEvtHdr.fEvtNum"})
          .Filter("good_current_flag == true");

  auto n_events = d_events.Count();
  auto n_events_good = d_events_good.Count();

  // now make histograms etc with your dataframe!
  auto ntrack0 = d_events_good.Histo1D({"ntrack0", "ntrack0", 100, -5., 50.},
                                       "P.dc.ntrack");
  //cout<<"i "<<i<<endl;
  // End of lazy evaluation
  std::cout << "#events: " << *n_events << ", #good events: " << *n_events_good
            << " (" << (double)*n_events_good / *n_events * 100. << "%)\n";

  TCanvas* ntrack =
      new TCanvas("c", "c", 1900, 1000);
  ntrack0->DrawClone();
  ntrack->Print("test.pdf");
}
