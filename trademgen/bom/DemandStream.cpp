// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <iostream>
#include <sstream>
#include <cmath>
// Boost
#include <boost/make_shared.hpp>
// StdAir
#include <stdair/basic/BasConst_General.hpp>
#include <stdair/basic/BasConst_Inventory.hpp>
#include <stdair/basic/BasConst_Request.hpp>
#include <stdair/bom/BookingRequestStruct.hpp>
#include <stdair/service/Logger.hpp>
// TraDemGen
#include <trademgen/basic/BasConst_DemandGeneration.hpp>
#include <trademgen/bom/DemandStream.hpp>

namespace TRADEMGEN {

  // ////////////////////////////////////////////////////////////////////
  DemandStream::DemandStream()
    : _key (stdair::DEFAULT_ORIGIN, stdair::DEFAULT_DESTINATION,
            stdair::DEFAULT_DEPARTURE_DATE, stdair::DEFAULT_CABIN_CODE),
      _parent (NULL),
      _demandCharacteristics (ArrivalPatternCumulativeDistribution_T(),
                              POSProbabilityMassFunction_T(),
                              ChannelProbabilityMassFunction_T(),
                              TripTypeProbabilityMassFunction_T(),
                              StayDurationProbabilityMassFunction_T(),
                              FrequentFlyerProbabilityMassFunction_T(),
                              PreferredDepartureTimeContinuousDistribution_T(),
                              0.0,
                              ValueOfTimeContinuousDistribution_T()),
      _posProMass (DEFAULT_POS_PROBALILITY_MASS),
      _firstDateTimeRequest (true) {
    assert (false);
  }

  // ////////////////////////////////////////////////////////////////////
  DemandStream::DemandStream (const DemandStream&)
    : _key (stdair::DEFAULT_ORIGIN, stdair::DEFAULT_DESTINATION,
            stdair::DEFAULT_DEPARTURE_DATE, stdair::DEFAULT_CABIN_CODE),
      _parent (NULL),
      _demandCharacteristics (ArrivalPatternCumulativeDistribution_T(),
                              POSProbabilityMassFunction_T(),
                              ChannelProbabilityMassFunction_T(),
                              TripTypeProbabilityMassFunction_T(),
                              StayDurationProbabilityMassFunction_T(),
                              FrequentFlyerProbabilityMassFunction_T(),
                              PreferredDepartureTimeContinuousDistribution_T(),
                              0.0,
                              ValueOfTimeContinuousDistribution_T()),
      _posProMass (DEFAULT_POS_PROBALILITY_MASS),
      _firstDateTimeRequest (true) {
    assert (false);
  }

  // ////////////////////////////////////////////////////////////////////
  DemandStream::DemandStream (const Key_T& iKey) :
    _key (iKey),
    _firstDateTimeRequest (true) {
  }

  // ////////////////////////////////////////////////////////////////////
  DemandStream::~DemandStream() {
  }

  // ////////////////////////////////////////////////////////////////////
  std::string DemandStream::toString() const {
    std::ostringstream oStr;
    oStr << _key.toString();
    return oStr.str();
  }

  // ////////////////////////////////////////////////////////////////////
  void DemandStream::
  setAll (const ArrivalPatternCumulativeDistribution_T& iArrivalPattern,
          const POSProbabilityMassFunction_T& iPOSProbMass,
          const ChannelProbabilityMassFunction_T& iChannelProbMass,
          const TripTypeProbabilityMassFunction_T& iTripTypeProbMass,
          const StayDurationProbabilityMassFunction_T& iStayDurationProbMass,
          const FrequentFlyerProbabilityMassFunction_T& iFrequentFlyerProbMass,
          const PreferredDepartureTimeContinuousDistribution_T& iPreferredDepartureTimeContinuousDistribution,
          const stdair::WTP_T& iMinWTP,
          const ValueOfTimeContinuousDistribution_T& iValueOfTimeContinuousDistribution,
          const DemandDistribution& iDemandDistribution,
          stdair::BaseGenerator_T& ioSharedGenerator,
          const stdair::RandomSeed_T& iRequestDateTimeSeed,
          const stdair::RandomSeed_T& iDemandCharacteristicsSeed,
          const POSProbabilityMass_T& iDefaultPOSProbablityMass) {

    setDemandCharacteristics (iArrivalPattern, iPOSProbMass,
                              iChannelProbMass, iTripTypeProbMass,
                              iStayDurationProbMass, iFrequentFlyerProbMass,
                              iPreferredDepartureTimeContinuousDistribution,
                              iMinWTP, iValueOfTimeContinuousDistribution);

    setDemandDistribution (iDemandDistribution);
    setTotalNumberOfRequestsToBeGenerated (0);
    setRequestDateTimeRandomGeneratorSeed (iRequestDateTimeSeed);
    setDemandCharacteristicsRandomGeneratorSeed (iDemandCharacteristicsSeed);
    setPOSProbabilityMass (iDefaultPOSProbablityMass);

    //
    init (ioSharedGenerator);
  }

