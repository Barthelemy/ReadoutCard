/// \file PdaBar.h
/// \brief Definition of the PdaBar class.
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#pragma once

#include <pda.h>

#include "RorcException.h"

namespace AliceO2 {
namespace Rorc {
namespace Pda {

/// A simple wrapper around the PDA BAR object, providing some convenience functions
class PdaBar
{
  public:
    PdaBar();
    PdaBar(PciDevice* pciDevice, int channel);

    inline size_t getBarLength() const
    {
      return mBarLength;
    }

    inline Bar* getPdaBar() const
    {
      return mPdaBar;
    }

    inline volatile void* getUserspaceAddress() const
    {
      return mUserspaceAddress;
    }

    inline volatile uint32_t* getUserspaceAddressU32() const
    {
      return reinterpret_cast<volatile uint32_t*>(mUserspaceAddress);
    }

    /// Get reference to register
    inline volatile uint32_t& operator[](size_t i)
    {
      return getUserspaceAddressU32()[i];
    }

  private:
    /// PDA object for the PCI BAR
    Bar* mPdaBar;

    /// Length of the BAR
    size_t mBarLength;

    /// Userspace addresses of the mapped BARs
    volatile void* mUserspaceAddress;
};

} // namespace Pda
} // namespace Rorc
} // namespace AliceO2