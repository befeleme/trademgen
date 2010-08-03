#ifndef __TRADEMGEN_CMD_DEMANDMANAGER_HPP
#define __TRADEMGEN_CMD_DEMANDMANAGER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// StdAir
#include <stdair/STDAIR_Types.hpp>
#include <stdair/basic/DemandCharacteristicTypes.hpp>
#include <stdair/bom/DemandStreamTypes.hpp>
#include <stdair/bom/BookingRequestTypes.hpp>
#include <stdair/command/CmdAbstract.hpp>
// Trademgen
#include <trademgen/TRADEMGEN_Types.hpp>

// Forward declarations
namespace stdair {
  class BomRoot;
  struct DemandCharacteristics;
  struct DemandDistribution;
  struct EventQueue;
}

namespace TRADEMGEN {

  /** Utility class for Demand and DemandStream objects. */
  class DemandManager : public stdair::CmdAbstract {
  public:
    // //////// Business methodes //////////
    /** Add a demand stream into the context. */
    static void addDemandStream (stdair::BomRoot&,
                                 const stdair::DemandStreamKey_T&,
                                 const stdair::ArrivalPatternCumulativeDistribution_T&,
                                 const stdair::POSProbabilityMassFunction_T&,
                                 const stdair::ChannelProbabilityMassFunction_T&,
                                 const stdair::TripTypeProbabilityMassFunction_T&,
                                 const stdair::StayDurationProbabilityMassFunction_T&,
                                 const stdair::FrequentFlyerProbabilityMassFunction_T&,
                                 const stdair::PreferredDepartureTimeContinuousDistribution_T&,
                                 const stdair::WTPContinuousDistribution_T&,
                                 const stdair::ValueOfTimeContinuousDistribution_T&,
                                 const stdair::DemandDistribution&,
                                 const stdair::RandomSeed_T&,
                                 const stdair::RandomSeed_T&,
                                 const stdair::RandomSeed_T&);
    
    /** Get the total number of requests to be generated by the demand
        stream which corresponds to the given key. */
    static const stdair::NbOfRequests_T&
    getTotalNumberOfRequestsToBeGenerated (const stdair::BomRoot&,
                                           const stdair::DemandStreamKeyStr_T&);

    /** Check whether enough requests have already been generated for
        the demand stream which corresponds to the given key. */
    static const bool
    stillHavingRequestsToBeGenerated (const stdair::BomRoot&,
                                      const stdair::DemandStreamKeyStr_T&);

    /** Browse the list of demand streams and generate the first
        request of each stream. */
    static void generateFirstRequests (stdair::EventQueue&,
                                       const stdair::BomRoot&);

    /** Generate a request with the demand stream which corresponds to
        the given key. */
    static stdair::BookingRequestPtr_T
    generateNextRequest (const stdair::BomRoot&,
                         const stdair::DemandStreamKeyStr_T&);

    /** Reset the context of the demand streams for another demand generation
        without having to reparse the demand input file. */
    static void reset (const stdair::BomRoot&);
  };

}
#endif // __TRADEMGEN_CMD_DEMANDMANAGER_HPP
