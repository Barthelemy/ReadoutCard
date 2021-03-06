/// \file PdaDmaBuffer.cxx
/// \brief Implementation of the PdaDmaBuffer class.
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#include "PdaDmaBuffer.h"
#include <pda.h>
#include <InfoLogger/InfoLogger.hxx>
#include "ExceptionInternal.h"
#include "InterprocessLock.h"
#include "Pda/PdaLock.h"

namespace AliceO2 {
namespace roc {
namespace Pda {

PdaDmaBuffer::PdaDmaBuffer(PdaDevice::PdaPciDevice pciDevice, void* userBufferAddress, size_t userBufferSize,
    int dmaBufferId, bool requireHugepage) : mPciDevice(pciDevice)
{
  // Safeguard against PDA kernel module deadlocks, since it does not like parallel buffer registration
  try {
    Pda::PdaLock lock{};
  } catch (const LockException& e) {
    InfoLogger::InfoLogger() << "Failed to acquire PDA lock" << e.what() << InfoLogger::InfoLogger::endm;
    throw;
  }

  try {
    // Tell PDA we're using our already allocated userspace buffer.
    if (PciDevice_registerDMABuffer(pciDevice.get(), dmaBufferId, userBufferAddress, userBufferSize,
        &mDmaBuffer) != PDA_SUCCESS) {
      // Failed to register it. Usually, this means a DMA buffer wasn't cleaned up properly (such as after a crash).
      // So, try to clean things up.

      // Get the previous buffer
      DMABuffer* tempDmaBuffer;
      if (PciDevice_getDMABuffer(pciDevice.get(), dmaBufferId, &tempDmaBuffer) != PDA_SUCCESS) {
        // Give up
        BOOST_THROW_EXCEPTION(PdaException() << ErrorInfo::Message(
            "Failed to register external DMA buffer; Failed to get previous buffer for cleanup"));
      }

      // Free it
      if (PciDevice_deleteDMABuffer(pciDevice.get(), tempDmaBuffer) != PDA_SUCCESS) {
        // Give up
        BOOST_THROW_EXCEPTION(PdaException() << ErrorInfo::Message(
            "Failed to register external DMA buffer; Failed to delete previous buffer for cleanup"));
      }

      // Retry the registration of our new buffer
      if (PciDevice_registerDMABuffer(pciDevice.get(), dmaBufferId, userBufferAddress, userBufferSize,
          &mDmaBuffer) != PDA_SUCCESS) {
        // Give up
        BOOST_THROW_EXCEPTION(PdaException() << ErrorInfo::Message(
            "Failed to register external DMA buffer; Failed retry after automatic cleanup of previous buffer"));
      }
    }
  }
  catch (PdaException& e) {
    addPossibleCauses(e, {"Program previously exited without cleaning up DMA buffer, reinserting DMA kernel module may "
        " help, but ensure no channels are open before reinsertion (modprobe -r uio_pci_dma; modprobe uio_pci_dma"});
    throw;
  }

  try {
    DMABuffer_SGNode *sgList;
    if (DMABuffer_getSGList(mDmaBuffer, &sgList) != PDA_SUCCESS) {
      BOOST_THROW_EXCEPTION(PdaException() << ErrorInfo::Message("Failed to get scatter-gather list"));
    }

    auto node = sgList;
    while (node != nullptr) {
      if (requireHugepage) {
        size_t hugePageMinSize = 1024 * 1024 * 2; // 2 MiB, the smallest hugepage size
        if (node->length < hugePageMinSize) {
          BOOST_THROW_EXCEPTION(
            PdaException() << ErrorInfo::Message("Scatter-gather node smaller than 2 MiB (minimum hugepage"
              " size. This means the IOMMU is off and the buffer is not backed by hugepages - an unsupported buffer "
              "configuration."));
        }
      }

      ScatterGatherEntry e;
      e.size = node->length;
      e.addressUser = reinterpret_cast<uintptr_t>(node->u_pointer);
      e.addressBus = reinterpret_cast<uintptr_t>(node->d_pointer);
      e.addressKernel = reinterpret_cast<uintptr_t>(node->k_pointer);
      mScatterGatherVector.push_back(e);
      node = node->next;
    }

    if (mScatterGatherVector.empty()) {
      BOOST_THROW_EXCEPTION(PdaException() << ErrorInfo::Message(
        "Failed to initialize scatter-gather list, was empty"));
    }
  }
  catch (const PdaException& ) {
    PciDevice_deleteDMABuffer(mPciDevice.get(), mDmaBuffer);
    throw;
  }
}

PdaDmaBuffer::~PdaDmaBuffer()
{
  // Safeguard against PDA kernel module deadlocks, since it does not like parallel buffer registration
  // NOTE: not sure if necessary for deregistration as well
  try {
    Pda::PdaLock lock{};
  } catch (const LockException& e) {
    InfoLogger::InfoLogger() << "Failed to acquire PDA lock" << e.what() << InfoLogger::InfoLogger::endm;
    assert(false);
    //throw; (changed to assert(false) to get rid of the warning)
  }

  try {
    PciDevice_deleteDMABuffer(mPciDevice.get(), mDmaBuffer);
  } catch (std::exception& e) {
    // Nothing to be done?
    InfoLogger::InfoLogger() << "PdaDmaBuffer::~PdaDmaBuffer() failed: " << e.what() << InfoLogger::InfoLogger::endm;
  }
}

uintptr_t PdaDmaBuffer::getBusOffsetAddress(size_t offset) const
{
  const auto& list = mScatterGatherVector;

  auto userBase = list.at(0).addressUser;
  auto userWithOffset = userBase + offset;

  // First we find the SGL entry that contains our address
  for (size_t i = 0; i < list.size(); ++i) {
    auto entryUserStartAddress = list[i].addressUser;
    auto entryUserEndAddress = entryUserStartAddress + list[i].size;

    if ((userWithOffset >= entryUserStartAddress) && (userWithOffset < entryUserEndAddress)) {
      // This is the entry we need
      // We now need to calculate the difference from the start of this entry to the given offset. We make use of the
      // fact that the userspace addresses will be contiguous
      auto entryOffset = userWithOffset - entryUserStartAddress;
      auto offsetBusAddress = list[i].addressBus + entryOffset;
      return offsetBusAddress;
    }
  }

  BOOST_THROW_EXCEPTION(Exception()
      << ErrorInfo::Message("Physical offset address out of range")
      << ErrorInfo::Offset(offset));
}

} // namespace Pda
} // namespace roc
} // namespace AliceO2
