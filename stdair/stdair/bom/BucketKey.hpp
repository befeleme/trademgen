#ifndef __STDAIR_BOM_BUCKETKEY_HPP
#define __STDAIR_BOM_BUCKETKEY_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STDAIR
#include <stdair/STDAIR_Types.hpp>
#include <stdair/bom/BomKey.hpp>
#include <stdair/bom/LegCabinKey.hpp>

namespace stdair {
  /** Key of booking-class. */
  struct BucketKey_T : public BomKey_T {
    
  public:
    // /////////// Typedefs ////////////
    /** Definition allowing to retrieve the parent key type. */
    typedef LegCabinKey_T ParentKey_T;
    
  public:
    // /////////// Construction ///////////
    /** Constructor. */
    BucketKey_T (const ClassCode_T& iClassCode);

    /** Destructor. */
    ~BucketKey_T ();
    
    // /////////// Getters //////////
    /** Get the class code. */
    const ClassCode_T& getClassCode () const {
      return _classCode;
    }
    
    // /////////// Setters /////////////
    void setParentKey (const ParentKey_T& iParentKey) {
      _parentKey = iParentKey;
    }
    
    // /////////// Display support methods /////////
    /** Dump a Business Object Key into an output stream.
        @param ostream& the output stream. */
    void toStream (std::ostream& ioOut) const;

    /** Read a Business Object Key from an input stream.
        @param istream& the input stream. */
    void fromStream (std::istream& ioIn);

   /** Get the serialised version of the Business Object Key.
       <br>That string is unique, at the level of a given Business Object,
       when among children of a given parent Business Object.
       <br>For instance, "H" and "K" allow to differentiate among two
       marketing classes for the same segment-cabin. */
    const std::string toString() const;
    
    /** Display of the key. */
    const std::string describe() const;
    
  private:
    // Attributes
    /** Leg-cabin key.*/
    ParentKey_T _parentKey;
    
    /** Cabin code. */
    ClassCode_T _classCode;
  };

}
#endif // __STDAIR_BOM_BUCKETKEY_HPP
