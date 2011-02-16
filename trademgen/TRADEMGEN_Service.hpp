#ifndef __TRADEMGEN_TRADEMGEN_SERVICE_HPP
#define __TRADEMGEN_TRADEMGEN_SERVICE_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// StdAir
#include <stdair/stdair_basic_types.hpp>
#include <stdair/stdair_demand_types.hpp>
#include <stdair/stdair_service_types.hpp>
#include <stdair/bom/BookingRequestTypes.hpp>
#include <stdair/bom/EventTypes.hpp>
// TraDemGen
#include <trademgen/TRADEMGEN_Types.hpp>

// Forward declarations
namespace stdair {
  struct BasLogParams;
  struct BasDBParams;
  struct BookingRequestStruct;
  struct DemandCharacteristics;
  struct DemandDistribution;
  class EventQueue;
  struct EventStruct;
}

namespace TRADEMGEN {

  // Forward declaration
  class TRADEMGEN_ServiceContext;
  struct DemandStreamKey;
  
  /**
   * @brief class holding the services related to Travel Demand Generation.
   */
  class TRADEMGEN_Service {
  public:
    // ////////////////// Constructors and Destructors //////////////////    
    /**
     * @brief Constructor.
     *
     * The init() method is called; see the corresponding documentation
     * for more details.
     * <br>A reference on an output stream is given, so that log
     * outputs can be directed onto that stream.
     * <br>Moreover, database connection parameters are given, so that a
     * session can be created on the corresponding database.
     *
     * @param const stdair::BasLogParams& Parameters for the output log stream.
     * @param const stdair::BasDBParams& Parameters for the database access.
     * @param const stdair::Filename_T& Filename of the input demand file.
     */
    TRADEMGEN_Service (const stdair::BasLogParams&, const stdair::BasDBParams&,
                       const stdair::Filename_T& iDemandInputFilename);

    /**
     * Constructor.
     *
     * The init() method is called; see the corresponding documentation
     * for more details.
     * <br>A reference on an output stream is given, so that log
     * outputs can be directed onto that stream.
     *
     * @param const stdair::BasLogParams& Parameters for the output log stream.
     * @param const stdair::Filename_T& Filename of the input demand file.
     */
    TRADEMGEN_Service (const stdair::BasLogParams&,
                       const stdair::Filename_T& iDemandInputFilename);

    /**
     * Constructor.
     *
     * The init() method is called; see the corresponding documentation
     * for more details.
     * <br>Moreover, as no reference on any output stream is given,
     * neither any database access parameter is given, it is assumed
     * that the StdAir log service has already been initialised with
     * the proper log output stream by some other methods in the
     * calling chain (for instance, when the TRADEMGEN_Service is
     * itself being initialised by another library service such as
     * SIMCRS_Service).
     *
     * @param stdair::STDAIR_ServicePtr_T Handler on the STDAIR_Service.
     * @param const stdair::Filename_T& Filename of the input demand file.
     */
    TRADEMGEN_Service (stdair::STDAIR_ServicePtr_T,
                       const stdair::Filename_T& iDemandInputFilename);
    
    /**
     * Destructor.
     */
    ~TRADEMGEN_Service();
    

    // ////////////////// Business support methods //////////////////    
    /**
     * Display the list of airlines, as held within the sample database.
     */
    void displayAirlineListFromDB() const;

    /**
     * Get the expected number of events/requests to be generated for
     * all the demand streams.
     *
     * The getExpectedTotalNbOfEvents() method is called on the
     * underlying EventQueue object, which keeps track of that number.
     *
     * \note That number usually corresponds to an expectation (i.e.,
     *   the mean value of a random distribution). The actual number
     *   will be drawn when calling the generateNextRequest() method.
     *
     * @return const stdair::Count_T& Expected number of events to be
     *   generated.
     */
    const stdair::Count_T& getTotalNumberOfRequestsToBeGenerated() const;

