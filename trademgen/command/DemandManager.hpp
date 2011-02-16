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
#include <trademgen/bom/DemandStreamKey.hpp>

// Forward declarations
namespace stdair {
  class BomRoot;
  struct EventQueue;
}

namespace TRADEMGEN {

  // Forward declarations
  struct DemandDistribution;
  struct DemandStruct;
  class DemandStream;
  namespace DemandParserHelper {
    struct doEndDemand;
  }

  /**
   * @brief Utility class for Demand and DemandStream objects.
   */
  class DemandManager : public stdair::CmdAbstract {
    friend struct DemandParserHelper::doEndDemand;
    friend class TRADEMGEN_Service;
    
  private:
    // //////// Business methodes //////////
    /**
     * Create an EventQueue object instance, and add it to the BOM
     * tree.
     *
     * @param stdair::BomRoot& Reference on the top of the BOM tree.
     */
    static stdair::EventQueue& initEventQueue (stdair::BomRoot&);

    /**
     * Retrieve the EventQueue object instance.
     *
     * @param stdair::BomRoot& Reference on the top of the BOM tree.
     */
    static stdair::EventQueue& getEventQueue (const stdair::BomRoot&);

    /**
     * Generate the Demand objects corresponding to the given
     * Flight-Period, and add them to the given BomRoot.
     */
    static void createDemandCharacteristics (stdair::BomRoot&,
                                             stdair::UniformGenerator_T&,
                                             const POSProbabilityMass_T&,
                                             const DemandStruct&);

    /**
     * Generate the random seed for the demand characteristic
     * distributions.
     */
    static stdair::RandomSeed_T generateSeed (stdair::UniformGenerator_T&);

    /**
     * Create a demand stream object and it into the BOM tree.
     *
     * <br>That method returns the expected number of events to be
     * generated by that demand stream. That number is expected, not
     * actual, because it corresponds to an average. The actual number
     * (of events to be generated) is then randomly drawn, according
     * to a normal distribution following the parameters (mean and
     * standard deviation) given as parameter within the
     * DemandDistribution structure.
     *
     * @param stdair::BomRoot& Reference on the top of the BOM tree.
     * @param const DemandStreamKey& A string identifying uniquely the
     *   demand stream (e.g., "SIN-HND 2010-Feb-08 Y").
     * @param const DemandDistribution& Parameters (mean, standard
     *   deviation) of the probability distribution for the demand
     *   generation.
     * @return DemandStream& The newly created DemandStream object.
     */
    static DemandStream&
    createDemandStream (stdair::BomRoot&,
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
    
    /**
     * Get the total number of requests to be generated by the demand
     * stream, for which the key is given as parameter.
     *
     * <br>That method returns the expected number of events to be
     * generated.
     *
     * @param stdair::BomRoot& Reference on the top of the BOM tree.
     * @param const DemandStreamKey& A string identifying uniquely the
     *   demand stream (e.g., "SIN-HND 2010-Feb-08 Y").
     * @return stdair::NbOfRequests_T Expected number of events to be
     *   generated for that demand stream.
    static const stdair::NbOfRequests_T&
    getTotalNumberOfRequestsToBeGenerated (const stdair::BomRoot&,
                                           const stdair::DemandStreamKeyStr_T&);
     */

    /**
     * Get the total number of requests to be generated by all the
     * demand streams.
     *
     * <br>That method returns the expected number of events to be
     * generated.
     *
     * @param stdair::BomRoot& Reference on the top of the BOM tree.
     * @return stdair::NbOfRequests_T Expected number of events to be
     *   generated for that demand stream.
     */
    static stdair::NbOfRequests_T
    getTotalNumberOfRequestsToBeGenerated (const stdair::BomRoot&);

    /**
     * State whether there are still events to be generated for
     * the demand stream, for which the key is given as parameter.
     *
     * @param stdair::BomRoot& Reference on the top of the BOM tree.
     * @param const DemandStreamKey& A string identifying uniquely the
     *   demand stream (e.g., "SIN-HND 2010-Feb-08 Y").
     * @return bool Whether or not there are still some events to be
     *   generated.
     */
    static const bool
    stillHavingRequestsToBeGenerated (const stdair::BomRoot&,
                                      const stdair::DemandStreamKeyStr_T&);

    /**
     * Generate the first event/booking request for every demand
     * stream.
     *
     * @param stdair::BomRoot& Reference on the top of the BOM tree.
     * @return stdair::Count_T The actual total number of events to
     *         be generated, for all the demand stream.
     */
    static stdair::Count_T generateFirstRequests (const stdair::BomRoot&);

    /**
     * Generate a request with the demand stream, for which the key is
     * given as parameter.
     *
     * @param stdair::BomRoot& Reference on the top of the BOM tree.
     * @param const DemandStreamKey& A string identifying uniquely the
     *   demand stream (e.g., "SIN-HND 2010-Feb-08 Y").
     * @return stdair::BookingRequestPtr_T (Boost) shared pointer on
     *   the booking request structure, which has just been created.
     */
    static stdair::BookingRequestPtr_T
    generateNextRequest (const stdair::BomRoot&,
                         const stdair::DemandStreamKeyStr_T&);

    /**
     * Reset the context of the demand streams for another demand
     * generation without having to reparse the demand input file.
     *
     * @param stdair::BomRoot& Reference on the top of the BOM tree.
     */
    static void reset (const stdair::BomRoot&);
  };

}
#endif // __TRADEMGEN_CMD_DEMANDMANAGER_HPP
