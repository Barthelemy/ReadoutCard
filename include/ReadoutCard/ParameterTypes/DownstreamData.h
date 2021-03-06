/// \file DownstreamData.h
/// \brief Definition of the DownstreamData enum and supporting functions.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#ifndef ALICEO2_INCLUDE_READOUTCARD_CRU_DOWNSTREAMDATA_H_
#define ALICEO2_INCLUDE_READOUTCARD_CRU_DOWNSTREAMDATA_H_

#include <string>
#include <Cru/Constants.h>

namespace AliceO2 {
namespace roc {

/// Namespace for the ROC GBT mux enum, and supporting functions
struct DownstreamData
{
    enum type
    {
      Ctp = Cru::Registers::DATA_CTP,
      Pattern = Cru::Registers::DATA_PATTERN,
      Midtrg = Cru::Registers::DATA_MIDTRG,
    };

    /// Converts a DownstreamData to an int
    static std::string toString(const DownstreamData::type& gbtMux);

    /// Converts a string to a DownstreamData
    static DownstreamData::type fromString(const std::string& string);
};

} // namespace roc
} // namespace AliceO2

#endif // ALICEO2_INCLUDE_READOUTCARD_CRU_DOWNSTREAMDATA_H_
