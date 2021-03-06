/// \file Gbt.h
/// \brief Definition of the Gbt class
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#ifndef ALICEO2_READOUTCARD_CRU_GBT_H_
#define ALICEO2_READOUTCARD_CRU_GBT_H_

#include "Pda/PdaBar.h"
#include "Constants.h"

namespace AliceO2 {
namespace roc {

class Gbt {
  
  using Link = Cru::Link;

  public: 
    Gbt(std::shared_ptr<Pda::PdaBar> pdaBar, std::vector<Link> mLinkList, int wrapperCount);
    void setMux(Link link, uint32_t mux);
    void setInternalDataGenerator(Link link, uint32_t value);
    void setTxMode(Link link, uint32_t mode);
    void setRxMode(Link link, uint32_t mode);
    void setLoopback(Link link, uint32_t enabled);
    void calibrateGbt();

  private:
    uint32_t getSourceSelectAddress(Link link);
    uint32_t getRxControlAddress(Link link);
    uint32_t getTxControlAddress(Link link);
    uint32_t getAtxPllRegisterAddress(int wrapper, uint32_t reg);

    void atxcal(uint32_t baseAddress=0x0);
    void cdrref(uint32_t refClock);
    void txcal();
    void rxcal();

    std::shared_ptr<Pda::PdaBar> mPdaBar;
    std::vector<Link> mLinkList;
    int mWrapperCount;
};
} // namespace roc
} // namespace AliceO2


#endif // ALICEO2_READOUTCARD_CRU_GBT_H_