  // ////////////////////////////////////////////////////////////////////
  std::string DemandStream::display() const {
    std::ostringstream oStr;

    oStr << "Demand stream key: " << _key.toString() << std::endl;

    //
    oStr << _demandCharacteristics.describe();

    //
    oStr << _demandDistribution.describe() << " => "
         << _totalNumberOfRequestsToBeGenerated << " to be generated"
         << std::endl;

    //
    oStr << "Random generation context: " << _randomGenerationContext
         << std::endl;

    //
    oStr << "Random generator for date-time: "
         << _requestDateTimeRandomGenerator << std::endl;
    oStr << "Random generator for demand characteristics: "
         << _demandCharacteristicsRandomGenerator << std::endl;

    //
    oStr << _posProMass.displayProbabilityMass() << std::endl;

    return oStr.str();
  }    

  // ////////////////////////////////////////////////////////////////////
  void DemandStream::init (stdair::BaseGenerator_T& ioSharedGenerator) {
    
    // Generate the number of requests
    const stdair::RealNumber_T lMu = _demandDistribution._meanNumberOfRequests;
    const stdair::RealNumber_T lSigma =
      _demandDistribution._stdDevNumberOfRequests;

    stdair::NormalDistribution_T lDistrib (lMu, lSigma);
    stdair::NormalGenerator_T lNormalGen (ioSharedGenerator, lDistrib);
    
    const stdair::RealNumber_T lRealNumberOfRequestsToBeGenerated =lNormalGen();

    const stdair::NbOfRequests_T lIntegerNumberOfRequestsToBeGenerated = 
      std::floor (lRealNumberOfRequestsToBeGenerated + 0.5);
    
    _totalNumberOfRequestsToBeGenerated = lIntegerNumberOfRequestsToBeGenerated;
  }  

  // ////////////////////////////////////////////////////////////////////
  const bool DemandStream::stillHavingRequestsToBeGenerated() const {
    bool hasStillHavingRequestsToBeGenerated = true;
    
    // Check whether enough requests have already been generated
    const stdair::Count_T lNbOfRequestsGeneratedSoFar =
      _randomGenerationContext.getNumberOfRequestsGeneratedSoFar();

    const stdair::Count_T lRemainingNumberOfRequestsToBeGenerated =
      _totalNumberOfRequestsToBeGenerated - lNbOfRequestsGeneratedSoFar;

    if (lRemainingNumberOfRequestsToBeGenerated <= 0) {
      hasStillHavingRequestsToBeGenerated = false;
    }
    
    return hasStillHavingRequestsToBeGenerated;
  }

