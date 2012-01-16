// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// StdAir
#include <stdair/basic/ProgressStatusSet.hpp>
#include <stdair/bom/BomManager.hpp>
#include <stdair/bom/EventStruct.hpp>
#include <stdair/bom/BookingRequestStruct.hpp>
#include <stdair/bom/EventQueue.hpp>
#include <stdair/bom/TravelSolutionStruct.hpp>
#include <stdair/bom/CancellationStruct.hpp>
#include <stdair/factory/FacBom.hpp>
#include <stdair/factory/FacBomManager.hpp>
#include <stdair/service/Logger.hpp>
// SEvMgr
#include <sevmgr/SEVMGR_Service.hpp>
// TraDemGen
#include <trademgen/basic/DemandCharacteristics.hpp>
#include <trademgen/basic/DemandDistribution.hpp>
#include <trademgen/bom/DemandStruct.hpp>
#include <trademgen/bom/DemandStream.hpp>
#include <trademgen/command/DemandManager.hpp>

namespace TRADEMGEN {

  // //////////////////////////////////////////////////////////////////////
  void DemandManager::
  buildSampleBomStd (stdair::EventQueue& ioEventQueue,
                  stdair::RandomGeneration& ioSharedGenerator,
                  const POSProbabilityMass_T& iPOSProbMass) {

    // Key of the demand stream
    const stdair::AirportCode_T lOrigin ("SIN");
    const stdair::AirportCode_T lDestination ("BKK");
    const stdair::Date_T lDepDate (2011, 2, 14);
    const stdair::CabinCode_T lCabin ("Y");

    //
    const DemandStreamKey lDemandStreamKey (lOrigin, lDestination, lDepDate,
                                            lCabin);

    // DEBUG
    // STDAIR_LOG_DEBUG ("Demand stream key: " << lDemandStreamKey.describe());
    
    // Distribution for the number of requests
    const stdair::MeanValue_T lDemandMean (10.0);
    const stdair::StdDevValue_T lDemandStdDev (1.0);
    const DemandDistribution lDemandDistribution (lDemandMean, lDemandStdDev);
    
    // Seed
    const stdair::RandomSeed_T& lRequestDateTimeSeed =
      generateSeed (ioSharedGenerator);
    const stdair::RandomSeed_T& lDemandCharacteristicsSeed =
      generateSeed (ioSharedGenerator);

    //
    ArrivalPatternCumulativeDistribution_T lDTDProbDist;
    lDTDProbDist.insert(ArrivalPatternCumulativeDistribution_T::value_type(-330,
                                                                           0));
    lDTDProbDist.insert(ArrivalPatternCumulativeDistribution_T::value_type(-40,
                                                                           0.2));
    lDTDProbDist.insert(ArrivalPatternCumulativeDistribution_T::value_type(-20,
                                                                           0.6));
    lDTDProbDist.insert(ArrivalPatternCumulativeDistribution_T::value_type(-1,
                                                                           1.0));
    //
    POSProbabilityMassFunction_T lPOSProbDist;
    lPOSProbDist.insert (POSProbabilityMassFunction_T::value_type ("BKK", 0.3));
    lPOSProbDist.insert (POSProbabilityMassFunction_T::value_type ("SIN", 0.7));
    //
    ChannelProbabilityMassFunction_T lChannelProbDist;
    lChannelProbDist.insert (ChannelProbabilityMassFunction_T::value_type ("DF",
                                                                           0.1));
    lChannelProbDist.insert (ChannelProbabilityMassFunction_T::value_type ("DN",
                                                                           0.3));
    lChannelProbDist.insert (ChannelProbabilityMassFunction_T::value_type ("IF",
                                                                           0.4));
    lChannelProbDist.insert (ChannelProbabilityMassFunction_T::value_type ("IN",
                                                                           0.2));
    //
    TripTypeProbabilityMassFunction_T lTripProbDist;
    lTripProbDist.insert (TripTypeProbabilityMassFunction_T::value_type ("RO",
                                                                         0.6));
    lTripProbDist.insert (TripTypeProbabilityMassFunction_T::value_type ("RI",
                                                                         0.2));
    lTripProbDist.insert (TripTypeProbabilityMassFunction_T::value_type ("OW",
                                                                         0.2));
    //
    StayDurationProbabilityMassFunction_T lStayProbDist;
    lStayProbDist.insert(StayDurationProbabilityMassFunction_T::value_type(0,
                                                                           0.1));
    lStayProbDist.insert(StayDurationProbabilityMassFunction_T::value_type(1,
                                                                           0.1));
    lStayProbDist.insert(StayDurationProbabilityMassFunction_T::value_type(2,
                                                                           .15));
    lStayProbDist.insert(StayDurationProbabilityMassFunction_T::value_type(3,
                                                                           .15));
    lStayProbDist.insert(StayDurationProbabilityMassFunction_T::value_type(4,
                                                                           .15));
    lStayProbDist.insert(StayDurationProbabilityMassFunction_T::value_type(5,
                                                                           .35));
    //
    FrequentFlyerProbabilityMassFunction_T lFFProbDist;
    lFFProbDist.insert(FrequentFlyerProbabilityMassFunction_T::value_type("P",
                                                                          0.01));
    lFFProbDist.insert(FrequentFlyerProbabilityMassFunction_T::value_type("G",
                                                                          0.05));
    lFFProbDist.insert(FrequentFlyerProbabilityMassFunction_T::value_type("S",
                                                                          0.15));
    lFFProbDist.insert(FrequentFlyerProbabilityMassFunction_T::value_type("M",
                                                                          0.3));
    lFFProbDist.insert(FrequentFlyerProbabilityMassFunction_T::value_type("N",
                                                                          0.49));
    //
    PreferredDepartureTimeContinuousDistribution_T lPrefDepTimeProbDist;
    lPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (6, 0));
    lPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (7,
                                                                          0.1));
    lPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (9,
                                                                          0.3));
    lPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (17,
                                                                          0.4));
    lPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (19,
                                                                          0.80));
    lPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (20,
                                                                          0.95));
    lPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (22,
                                                                          1));
    //
    ValueOfTimeContinuousDistribution_T lTimeValueProbDist;
    lTimeValueProbDist.insert(ValueOfTimeContinuousDistribution_T::value_type(15,
                                                                              0));
    lTimeValueProbDist.insert(ValueOfTimeContinuousDistribution_T::value_type(60,
                                                                              1));

    //
    const stdair::WTP_T lWTP (1000.0);
    const stdair::ChangeFeesRatio_T lChangeFees (0.5);
    const stdair::NonRefundableRatio_T lNonRefundable (0.5);


    // Delegate the call to the dedicated command
    DemandStream& lDemandStream = 
      createDemandStream (ioEventQueue, lDemandStreamKey, lDTDProbDist,
                          lPOSProbDist, lChannelProbDist, lTripProbDist,
                          lStayProbDist, lFFProbDist,
                          lChangeFees, lNonRefundable,
                          lPrefDepTimeProbDist,
                          lWTP, lTimeValueProbDist, lDemandDistribution,
                          ioSharedGenerator.getBaseGenerator(),
                          lRequestDateTimeSeed,
                          lDemandCharacteristicsSeed, iPOSProbMass);

    // Calculate the expected total number of events for the current
    // demand stream
    const stdair::NbOfRequests_T& lExpectedTotalNbOfEvents =
      lDemandStream.getMeanNumberOfRequests();

    /**
     * Initialise the progress statuses, specific to the booking request type
     */
    ioEventQueue.addStatus (stdair::EventType::BKG_REQ,
                            lExpectedTotalNbOfEvents);
  }

  // //////////////////////////////////////////////////////////////////////
  DemandStream& DemandManager::createDemandStream
  (stdair::EventQueue& ioEventQueue,
   const DemandStreamKey& iKey,
   const ArrivalPatternCumulativeDistribution_T& iArrivalPattern,
   const POSProbabilityMassFunction_T& iPOSProbMass,
   const ChannelProbabilityMassFunction_T& iChannelProbMass,
   const TripTypeProbabilityMassFunction_T& iTripTypeProbMass,
   const StayDurationProbabilityMassFunction_T& iStayDurationProbMass,
   const FrequentFlyerProbabilityMassFunction_T& iFrequentFlyerProbMass,
   const stdair::ChangeFeesRatio_T& iChangeFeeProb,
   const stdair::NonRefundableRatio_T& iNonRefundableProb,
   const PreferredDepartureTimeContinuousDistribution_T& iPreferredDepartureTimeContinuousDistribution,
   const stdair::WTP_T& iMinWTP,
   const ValueOfTimeContinuousDistribution_T& iValueOfTimeContinuousDistribution,
   const DemandDistribution& iDemandDistribution,
   stdair::BaseGenerator_T&  ioSharedGenerator,
   const stdair::RandomSeed_T& iRequestDateTimeSeed,
   const stdair::RandomSeed_T& iDemandCharacteristicsSeed,
   const POSProbabilityMass_T& iDefaultPOSProbablityMass) {
    
    DemandStream& oDemandStream =
      stdair::FacBom<DemandStream>::instance().create (iKey);

    oDemandStream.setAll (iArrivalPattern, iPOSProbMass,
                          iChannelProbMass, iTripTypeProbMass,
                          iStayDurationProbMass, iFrequentFlyerProbMass,
                          iChangeFeeProb, iNonRefundableProb,
                          iPreferredDepartureTimeContinuousDistribution,
                          iMinWTP, iValueOfTimeContinuousDistribution,
                          iDemandDistribution, ioSharedGenerator,
                          iRequestDateTimeSeed, iDemandCharacteristicsSeed,
                          iDefaultPOSProbablityMass);

    // Link the DemandStream to its parent (EventQueue)
    stdair::FacBomManager::linkWithParent (ioEventQueue, oDemandStream);
    
    // Add the DemandStream to the dedicated list and map
    stdair::FacBomManager::addToListAndMap (ioEventQueue, oDemandStream);

    return oDemandStream;
  }
    
  // //////////////////////////////////////////////////////////////////////
  void DemandManager::
  createDemandCharacteristics (stdair::EventQueue& ioEventQueue,
                               stdair::RandomGeneration& ioSharedGenerator,
                               const POSProbabilityMass_T& iPOSProbMass,
                               const DemandStruct& iDemand) {
    stdair::BaseGenerator_T& lSharedGenerator =
      ioSharedGenerator.getBaseGenerator();
    
    // Parse the date period and DoW and generate demand characteristics.
    const stdair::DatePeriod_T lDateRange = iDemand._dateRange;
    for (boost::gregorian::day_iterator itDate = lDateRange.begin();
         itDate != lDateRange.end(); ++itDate) {
      const stdair::Date_T& currentDate = *itDate;

      // Retrieve, for the current day, the Day-Of-the-Week (thanks to Boost)
      const unsigned short currentDoW = currentDate.day_of_week().as_number();
        
      // The demand structure stores which Days (-Of-the-Week) are
      // active within the week. For each day (Mon., Tue., etc.), a boolean
      // states whether the Flight is active for that day.
      const stdair::DoWStruct& lDoWList = iDemand._dow;
      const bool isDoWActive = lDoWList.getStandardDayOfWeek (currentDoW);

      if (isDoWActive == true) {
        const DemandStreamKey lDemandStreamKey (iDemand._origin,
                                                iDemand._destination,
                                                currentDate,
                                                iDemand._prefCabin);
        // DEBUG
        // STDAIR_LOG_DEBUG ("Demand stream key: " << lDemandStreamKey.describe());
        
        //
        const DemandDistribution lDemandDistribution (iDemand._demandMean,
                                                      iDemand._demandStdDev);
        
        // Seed
        const stdair::RandomSeed_T& lRequestDateTimeSeed =
          generateSeed (ioSharedGenerator);
        const stdair::RandomSeed_T& lDemandCharacteristicsSeed =
          generateSeed (ioSharedGenerator);
        
        // Delegate the call to the dedicated command
        DemandStream& lDemandStream = 
          createDemandStream (ioEventQueue, lDemandStreamKey,
                              iDemand._dtdProbDist, iDemand._posProbDist,
                              iDemand._channelProbDist,
                              iDemand._tripProbDist,
                              iDemand._stayProbDist, iDemand._ffProbDist,
                              iDemand._changeFeeProb,
                              iDemand._nonRefundableProb,
                              iDemand._prefDepTimeProbDist,
                              iDemand._minWTP,
                              iDemand._timeValueProbDist,
                              lDemandDistribution, lSharedGenerator,
                              lRequestDateTimeSeed,
                              lDemandCharacteristicsSeed,
                              iPOSProbMass);
        
        // Calculate the expected total number of events for the current
        // demand stream
        const stdair::NbOfRequests_T& lExpectedTotalNbOfEvents =
          lDemandStream.getMeanNumberOfRequests();
        
        /**
         * Initialise the progress statuses, one specific to the
         * booking request type.
         */
        ioEventQueue.addStatus (stdair::EventType::BKG_REQ,
                                lExpectedTotalNbOfEvents);
      }
    }
  }

  // ////////////////////////////////////////////////////////////////////
  stdair::RandomSeed_T DemandManager::
  generateSeed (stdair::RandomGeneration& ioSharedGenerator) {
    stdair::RealNumber_T lVariateUnif = ioSharedGenerator() * 1e9;
    stdair::RandomSeed_T oSeed = static_cast<stdair::RandomSeed_T>(lVariateUnif);
    return oSeed;
  }
  
  // ////////////////////////////////////////////////////////////////////
  const bool DemandManager::
  stillHavingRequestsToBeGenerated (const stdair::EventQueue& iEventQueue,
                                    const stdair::DemandStreamKeyStr_T& iKey,
                                    stdair::ProgressStatusSet& ioPSS,
                                    const stdair::DemandGenerationMethod& iDemandGenerationMethod) {
    // Retrieve the DemandStream which corresponds to the given key.
    const DemandStream& lDemandStream =
      stdair::BomManager::getObject<DemandStream> (iEventQueue, iKey);

    // Retrieve the progress status of the demand stream.
    stdair::ProgressStatus
      lProgressStatus (lDemandStream.getNumberOfRequestsGeneratedSoFar(),
                       lDemandStream.getMeanNumberOfRequests(),
                       lDemandStream.getTotalNumberOfRequestsToBeGenerated());
    ioPSS.setSpecificGeneratorStatus (lProgressStatus, iKey);
    
    return lDemandStream.stillHavingRequestsToBeGenerated (iDemandGenerationMethod);
  }

  // ////////////////////////////////////////////////////////////////////
  stdair::BookingRequestPtr_T DemandManager::
  generateNextRequest (stdair::EventQueue& ioEventQueue,
                       stdair::RandomGeneration& ioGenerator,
                       const stdair::DemandStreamKeyStr_T& iKey,
                       const stdair::DemandGenerationMethod& iDemandGenerationMethod) {

    // Retrieve the DemandStream which corresponds to the given key.
    DemandStream& lDemandStream = 
      stdair::BomManager::getObject<DemandStream> (ioEventQueue, iKey);

    // Generate the next booking request
    stdair::BookingRequestPtr_T lBookingRequest =
      lDemandStream.generateNextRequest (ioGenerator,
                                         iDemandGenerationMethod);

    // Create an event structure
    stdair::EventStruct lEventStruct (stdair::EventType::BKG_REQ,
                                      lBookingRequest);

    /**
       \note When adding an event in the event queue, the event can be
       altered. That happens when an event already exists, in the
       event queue, with exactly the same date-time stamp. In that
       case, the date-time stamp is altered for the newly added event,
       so that the unicity on the date-time stamp can be guaranteed.
    */
    ioEventQueue.addEvent (lEventStruct);
    
    return lBookingRequest;
  }

  // ////////////////////////////////////////////////////////////////////
  stdair::Count_T DemandManager::
  generateFirstRequests (stdair::EventQueue& ioEventQueue,
                         stdair::RandomGeneration& ioGenerator,
                         const stdair::DemandGenerationMethod& iDemandGenerationMethod) {

    // Actual total number of events to be generated
    stdair::NbOfRequests_T lActualTotalNbOfEvents = 0.0;

    // Retrieve the DemandStream list
    const DemandStreamList_T& lDemandStreamList =
      stdair::BomManager::getList<DemandStream> (ioEventQueue);

    for (DemandStreamList_T::const_iterator itDemandStream =
           lDemandStreamList.begin();
         itDemandStream != lDemandStreamList.end(); ++itDemandStream) {
      DemandStream* lDemandStream_ptr = *itDemandStream;
      assert (lDemandStream_ptr != NULL);

      lDemandStream_ptr->setBoolFirstDateTimeRequest(true);

      // Calculate the expected total number of events for the current
      // demand stream
      const stdair::NbOfRequests_T& lActualNbOfEvents =
        lDemandStream_ptr->getTotalNumberOfRequestsToBeGenerated();
      lActualTotalNbOfEvents += lActualNbOfEvents;

      // Retrieve the key of the demand stream
      const DemandStreamKey& lKey = lDemandStream_ptr->getKey();

      // Update the progress status for the given event type (i.e.,
      // booking request)
      ioEventQueue.updateStatus (stdair::EventType::BKG_REQ, lActualNbOfEvents);

      // Check whether there are still booking requests to be generated
      const bool stillHavingRequestsToBeGenerated =
        lDemandStream_ptr->stillHavingRequestsToBeGenerated (iDemandGenerationMethod);
   
      if (stillHavingRequestsToBeGenerated) {
        // Generate the next event (booking request), and insert it
        // into the event queue
        generateNextRequest (ioEventQueue, ioGenerator, lKey.toString(),
                             iDemandGenerationMethod);
      }
    }

    // Update the actual total number of events to be generated
    ioEventQueue.setActualTotalNbOfEvents (lActualTotalNbOfEvents);

    // Retrieve the actual total number of events to be generated
    const stdair::Count_T oTotalNbOfEvents = std::floor (lActualTotalNbOfEvents);

    //
    return oTotalNbOfEvents;
  }
  
  // ////////////////////////////////////////////////////////////////////
  void DemandManager::reset (stdair::EventQueue& ioEventQueue,
                             stdair::BaseGenerator_T& ioShareGenerator) {

    // TODO: check whether it is really necessary to destroy the
    // objects manually. Indeed, FacSupervisor::cleanAll() should
    // destroy any BOM object.

    // Reset all the DemandStream objects
    const DemandStreamList_T& lDemandStreamList =
      stdair::BomManager::getList<DemandStream> (ioEventQueue);
    for (DemandStreamList_T::const_iterator itDS = lDemandStreamList.begin();
         itDS != lDemandStreamList.end(); ++itDS) {
      DemandStream* lCurrentDS_ptr = *itDS;
      assert (lCurrentDS_ptr != NULL);
      
      lCurrentDS_ptr->reset (ioShareGenerator);
    }
  }
  

  // ////////////////////////////////////////////////////////////////////
  bool DemandManager::
  generateCancellation (stdair::RandomGeneration& ioGenerator,
                        const stdair::TravelSolutionStruct& iTravelSolution,
                        const stdair::PartySize_T& iPartySize,
                        const stdair::DateTime_T& iRequestTime,
                        const stdair::Date_T& iDepartureDate,
                        stdair::EventStruct& ioEventStruct) {

    // Draw a random number to decide if we generate a
    // cancellation. For instance, the probability will be hardcoded.
    // The cancellation time will be generated uniformly.
    double lRandomNumber = ioGenerator();
   
    if (lRandomNumber >= 0.5) {
      return false;
    }
    lRandomNumber /= 0.5;

    // Hardcode the latest cancellation time.
    const stdair::Time_T lMidNight =
      boost::posix_time::hours (0);
    const stdair::DateTime_T lDepartureDateTime =
      boost::posix_time::ptime (iDepartureDate, lMidNight);

    // Time to departure.
    const stdair::Duration_T lTimeToDeparture = lDepartureDateTime-iRequestTime;

    // Cancellation time to departure
    const long lTimeToDepartureInSeconds = lTimeToDeparture.total_seconds();
    const long lCancellationTimeToDepartureInSeconds =
      static_cast<long> (lTimeToDepartureInSeconds * lRandomNumber);
    const stdair::Duration_T lCancellationTimeToDeparture (0, 0, lCancellationTimeToDepartureInSeconds);
    
    // Cancellation time
    const stdair::DateTime_T lCancellationTime =
      lDepartureDateTime - lCancellationTimeToDeparture;

    // Build the list of Class ID's.
    stdair::BookingClassIDList_T lClassIDList;
    
    const stdair::ClassObjectIDMapHolder_T& lClassObjectIDMapHolder =
      iTravelSolution.getClassObjectIDMapHolder();
    const stdair::FareOptionStruct& lChosenFareOption =
      iTravelSolution.getChosenFareOption ();
    const stdair::ClassList_StringList_T& lClassPath =
      lChosenFareOption.getClassPath();
    stdair::ClassList_StringList_T::const_iterator itClassKeyList =
      lClassPath.begin();
    for (stdair::ClassObjectIDMapHolder_T::const_iterator itClassObjectIDMap =
           lClassObjectIDMapHolder.begin();
         itClassObjectIDMap != lClassObjectIDMapHolder.end();
         ++itClassObjectIDMap, ++itClassKeyList) {
      const stdair::ClassObjectIDMap_T& lClassObjectIDMap = *itClassObjectIDMap;
      
      // TODO: Removed this hardcode.
      std::ostringstream ostr;
      const stdair::ClassList_String_T& lClassList = *itClassKeyList;
      assert (lClassList.size() > 0);
      ostr << lClassList.at(0);
      const stdair::ClassCode_T lClassCode (ostr.str());
      stdair::ClassObjectIDMap_T::const_iterator itClassID =
        lClassObjectIDMap.find (lClassCode);
      assert (itClassID != lClassObjectIDMap.end());
      const stdair::BookingClassID_T& lClassID = itClassID->second;

      lClassIDList.push_back (lClassID);
    }
    
    // Create the cancellation.
    stdair::CancellationPtr_T lCancellation_ptr =
      stdair::CancellationPtr_T
      (new stdair::CancellationStruct (lClassIDList, iPartySize,
                                       lCancellationTime));

    // Create an event structure
    stdair::EventStruct lEventStruct (stdair::EventType::CX, lCancellation_ptr);
    ioEventStruct = lEventStruct;
    
    return true;
  }
  
  // //////////////////////////////////////////////////////////////////////
  void DemandManager::
  buildSampleBom (stdair::EventQueue& ioEventQueue,
                  stdair::RandomGeneration& ioSharedGenerator,
                  const POSProbabilityMass_T& iPOSProbMass) {

    //
    ArrivalPatternCumulativeDistribution_T lDTDProbDist;
    lDTDProbDist.insert(ArrivalPatternCumulativeDistribution_T::value_type(-330,
                                                                           0));
    lDTDProbDist.insert(ArrivalPatternCumulativeDistribution_T::value_type(-150,
                                                                           0.1));
    lDTDProbDist.insert(ArrivalPatternCumulativeDistribution_T::value_type(-92,
                                                                           0.2));
    lDTDProbDist.insert(ArrivalPatternCumulativeDistribution_T::value_type(-55,
                                                                           0.3));
    lDTDProbDist.insert(ArrivalPatternCumulativeDistribution_T::value_type(-34,
                                                                           0.4));
    lDTDProbDist.insert(ArrivalPatternCumulativeDistribution_T::value_type(-21,
                                                                           0.5));
    lDTDProbDist.insert(ArrivalPatternCumulativeDistribution_T::value_type(-12,
                                                                           0.6));
    lDTDProbDist.insert(ArrivalPatternCumulativeDistribution_T::value_type(-6,
                                                                           0.7));
    lDTDProbDist.insert(ArrivalPatternCumulativeDistribution_T::value_type(-3,
                                                                           0.8));
    lDTDProbDist.insert(ArrivalPatternCumulativeDistribution_T::value_type(-1,
                                                                           0.9));
    lDTDProbDist.insert(ArrivalPatternCumulativeDistribution_T::value_type(0,
                                                                           1.0));

    //
    ChannelProbabilityMassFunction_T lChannelProbDist;
    lChannelProbDist.insert (ChannelProbabilityMassFunction_T::value_type ("DF",
                                                                           0.0));
    lChannelProbDist.insert (ChannelProbabilityMassFunction_T::value_type ("DN",
                                                                           0.0));
    lChannelProbDist.insert (ChannelProbabilityMassFunction_T::value_type ("IF",
                                                                           0.0));
    lChannelProbDist.insert (ChannelProbabilityMassFunction_T::value_type ("IN",
                                                                           1.0));

    //
    TripTypeProbabilityMassFunction_T lTripProbDist;
    lTripProbDist.insert (TripTypeProbabilityMassFunction_T::value_type ("RO",
                                                                         0.0));
    lTripProbDist.insert (TripTypeProbabilityMassFunction_T::value_type ("RI",
                                                                         0.0));
    lTripProbDist.insert (TripTypeProbabilityMassFunction_T::value_type ("OW",
                                                                         1.0));

    //
    StayDurationProbabilityMassFunction_T lStayProbDist;
    lStayProbDist.insert(StayDurationProbabilityMassFunction_T::value_type(0,
                                                                           0.1));
    lStayProbDist.insert(StayDurationProbabilityMassFunction_T::value_type(1,
                                                                           0.1));
    lStayProbDist.insert(StayDurationProbabilityMassFunction_T::value_type(2,
                                                                           .15));
    lStayProbDist.insert(StayDurationProbabilityMassFunction_T::value_type(3,
                                                                           .15));
    lStayProbDist.insert(StayDurationProbabilityMassFunction_T::value_type(4,
                                                                           .15));
    lStayProbDist.insert(StayDurationProbabilityMassFunction_T::value_type(5,
                                                                           .35));

    //
    FrequentFlyerProbabilityMassFunction_T lFFProbDist;
    lFFProbDist.insert(FrequentFlyerProbabilityMassFunction_T::value_type("P",
                                                                          0.1));
    lFFProbDist.insert(FrequentFlyerProbabilityMassFunction_T::value_type("G",
                                                                          0.01));
    lFFProbDist.insert(FrequentFlyerProbabilityMassFunction_T::value_type("S",
                                                                          0.09));
    lFFProbDist.insert(FrequentFlyerProbabilityMassFunction_T::value_type("M",
                                                                          0.4));
    lFFProbDist.insert(FrequentFlyerProbabilityMassFunction_T::value_type("N",
                                                                          0.4));

    //
    ValueOfTimeContinuousDistribution_T lTimeValueProbDist;
    lTimeValueProbDist.insert(ValueOfTimeContinuousDistribution_T::value_type(15,
                                                                              0));
    lTimeValueProbDist.insert(ValueOfTimeContinuousDistribution_T::value_type(60,
                                                                              1));

    /*===================================================================================*/

    // Key of the demand stream
    const stdair::AirportCode_T lSINOrigin ("SIN");
    const stdair::AirportCode_T lBKKDestination ("BKK");
    const stdair::Date_T lDepDate (2010, 2, 8);
    const stdair::CabinCode_T lCabin ("Y");

    //
    const DemandStreamKey lSINBKKDemandStreamKey (lSINOrigin, lBKKDestination, lDepDate,
                                                  lCabin);

    // DEBUG
    // STDAIR_LOG_DEBUG ("Demand stream key: " << lDemandStreamKey.describe());
    
    // Distribution for the number of requests
    const stdair::MeanValue_T lSINBKKDemandMean (60.0);
    const stdair::StdDevValue_T lSINBKKDemandStdDev (4.0);
    const DemandDistribution lSINBKKDemandDistribution (lSINBKKDemandMean, lSINBKKDemandStdDev);
    
    // Seed
    const stdair::RandomSeed_T& lSINBKKRequestDateTimeSeed =
      generateSeed (ioSharedGenerator);
    const stdair::RandomSeed_T& lSINBKKDemandCharacteristicsSeed =
      generateSeed (ioSharedGenerator);

    
    //
    POSProbabilityMassFunction_T lSINBKKPOSProbDist;    
    lSINBKKPOSProbDist.insert (POSProbabilityMassFunction_T::value_type ("SIN", 1.0));
    lSINBKKPOSProbDist.insert (POSProbabilityMassFunction_T::value_type ("BKK", 0.0)); 
        
    //
    PreferredDepartureTimeContinuousDistribution_T lSINPrefDepTimeProbDist;
    lSINPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (6, 0));
    lSINPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (8,
                                                                          0.7));
    lSINPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (10,
                                                                          0.8));
    lSINPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (12,
                                                                          0.9));
    lSINPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (14,
                                                                          1.0));    

    //
    const stdair::WTP_T lSINBKKWTP (400.0);
    const stdair::ChangeFeesRatio_T lChangeFees (0.5);
    const stdair::NonRefundableRatio_T lNonRefundable (0.5);


    // Delegate the call to the dedicated command
    DemandStream& lSINBKKDemandStream = 
      createDemandStream (ioEventQueue, lSINBKKDemandStreamKey, lDTDProbDist,
                          lSINBKKPOSProbDist, lChannelProbDist, lTripProbDist,
                          lStayProbDist, lFFProbDist,
                          lChangeFees, lNonRefundable,
                          lSINPrefDepTimeProbDist,
                          lSINBKKWTP, lTimeValueProbDist,
                          lSINBKKDemandDistribution,
                          ioSharedGenerator.getBaseGenerator(),
                          lSINBKKRequestDateTimeSeed,
                          lSINBKKDemandCharacteristicsSeed, iPOSProbMass);

    // Calculate the expected total number of events for the current
    // demand stream
    const stdair::NbOfRequests_T& lSINBKKExpectedNbOfEvents =
      lSINBKKDemandStream.getMeanNumberOfRequests();

    /*===================================================================================*/

    // Key of the demand stream
    const stdair::AirportCode_T lBKKOrigin ("BKK");
    const stdair::AirportCode_T lHKGDestination ("HKG");

    //
    const DemandStreamKey lBKKHKGDemandStreamKey (lBKKOrigin, lHKGDestination, lDepDate,
                                                  lCabin);

    // DEBUG
    // STDAIR_LOG_DEBUG ("Demand stream key: " << lDemandStreamKey.describe());
    
    // Distribution for the number of requests
    const stdair::MeanValue_T lBKKHKGDemandMean (60.0);
    const stdair::StdDevValue_T lBKKHKGDemandStdDev (4.0);
    const DemandDistribution lBKKHKGDemandDistribution (lBKKHKGDemandMean, lBKKHKGDemandStdDev);
    
    // Seed
    const stdair::RandomSeed_T& lBKKHKGRequestDateTimeSeed =
      generateSeed (ioSharedGenerator);
    const stdair::RandomSeed_T& lBKKHKGDemandCharacteristicsSeed =
      generateSeed (ioSharedGenerator);

    
    //
    POSProbabilityMassFunction_T lBKKHKGPOSProbDist;    
    lBKKHKGPOSProbDist.insert (POSProbabilityMassFunction_T::value_type ("BKK", 1.0));
    lBKKHKGPOSProbDist.insert (POSProbabilityMassFunction_T::value_type ("HKG", 0.0)); 
        
    //
    PreferredDepartureTimeContinuousDistribution_T lBKKPrefDepTimeProbDist;
    lBKKPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (8, 0));
    lBKKPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (10,
                                                                          0.2));
    lBKKPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (1,
                                                                          0.6));
    lBKKPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (14,
                                                                          0.8));
    lBKKPrefDepTimeProbDist.
      insert (PreferredDepartureTimeContinuousDistribution_T::value_type (16,
                                                                          1.0));    

    //
    const stdair::WTP_T lBKKHKGWTP (400.0);


    // Delegate the call to the dedicated command
    DemandStream& lBKKHKGDemandStream = 
      createDemandStream (ioEventQueue, lBKKHKGDemandStreamKey, lDTDProbDist,
                          lBKKHKGPOSProbDist, lChannelProbDist, lTripProbDist,
                          lStayProbDist, lFFProbDist,
                          lChangeFees, lNonRefundable,
                          lBKKPrefDepTimeProbDist,
                          lBKKHKGWTP, lTimeValueProbDist,
                          lBKKHKGDemandDistribution,
                          ioSharedGenerator.getBaseGenerator(),
                          lBKKHKGRequestDateTimeSeed,
                          lBKKHKGDemandCharacteristicsSeed, iPOSProbMass);

    // Calculate the expected total number of events for the current
    // demand stream
    const stdair::NbOfRequests_T& lBKKHKGExpectedNbOfEvents =
      lBKKHKGDemandStream.getMeanNumberOfRequests();

    /*===================================================================================*/

    // Key of the demand stream

    //
    const DemandStreamKey lSINHKGDemandStreamKey (lSINOrigin, lHKGDestination, lDepDate,
                                                  lCabin);

    // DEBUG
    // STDAIR_LOG_DEBUG ("Demand stream key: " << lDemandStreamKey.describe());
    
    // Distribution for the number of requests
    const stdair::MeanValue_T lSINHKGDemandMean (60.0);
    const stdair::StdDevValue_T lSINHKGDemandStdDev (4.0);
    const DemandDistribution lSINHKGDemandDistribution (lSINHKGDemandMean, lSINHKGDemandStdDev);
    
    // Seed
    const stdair::RandomSeed_T& lSINHKGRequestDateTimeSeed =
      generateSeed (ioSharedGenerator);
    const stdair::RandomSeed_T& lSINHKGDemandCharacteristicsSeed =
      generateSeed (ioSharedGenerator);

    
    //
    POSProbabilityMassFunction_T lSINHKGPOSProbDist;    
    lSINHKGPOSProbDist.insert (POSProbabilityMassFunction_T::value_type ("SIN", 1.0));
    lSINHKGPOSProbDist.insert (POSProbabilityMassFunction_T::value_type ("HKG", 0.0)); 
        
    //
    const stdair::WTP_T lSINHKGWTP (750.0);


    // Delegate the call to the dedicated command
    DemandStream& lSINHKGDemandStream = 
      createDemandStream (ioEventQueue, lSINHKGDemandStreamKey, lDTDProbDist,
                          lSINHKGPOSProbDist, lChannelProbDist, lTripProbDist,
                          lStayProbDist, lFFProbDist, 
                          lChangeFees, lNonRefundable,
                          lSINPrefDepTimeProbDist,
                          lSINHKGWTP, lTimeValueProbDist, lSINHKGDemandDistribution,
                          ioSharedGenerator.getBaseGenerator(),
                          lSINHKGRequestDateTimeSeed,
                          lSINHKGDemandCharacteristicsSeed, iPOSProbMass);

    // Calculate the expected total number of events for the current
    // demand stream
    const stdair::NbOfRequests_T& lSINHKGExpectedNbOfEvents =
      lSINHKGDemandStream.getMeanNumberOfRequests();

    /*===================================================================================*/

    /**
     * Initialise the progress statuses, specific to the booking request type
     */
    const stdair::NbOfRequests_T lExpectedTotalNbOfEvents =
      lSINBKKExpectedNbOfEvents + lBKKHKGExpectedNbOfEvents + lSINHKGExpectedNbOfEvents;
    ioEventQueue.addStatus (stdair::EventType::BKG_REQ,
                            lExpectedTotalNbOfEvents);
  }

}