    /**
     * Check whether enough requests have already been generated for
     * the demand stream which corresponds to the given key.
     *
     * @param const DemandStreamKey& A string identifying uniquely the
     *   demand stream (e.g., "SIN-HND 2010-Feb-08 Y").
     * @return bool Whether or not there are still events to be
     *   generated for that demand stream.
     */
    const bool
    stillHavingRequestsToBeGenerated(const stdair::DemandStreamKeyStr_T&) const;

    /**
     * Browse the list of demand streams and generate the first
     * request of each stream.
     *
     * @return stdair::Count_T The expected total number of events to
     *         be generated
     */
    stdair::Count_T generateFirstRequests() const;

    /**
     * Generate a request with the demand stream which corresponds to
     * the given key.
     *
     * @param const DemandStreamKey& A string identifying uniquely the
     *   demand stream (e.g., "SIN-HND 2010-Feb-08 Y").
     * @return stdair::BookingRequestPtr_T (Boost) shared pointer on
     *   the booking request structure, which has just been created.
     */
    stdair::BookingRequestPtr_T
    generateNextRequest (const stdair::DemandStreamKeyStr_T&) const;

    /**
     * Pop the next coming (in time) event, and remove it from the
     * event queue.
     * <ul>
     *   <li>The next coming (in time) event corresponds to the event
     *     having the earliest date-time stamp. In other words, it is
     *     the first/front element of the event queue.</li>
     *   <li>That (first) event/element is then removed from the event
     *     queue</li>
     *   <li>The progress status is updated for the corresponding
     *     demand stream.</li>
     * </ul>
     *
     * @return stdair::EventStruct A copy of the event structure,
     *   which comes first in time from within the event queue.
     */
    stdair::EventStruct popEvent() const;

    /**
     * States whether the event queue has reached the end.
     *
     * For now, that method states whether the event queue is empty.
     */
    bool isQueueDone() const;

    /**
     * Reset the context of the demand streams for another demand generation
     * without having to reparse the demand input file.
     */
    void reset() const;
    
    
  private:
    // ////////////////// Constructors and Destructors //////////////////    
    /**
     * Default Constructors, which must not be used.
     */
    TRADEMGEN_Service();
    
    /**
     * Default copy constructor.
     */
    TRADEMGEN_Service (const TRADEMGEN_Service&);

    /**
     * Initialise the (TRADEMGEN) service context (i.e., the
     * TRADEMGEN_ServiceContext object).
     */
    void initServiceContext ();

    /**
     * Initialise the STDAIR service (including the log service).
     *
     * A reference on the root of the BOM tree, namely the BomRoot object,
     * is stored within the service context for later use.
     *
     * @param const stdair::BasLogParams& Parameters for the output
     *   log stream.
     * @param const stdair::BasDBParams& Parameters for the database
     *   access.
     */
    void initStdAirService (const stdair::BasLogParams&,
                            const stdair::BasDBParams&);
    
    /**
     * Initialise the STDAIR service (including the log service).
     * <br>A reference on the root of the BOM tree, namely the BomRoot object,
     * is stored within the service context for later use.
     * @param const stdair::BasLogParams& Parameters for the output log stream.
     */
    void initStdAirService (const stdair::BasLogParams&);
    
    /**
     * Initialise.
     * <br>The CSV file, describing the characteristics of the demand for the
     * simulator, is parsed and the demand streams are generated accordingly.
     * @param const stdair::Filename_T& Filename of the input demand file.
     */
    void init (const stdair::Filename_T& iDemandInputFilename);
    
    /**
     * Finalise.
     */
    void finalise();

    
  private:
    // ///////// Service Context /////////
    /**
     * Trademgen context.
     */
    TRADEMGEN_ServiceContext* _trademgenServiceContext;
  };

}
#endif // __TRADEMGEN_TRADEMGEN_SERVICE_HPP