  // ////////////////////////////////////////////////////////////////////
  const stdair::DateTime_T DemandStream::generateTimeOfRequestExponentialLaw() {

    const ContinuousFloatDuration_T& lArrivalPattern =
      _demandCharacteristics._arrivalPattern;

    const stdair::Time_T lHardcodedReferenceDepartureTime =
      boost::posix_time::hours (8);
    
    const stdair::DateTime_T lDepartureDateTime =
      boost::posix_time::ptime (_key.getPreferredDepartureDate(),
                                lHardcodedReferenceDepartureTime);

    if (_firstDateTimeRequest) {

      const stdair::Probability_T lProbabilityFirstRequest = 0;
      
      const stdair::FloatDuration_T lNumberOfDays =
        lArrivalPattern.getValue (lProbabilityFirstRequest);

      const stdair::Duration_T lDifferenceBetweenDepartureAndThisRequest =
        convertFloatIntoDuration (lNumberOfDays);

      // The request date-time is derived from departure date and arrival pattern.
      const stdair::DateTime_T oDateTimeThisRequest =
        lDepartureDateTime + lDifferenceBetweenDepartureAndThisRequest;

      _dateTimeLastRequest = oDateTimeThisRequest;
      _firstDateTimeRequest = false;

      // Update the counter of requests generated so far.
      incrementGeneratedRequestsCounter();

      return oDateTimeThisRequest;
      
    }
    std::cout << "\n Generate new request: \n";

    std::cout << "Last Request: " <<  _dateTimeLastRequest << "\n";
    
    assert(_firstDateTimeRequest == false);

    const stdair::Duration_T lDaysBeforeDepartureLastRequest =
      lDepartureDateTime - _dateTimeLastRequest;

    double lDailyRate =
      lArrivalPattern.getDerivativeValue (-lDaysBeforeDepartureLastRequest.hours()/24);

    const double lDemandMean = _demandDistribution._meanNumberOfRequests;

    lDailyRate *= lDemandMean;

    std::cout << "Daily Rate: " <<  lDailyRate << "\n";

    const stdair::Duration_T lExponentialVariable = drawInterArrivalTime (lDailyRate);

    std::cout << "Exponential Variable: " << lExponentialVariable  << "\n";
    
    const stdair::DateTime_T oDateTimeThisRequest =
      _dateTimeLastRequest + lExponentialVariable;

    std::cout << "Date Time Request: " << oDateTimeThisRequest << "\n";

    _dateTimeLastRequest = oDateTimeThisRequest;

    // Update the counter of requests generated so far.
    incrementGeneratedRequestsCounter();

    return oDateTimeThisRequest;

  }

  // //////////////////////////////////////////////////////////////////////
  const stdair::Duration_T DemandStream::
    drawInterArrivalTime (double iDailyRate) {
    
    // Generate a random variate, expressed in (fractional) day
    const double lExponentialVariateInDays =
      _requestDateTimeRandomGenerator.generateExponential (iDailyRate);

    std::cout << "!!!!!!!! Exponential law: " << lExponentialVariateInDays << "\n";
    
    // Convert the variate in a number of seconds
    const double lExponentialVariateInSeconds =
      (lExponentialVariateInDays * stdair::SECONDS_IN_ONE_DAY);
    assert (lExponentialVariateInSeconds > 0);

    // At least the returned inter-arrival time is equal to 1 sec
    int lExponentialVariateInSecondsInt = 1;
    if (lExponentialVariateInSeconds > 1) {
      lExponentialVariateInSecondsInt =
        static_cast<int>(lExponentialVariateInSeconds);
    }

    // Convert the variate in a (Boost typedef) time duration
    stdair::Duration_T lExponentialVariate =
      boost::posix_time::seconds (lExponentialVariateInSecondsInt);
      
    return lExponentialVariate;
  }

