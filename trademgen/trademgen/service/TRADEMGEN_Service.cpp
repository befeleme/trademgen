// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <sstream>
// Boost
#include <boost/make_shared.hpp>
// SOCI
#include <soci/core/soci.h>
#include <soci/backends/mysql/soci-mysql.h>
// StdAir
#include <stdair/basic/BasChronometer.hpp>
#include <stdair/basic/BasFileMgr.hpp>
#include <stdair/bom/BomManager.hpp> // for display()
#include <stdair/bom/BomRoot.hpp>
#include <stdair/bom/AirlineFeature.hpp>
#include <stdair/bom/AirlineFeatureSet.hpp>
#include <stdair/bom/BookingRequestStruct.hpp>
#include <stdair/service/Logger.hpp>
#include <stdair/STDAIR_Service.hpp>
// TraDemGen
#include <trademgen/basic/BasConst_TRADEMGEN_Service.hpp>
#include <trademgen/factory/FacTRADEMGENServiceContext.hpp>
#include <trademgen/service/TRADEMGEN_ServiceContext.hpp>
#include <trademgen/TRADEMGEN_Service.hpp>

namespace TRADEMGEN {

  // //////////////////////////////////////////////////////////////////////
  TRADEMGEN_Service::TRADEMGEN_Service ()
    : _trademgenServiceContext (NULL) {
    assert (false);
  }

  // //////////////////////////////////////////////////////////////////////
  TRADEMGEN_Service::TRADEMGEN_Service (const TRADEMGEN_Service& iService) 
    : _trademgenServiceContext (NULL) {
    assert (false);
  }

  // ////////////////////////////////////////////////////////////////////
  TRADEMGEN_Service::
  TRADEMGEN_Service (stdair::STDAIR_ServicePtr_T ioSTDAIR_ServicePtr,
                     const stdair::Filename_T& iDemandInputFilename)
    : _trademgenServiceContext (NULL) {

    // Initialise the service context
    initServiceContext ();
    
    // Retrieve the Trademgen service context
    assert (_trademgenServiceContext != NULL);
    TRADEMGEN_ServiceContext& lTRADEMGEN_ServiceContext =
      *_trademgenServiceContext;
    
    // Store the STDAIR service object within the (TRADEMGEN) service context
    lTRADEMGEN_ServiceContext.setSTDAIR_Service (ioSTDAIR_ServicePtr);
    
    // Initialise the context
    init (iDemandInputFilename);
  }

  // //////////////////////////////////////////////////////////////////////
  TRADEMGEN_Service::
  TRADEMGEN_Service (const stdair::BasLogParams& iLogParams,
                     const stdair::BasDBParams& iDBParams,
                     const stdair::AirlineFeatureSet& iAirlineFeatureSet,
                     const stdair::Filename_T& iDemandInputFilename)
    : _trademgenServiceContext (NULL) {
    
    // Initialise the service context
    initServiceContext ();
    
    // Initialise the STDAIR service handler
    initStdAirService (iLogParams, iDBParams, iAirlineFeatureSet);
    
    // Initialise the (remaining of the) context
    init (iDemandInputFilename);
  }

  // //////////////////////////////////////////////////////////////////////
  TRADEMGEN_Service::~TRADEMGEN_Service () {
  }

  // //////////////////////////////////////////////////////////////////////
  void TRADEMGEN_Service::initServiceContext () {
    // Initialise the service context
    TRADEMGEN_ServiceContext& lTRADEMGEN_ServiceContext = 
      FacTRADEMGENServiceContext::instance().create ();
    _trademgenServiceContext = &lTRADEMGEN_ServiceContext;
  }

  // //////////////////////////////////////////////////////////////////////
  void TRADEMGEN_Service::
  initStdAirService (const stdair::BasLogParams& iLogParams,
                     const stdair::BasDBParams& iDBParams,
                     const stdair::AirlineFeatureSet& iAirlineFeatureSet) {

    // Retrieve the Trademgen service context
    assert (_trademgenServiceContext != NULL);
    TRADEMGEN_ServiceContext& lTRADEMGEN_ServiceContext =
      *_trademgenServiceContext;
    
    // Initialise the STDAIR service handler
    // Note that the track on the object memory is kept thanks to the Boost
    // Smart Pointers component.
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr = 
      boost::make_shared<stdair::STDAIR_Service> (iLogParams, iDBParams);

    // Retrieve the root of the BOM tree, on which all of the other BOM objects
    // will be attached
    assert (lSTDAIR_Service_ptr != NULL);
    stdair::BomRoot& lBomRoot = lSTDAIR_Service_ptr->getBomRoot();

    // Set the AirlineFeatureSet for the BomRoot.
    lBomRoot.setAirlineFeatureSet (&iAirlineFeatureSet);
    
    // Store the STDAIR service object within the (TRADEMGEN) service context
    lTRADEMGEN_ServiceContext.setSTDAIR_Service (lSTDAIR_Service_ptr);
  }
  
