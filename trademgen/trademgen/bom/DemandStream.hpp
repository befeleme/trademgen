#ifndef __TRADEMGEN_BOM_DEMANDSTREAM_HPP
#define __TRADEMGEN_BOM_DEMANDSTREAM_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// Boost
#include <boost/shared_ptr.hpp>
// STL
#include <iosfwd>
// STDAIR
#include <stdair/STDAIR_Types.hpp>
#include <stdair/bom/DemandStream.hpp>
#include <stdair/bom/BookingRequestTypes.hpp>

// Forward declarations.
namespace stdair {
  struct DemandCharacteristics;
  struct DemandDistribution;
  struct RandomGeneration;
  struct RandomGenerationContext;
}

namespace TRADEMGEN {
  
  /** Class modeling a demand stream. */
  class DemandStream  : public stdair::DemandStream {
    friend class TRADEMGEN_ServiceContext;
 
  private:
    // /////////////// Business Methods //////////////
    /** Check whether enough requests have already been generated. */
    static const bool stillHavingRequestsToBeGenerated (const stdair::DemandStream&);
    const bool stillHavingRequestsToBeGenerated () const;
    
    /** Generate the next request. */
    static stdair::BookingRequestPtr_T generateNextRequest (stdair::DemandStream&);
    stdair::BookingRequestPtr_T generateNextRequest ();


  private:
    // ////////// Constructors and destructors /////////
    /** Default constructors. */
    DemandStream ();
    DemandStream (const DemandStream&);
    DemandStream (const stdair::DemandStreamKey_T&,
                  const stdair::DemandCharacteristics&,
                  const stdair::DemandDistribution&, const stdair::RandomSeed_T&,
                  const stdair::RandomSeed_T&, const stdair::RandomSeed_T&);
    /** Destructor. */
    ~DemandStream ();

    
  private:
    // ////////// Attributes //////////
    /** Key */
    stdair::DemandStreamKey_T _key;
    
    /** Demand characteristics */
    stdair::DemandCharacteristics _demandCharacteristics;

    /** Demand distribution */
    stdair::DemandDistribution _demandDistribution;
    
    /** Total number of requests to be generated*/
    stdair::Count_T _totalNumberOfRequestsToBeGenerated;

    /** Random generation context */
    stdair::RandomGenerationContext _randomGenerationContext;
  
    /** Random generator for number of requests */
    stdair::RandomGeneration _numberOfRequestsRandomGenerator;
    
    /** Random generator for request datetime */
    stdair::RandomGeneration _requestDateTimeRandomGenerator;
    
    /** Random generator for demand characteristics */
    stdair::RandomGeneration _demandCharacteristicsRandomGenerator;
    
  };

}
#endif // __TRADEMGEN_BOM_DEMANDSTREAM_HPP