  // ////////////////////////////////////////////////////////////////////
  const stdair::DateTime_T DemandStream::generateTimeOfRequestStatisticOrder() {
   
    /**
     * Sequential Generation in Increasing Order.
     * The k-th order statistic of a statistical sample is its k-th
     * smallest value, and is usually denoted by X(k).
     * Here, the time of the k-th request must be generated, given the
     * value of X(k-1).
     *
     * The total number of requests to be generated is denoted by n.
     * The number of requests generated so far is denoted by k - 1.
     * X(k) follow the distribution of the k-th order statistic,
     * given the value of X(k-1).
     * F^{-1}_{ X(k) | X(k - 1) = x(k - 1) } (y)
     *               = 1 - (1 - x(k - 1))(1 - y)^{1/(n - k + 1)}
     *
     */

    //
    // Calculate the result of the formula above step by step.
    //
    
    // 1) Get the number of requests generated so far.
    //    (equal to k - 1)
    const stdair::Count_T& lNbOfRequestsGeneratedSoFar =
      _randomGenerationContext.getNumberOfRequestsGeneratedSoFar();

    // 2) Deduce the number of requests not generated yet.
    //    (equal to n - k + 1)
    const stdair::Count_T lRemainingNumberOfRequestsToBeGenerated =
      _totalNumberOfRequestsToBeGenerated - lNbOfRequestsGeneratedSoFar;

    // Assert that there are still requests to be generated.
    assert (lRemainingNumberOfRequestsToBeGenerated > 0);

    // 3) Inverse the number of requests not generated yet.
    //    1/(n - k + 1)
    const double lRemainingRate =
      1.0 / static_cast<double> (lRemainingNumberOfRequestsToBeGenerated);
   
    // 4) Get the cumulative probality so far and take its complement.
    //    (equal to 1 - x(k-1))
    const stdair::Probability_T& lCumulativeProbabilitySoFar =
      _randomGenerationContext.getCumulativeProbabilitySoFar();
    const stdair::Probability_T lComplementOfCumulativeProbabilitySoFar =
      1.0 - lCumulativeProbabilitySoFar;

    // 5) Draw a random variable y and calculate the factor equal to 
    //    (1 - y)^(1/(n - k + 1)).
    const stdair::Probability_T& lVariate = _requestDateTimeRandomGenerator();
    double lFactor = std::pow (1.0 - lVariate, lRemainingRate);

    // 6) Apply the whole formula above to calculate the cumulative probability
    //    of the new request.
    //    (equal to 1 - (1 - x(k-1))(1 - y)^(1/(n - k + 1)))
    const stdair::Probability_T lCumulativeProbabilityThisRequest =
      1.0 - lComplementOfCumulativeProbabilitySoFar * lFactor;
    
    // Now that the cumulative proportion of events generated has been
    // calculated, we deduce from the arrival pattern the arrival time of the
    // k-th event.
    const stdair::FloatDuration_T lNumberOfDaysBetweenDepartureAndThisRequest =
      _demandCharacteristics._arrivalPattern.getValue (lCumulativeProbabilityThisRequest);
    
    const stdair::Duration_T lDifferenceBetweenDepartureAndThisRequest =
      convertFloatIntoDuration (lNumberOfDaysBetweenDepartureAndThisRequest);

    const stdair::Time_T lHardcodedReferenceDepartureTime =
      boost::posix_time::hours (8);
    
    const stdair::DateTime_T lDepartureDateTime =
      boost::posix_time::ptime (_key.getPreferredDepartureDate(),
                                lHardcodedReferenceDepartureTime);

    // The request date-time is derived from departure date and arrival pattern.
    const stdair::DateTime_T oDateTimeThisRequest =
      lDepartureDateTime + lDifferenceBetweenDepartureAndThisRequest;
    
    // Update random generation context
    _randomGenerationContext.setCumulativeProbabilitySoFar (lCumulativeProbabilityThisRequest);

    // Update the counter of requests generated so far.
    incrementGeneratedRequestsCounter();

    // DEBUG
    // STDAIR_LOG_DEBUG (lCumulativeProbabilityThisRequest << "; "
    //                   << lNumberOfDaysBetweenDepartureAndThisRequest);
    
    return oDateTimeThisRequest;
  }

  // ////////////////////////////////////////////////////////////////////
  const stdair::AirportCode_T DemandStream::generatePOS() {
    
    // Generate a random number between 0 and 1.
    const stdair::Probability_T& lVariate = _demandCharacteristicsRandomGenerator();
    const stdair::AirportCode_T& oPOS = _demandCharacteristics.getPOSValue (lVariate);

    return oPOS;
  }

  // ////////////////////////////////////////////////////////////////////
  const stdair::ChannelLabel_T DemandStream::generateChannel() {
    // Generate a random number between 0 and 1.
    const stdair::Probability_T lVariate =
      _demandCharacteristicsRandomGenerator();

    return _demandCharacteristics._channelProbabilityMass.getValue (lVariate);
  }