  // //////////////////////////////////////////////////////////////////////
  void TRADEMGEN_Service::
  init (const stdair::Filename_T& iDemandInputFilename) {

    // Check that the file path given as input corresponds to an actual file
    const bool doesExistAndIsReadable =
      stdair::BasFileMgr::doesExistAndIsReadable (iDemandInputFilename);
    if (doesExistAndIsReadable == false) {
      STDAIR_LOG_ERROR ("The schedule input file, '" << iDemandInputFilename
                        << "', can not be retrieved on the file-system");
      throw stdair::FileNotFoundException();
    }

    // Retrieve the Trademgen service context
    assert (_trademgenServiceContext != NULL);
    TRADEMGEN_ServiceContext& lTRADEMGEN_ServiceContext =
      *_trademgenServiceContext;

    // Retrieve the StdAir service context
    stdair::STDAIR_Service& lSTDAIR_Service =
      lTRADEMGEN_ServiceContext.getSTDAIR_Service();
    
    // Get the root of the BOM tree, on which all of the other BOM objects
    // will be attached
    stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();

    // Initialise the demand generators
    // DemandGenerator::initialiseGenerators (iDemandInputFilename, lBomRoot);

    // DEBUG
    STDAIR_LOG_DEBUG ("Generated BomRoot:");
    std::ostringstream oStream;
    stdair::BomManager::display (oStream, lBomRoot);
    STDAIR_LOG_DEBUG (oStream.str());
  }
  
  // //////////////////////////////////////////////////////////////////////
  std::string TRADEMGEN_Service::calculateTrademgen () {
    std::ostringstream oStr;

    if (_trademgenServiceContext == NULL) {
      throw NonInitialisedServiceException();
    }
    assert (_trademgenServiceContext != NULL);
    //TRADEMGEN_ServiceContext& lTRADEMGEN_ServiceContext =
    //  *_trademgenServiceContext;

    // Get the date-time for the present time
    boost::posix_time::ptime lNowDateTime =
      boost::posix_time::second_clock::local_time();
    boost::gregorian::date lNowDate = lNowDateTime.date();

    // DEBUG
    STDAIR_LOG_DEBUG (std::endl
                      << "==================================================="
                      << std::endl
                      << lNowDateTime);

    try {
      
      // Delegate the query execution to the dedicated command
      stdair::BasChronometer lTrademgenChronometer;
      lTrademgenChronometer.start();

      //
      oStr << "That is my request: hello world!";

      const double lTrademgenMeasure = lTrademgenChronometer.elapsed();

      // DEBUG
      STDAIR_LOG_DEBUG ("Sample service for TraDemGen: " << lTrademgenMeasure);
      
    } catch (const std::exception& error) {
      STDAIR_LOG_ERROR ("Exception: "  << error.what());
      throw TrademgenCalculationException();
    }
  
    return oStr.str();
  }

  // ////////////////////////////////////////////////////////////////////
  stdair::BookingRequestStruct TRADEMGEN_Service::
  generateBookingRequest () const {
    
      // TODO: remove this hardcoded section
      // Hardcode a booking request in order to simulate a sale.
      // Departure airport code
      stdair::AirportCode_T lOrigin ("LHR");
      // Arrival airport code
      stdair::AirportCode_T lDestination ("JFK");
      // Departure date
      stdair::Date_T lDepartureDate (2010, 01, 19);
      // Passenger type
      stdair::PassengerType_T lPaxType ("L");
      // Number of passengers in the travelling group
      stdair::NbOfSeats_T lPartySize = 5;

      // Booking request
      return stdair::BookingRequestStruct (lOrigin, lDestination,
                                           lDepartureDate, lPaxType, lPartySize);
  }

}
