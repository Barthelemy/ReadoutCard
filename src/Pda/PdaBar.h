/// \file PdaBar.h
/// \brief Definition of the PdaBar class.
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_SRC_READOUTCARD_PDA_PDABAR_H_
#define ALICEO2_SRC_READOUTCARD_PDA_PDABAR_H_

#include "ReadoutCard/BarInterface.h"
#include <pda.h>
#include "PdaDevice.h"
#include "ExceptionInternal.h"
#ifndef NDEBUG
# include <boost/type_index.hpp>
#endif
#include "Utilities/Util.h"

namespace AliceO2 {
namespace roc {
namespace Pda {

/// A simple wrapper around the PDA BAR object, providing some convenience functions
class PdaBar : public BarInterface
{
  public:
    PdaBar();

    PdaBar(PdaDevice::PdaPciDevice pciDevice, int barNumber);

    virtual uint32_t readRegister(int index)
    {
      return barRead<uint32_t>(index * sizeof(uint32_t));
    }

    virtual void writeRegister(int index, uint32_t value)
    {
      barWrite<uint32_t>(index * sizeof(uint32_t), value);
    }

    virtual void modifyRegister(int index, int position, int width, uint32_t value)
    {
      uint32_t regValue = barRead<uint32_t>(index * sizeof(uint32_t));
      Utilities::setBits(regValue, position, width, value);
      barWrite<uint32_t>(index * sizeof(uint32_t), regValue);
    }

    virtual int getIndex() const override
    {
      return mBarNumber;
    }

    virtual size_t getSize() const override
    {
      return mBarNumber;
    }

    virtual CardType::type getCardType() override
    {
      return CardType::Unknown;
    }

    template<typename T>
    void barWrite(uintptr_t byteOffset, const T &value) const
    {
//#ifndef NDEBUG
//      std::printf("PdaBar::barWrite<%s>(address=0x%lx, value=0x%x)\n",
//                  boost::typeindex::type_id<T>().pretty_name().c_str(), byteOffset, value);
//      std::fflush(stdout);
//#endif
      assertRange<T>(byteOffset);
      memcpy(getOffsetAddress(byteOffset), &value, sizeof(T));
    }

    template<typename T>
    T barRead(uintptr_t byteOffset) const
    {
//#ifndef NDEBUG
//      std::printf("PdaBar::barRead<%s>(address=0x%lx)\n",
//                  boost::typeindex::type_id<T>().pretty_name().c_str(), byteOffset);
//      std::fflush(stdout);
//#endif
      assertRange<T>(byteOffset);
      T value;
      memcpy(&value, getOffsetAddress(byteOffset), sizeof(T));
//#ifndef NDEBUG
//      std::printf("PdaBar::barRead<%s>(address=0x%lx) -> 0x%x\n",
//                  boost::typeindex::type_id<T>().pretty_name().c_str(), byteOffset, value);
//      std::fflush(stdout);
//#endif
      return value;
    }

    int getBarNumber() const
    {
      return mBarNumber;
    }

    size_t getBarLength() const
    {
      return mBarLength;
    }

    virtual boost::optional<int32_t> getSerial() override
    {
      return {};
    }
    
    virtual boost::optional<float> getTemperature() override
    {
      return {};
    }

    virtual boost::optional<std::string> getFirmwareInfo() override
    {
      return {};
    }

    virtual boost::optional<std::string> getCardId() override
    {
      return {};
    }

    virtual int32_t getDroppedPackets() override
    {
      return -1;
    }
 
    virtual uint32_t getCTPClock() override
    {
      return 0;
    }
    
    virtual uint32_t getLocalClock() override
    {
      return 0;
    }

    virtual int32_t getLinks() override
    {
      return 0;
    }

    virtual int32_t getLinksPerWrapper(int /*wrapper*/) override
    {
      return 0;
    }

    void configure() override
    {
      std::cout << "Configure invalid through the PDA BAR" << std::endl;
    }
    
  private:
    template<typename T>
    bool isInRange(size_t offset) const
    {
      return (mUserspaceAddress + offset + sizeof(T)) < (mUserspaceAddress + mBarLength);
    }

    template <typename T>
    void assertRange(uintptr_t offset) const
    {
      if (!isInRange<T>(offset)) {
        BOOST_THROW_EXCEPTION(Exception()
            << ErrorInfo::Message("BAR offset out of range")
            << ErrorInfo::BarIndex(offset)
            << ErrorInfo::BarSize(getBarLength()));
      }
    }

    void* getOffsetAddress(uintptr_t byteOffset) const
    {
      return reinterpret_cast<void*>(mUserspaceAddress + byteOffset);
    }

    /// PDA object for the PCI BAR
    Bar* mPdaBar;

    /// Length of the BAR
    size_t mBarLength;

    /// Index of the BAR
    int mBarNumber;

    /// Userspace addresses of the mapped BARs
    uintptr_t mUserspaceAddress;
};

} // namespace Pda
} // namespace roc
} // namespace AliceO2

#endif // ALICEO2_SRC_READOUTCARD_PDA_PDABAR_H_