  // ////////////////////////////////////////////////////////////////////
  const stdair::TripType_T DemandStream::generateTripType() {
    // Generate a random number between 0 and 1.
    const stdair::Probability_T lVariate =
      _demandCharacteristicsRandomGenerator(); 

    return _demandCharacteristics._tripTypeProbabilityMass.getValue (lVariate);
  }

  // ////////////////////////////////////////////////////////////////////
  const stdair::DayDuration_T DemandStream::generateStayDuration() {
    // Generate a random number between 0 and 1.
    const stdair::Probability_T lVariate =
      _demandCharacteristicsRandomGenerator();    

    return _demandCharacteristics._stayDurationProbabilityMass.getValue (lVariate);
  }
  
  // ////////////////////////////////////////////////////////////////////
  const stdair::FrequentFlyer_T DemandStream::generateFrequentFlyer() {
    // Generate a random number between 0 and 1.
    const stdair::Probability_T lVariate =
      _demandCharacteristicsRandomGenerator();       

    return _demandCharacteristics._frequentFlyerProbabilityMass.getValue (lVariate);
  }

  // ////////////////////////////////////////////////////////////////////
  const stdair::Duration_T DemandStream::generatePreferredDepartureTime() {
    // Generate a random number between 0 and 1.
    const stdair::Probability_T lVariate =
      _demandCharacteristicsRandomGenerator();     
    const stdair::IntDuration_T lNbOfSeconds = _demandCharacteristics.
      _preferredDepartureTimeCumulativeDistribution.getValue (lVariate);

    const stdair::Duration_T oTime = boost::posix_time::seconds (lNbOfSeconds);

    return oTime;
  }

  // ////////////////////////////////////////////////////////////////////
  const stdair::WTP_T DemandStream::
  generateWTP (stdair::RandomGeneration& ioGenerator,
               const stdair::Date_T& iDepartureDate,
               const stdair::DateTime_T& iDateTimeThisRequest,
               const stdair::DayDuration_T& iDurationOfStay) {
    const stdair::Date_T lDateThisRequest = iDateTimeThisRequest.date ();
    const stdair::DateOffset_T lAP = iDepartureDate - lDateThisRequest;
    const stdair::DayDuration_T lAPInDays = lAP.days ();

    stdair::RealNumber_T lProb =
      1 - lAPInDays / DEFAULT_MAX_ADVANCE_PURCHASE;
    if (lProb < 0.0) { lProb = 0.0; }
    stdair::RealNumber_T lFrat5Coef =
      _demandCharacteristics._frat5Pattern.getValue (lProb);

    const stdair::WTP_T lWTP =  _demandCharacteristics._minWTP
      * (1.0 + (lFrat5Coef - 1.0) * log(ioGenerator()) / log(0.5));
    
    return lWTP;
  }

  // ////////////////////////////////////////////////////////////////////
  const stdair::PriceValue_T DemandStream::generateValueOfTime() {
    // Generate a random number between 0 and 1.
    const stdair::Probability_T lVariate =
      _demandCharacteristicsRandomGenerator();    

    return _demandCharacteristics._valueOfTimeCumulativeDistribution.getValue (lVariate);
  }
  
