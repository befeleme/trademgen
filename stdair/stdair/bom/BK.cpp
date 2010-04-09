// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// STDAIR
#include <stdair/bom/Structure.hpp>
#include <stdair/bom/BK.hpp>

namespace stdair {

  // ////////////////////////////////////////////////////////////////////
  BK::BK (const BomKey_T& iKey, BomStructure_T& ioBKStructure)
    : BucketContent (iKey), _structure (ioBKStructure) {
  }

  // ////////////////////////////////////////////////////////////////////
  BK::~BK () {
  }
  
}
