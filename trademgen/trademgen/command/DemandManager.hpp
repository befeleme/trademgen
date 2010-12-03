#ifndef __TRADEMGEN_CMD_DEMANDMANAGER_HPP
#define __TRADEMGEN_CMD_DEMANDMANAGER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// StdAir
#include <stdair/stdair_basic_types.hpp>
#include <stdair/bom/BookingRequestTypes.hpp>
#include <stdair/command/CmdAbstract.hpp>
// TraDemGen
#include <trademgen/TRADEMGEN_Types.hpp>
#include <trademgen/basic/DemandCharacteristicTypes.hpp>

// Forward declarations
namespace stdair {
  class BomRoot;
  struct EventQueue;
}

namespace TRADEMGEN {

  // Forward declarations
  struct DemandCharacteristics;
  struct DemandDistribution;

  /** Utility class for Demand and DemandStream objects. */
  class DemandManager : public stdair::CmdAbstract {
  public:
    // //////// Business methodes //////////
    /** Add a demand stream into the context. */
    static void addDemandStream (stdair::BomRoot&,
                                 const DemandStreamKey&,
                                 const ArrivalPatternCumulativeDistribution_T&,
                                 const POSProbabilityMassFunction_T&,
                                 const ChannelProbabilityMassFunction_T&,
                                 const TripTypeProbabilityMassFunction_T&,
                                 const StayDurationProbabilityMassFunction_T&,
                                 const FrequentFlyerProbabilityMassFunction_T&,
                                 const PreferredDepartureTimeContinuousDistribution_T&,
                                 const stdair::WTP_T&,
                                 const ValueOfTimeContinuousDistribution_T&,
                                 const DemandDistribution&,
                                 const stdair::RandomSeed_T&,
                                 const stdair::RandomSeed_T&,
                                 const stdair::RandomSeed_T&,
                                 stdair::UniformGenerator_T&,
                                 const POSProbabilityMass_T&);
    
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