  // ////////////////////////////////////////////////////////////////////
  stdair::BookingRequestPtr_T DemandStream::
  generateNextRequest (stdair::RandomGeneration& ioGenerator,
                       const bool iGenerateRequestWithStatisticOrder) {

    // Origin
    const stdair::AirportCode_T& lOrigin = _key.getOrigin();
    // Destination
    const stdair::AirportCode_T& lDestination = _key.getDestination();
    // Preferred departure date
    const stdair::Date_T& lPreferredDepartureDate = 
      _key.getPreferredDepartureDate();
    // Preferred cabin
    const stdair::CabinCode_T& lPreferredCabin = _key.getPreferredCabin();
    // Party size
    const stdair::NbOfSeats_T lPartySize = stdair::DEFAULT_PARTY_SIZE;
    // POS
    const stdair::AirportCode_T lPOS = generatePOS();
    
    // Time of request.
    stdair::DateTime_T lDateTimeThisRequest;
    if (iGenerateRequestWithStatisticOrder) {
      lDateTimeThisRequest = generateTimeOfRequestStatisticOrder();
    } else {
      lDateTimeThisRequest =
        generateTimeOfRequestExponentialLaw();
    }
    
    // Booking channel.
    const stdair::ChannelLabel_T lChannelLabel = generateChannel();
    // Trip type.
    const stdair::TripType_T lTripType = generateTripType();
    // Stay duration.
    const stdair::DayDuration_T lStayDuration = generateStayDuration();
    // Frequet flyer type.
    const stdair::FrequentFlyer_T lFrequentFlyer = generateFrequentFlyer();
    // Preferred departure time.
    const stdair::Duration_T lPreferredDepartureTime =
      generatePreferredDepartureTime();
    // Value of time
    const stdair::PriceValue_T lValueOfTime = generateValueOfTime();
    // WTP
    const stdair::WTP_T lWTP = generateWTP (ioGenerator, lPreferredDepartureDate,
                                            lDateTimeThisRequest, lStayDuration);

    // TODO 1: understand why the following form does not work, knowing
    // that:
    // typedef boost::shared_ptr<stdair::BookingRequestStruct> stdair::BookingRequestPtr_T
    // stdair::BookingRequestPtr_T oBookingRequest_ptr =
    //   boost::make_shared<stdair::BookingRequestStruct> ();


    // TODO 2: move the creation of the structure out of the BOM layer
    //  (into the command layer, e.g., within the DemandManager command).
    
    // Create the booking request
    stdair::BookingRequestPtr_T oBookingRequest_ptr = stdair::
      BookingRequestPtr_T (new
                           stdair::BookingRequestStruct (describeKey(),
                                                         lOrigin,
                                                         lDestination,
                                                         lPOS,
                                                         lPreferredDepartureDate,
                                                         lDateTimeThisRequest,
                                                         lPreferredCabin,
                                                         lPartySize,
                                                         lChannelLabel,
                                                         lTripType,
                                                         lStayDuration,
                                                         lFrequentFlyer,
                                                         lPreferredDepartureTime,
                                                         lWTP,
                                                         lValueOfTime));
    
    // DEBUG
    // STDAIR_LOG_DEBUG ("\n[BKG] " << oBookingRequest_ptr->describe());
    
    return oBookingRequest_ptr;
  }

  // ////////////////////////////////////////////////////////////////////
  void DemandStream::reset (stdair::BaseGenerator_T& ioSharedGenerator) {
    _randomGenerationContext.reset();
    init (ioSharedGenerator);
  }

  // ////////////////////////////////////////////////////////////////////

  const stdair::Duration_T DemandStream::
  convertFloatIntoDuration (const stdair::FloatDuration_T iNumberOfDays) {
    
    // Convert the number of days in number of seconds + number of milliseconds
    const stdair::FloatDuration_T lNumberOfSeconds =
      iNumberOfDays * stdair::SECONDS_IN_ONE_DAY;
    
    //
    const stdair::IntDuration_T lIntNumberOfSeconds =
      std::floor (lNumberOfSeconds);
    
    //
    const stdair::FloatDuration_T lNumberOfMilliseconds =
      (lNumberOfSeconds - lIntNumberOfSeconds)
      * stdair::MILLISECONDS_IN_ONE_SECOND;

    // +1 is a trick to ensure that the next Event is strictly later
    // than the current one
    const stdair::IntDuration_T lIntNumberOfMilliseconds =
      std::floor (lNumberOfMilliseconds) + 1;

    const stdair::Duration_T lDifferenceBetweenDepartureAndThisRequest =
      boost::posix_time::seconds (lIntNumberOfSeconds)
      + boost::posix_time::millisec (lIntNumberOfMilliseconds);

    return lDifferenceBetweenDepartureAndThisRequest;
  }

}
