/// \file CruRegisterIndex.h
/// \brief Definition of CRU register indexes.
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#pragma once

#include <cstddef>

namespace AliceO2 {
namespace Rorc {

/// Namespace containing definitions of indexes for CRU BAR registers
/// They are based on the current best-guess understanding of both the CRU firmware and the code in here:
///   https://gitlab.cern.ch/alice-cru/pciedma_eval
/// Note that they are 32-bit indexes, not byte indexes
namespace CruRegisterIndex
{

/// Status table base address (low 32 bits)
/// Byte address: 0x0
static constexpr size_t STATUS_BASE_BUS_LOW = 0;

/// Status table base address (high 32 bits)
/// Byte address: 0x4
static constexpr size_t STATUS_BASE_BUS_HIGH = 1;

/// Status address in card (low 32 bits)
/// Byte address: 0x8
static constexpr size_t STATUS_BASE_CARD_LOW = 2;

/// Status address in card (high 32 bits)
/// Note: Appears to be unused, it's set to 0 in code examples
/// Byte address: 0xC
static constexpr size_t STATUS_BASE_CARD_HIGH = 3;

/// Set to number of available pages - 1
/// Byte address: 0x10
static constexpr size_t DMA_POINTER = 4;

/// Size of the descriptor table
/// Set to the same as (number of available pages - 1)
/// Used only if descriptor table size is other than 128
/// Byte address: 0x14
static constexpr size_t DESCRIPTOR_TABLE_SIZE = 5;

/// Control register for the way the done bit is set in status registers.
/// When register bit 0 is set, the status register's done bit will be set for each descriptor and a single MSI
/// interrupt will be sent after the final descriptor completes.
/// If not set, the done bit will be set only for the final descriptor.
/// Byte address: 0x18
static constexpr size_t DONE_CONTROL = 6;

/// Control register for the data emulator
/// Write 0x1 to this register to enable it
/// Byte address: 0x200
static constexpr size_t DATA_EMULATOR_CONTROL = 128;

/// Control register for PCI status
/// Write 0x1 to this register to signal that the host RAM is available for transfer
/// Byte address: 0x204
static constexpr size_t BUFFER_READY = 129;

/// Some kind of control register
/// One can "deassert reset for led module" by writing 0xd into this (not sure what that means).
/// This register does not appear to be necessary to use the LED on/off toggle functionality
/// Byte address: 0x220
static constexpr size_t LED_DEASSERT_RESET = 136;

/// Some kind of control register
/// One can "write data in led module" by writing 0x3 into this (not sure what that means).
/// This register does not appear to be necessary to use the LED on/off toggle functionality
/// Byte address: 0x230
static constexpr size_t LED_MODULE_DATA = 140;

/// Read status count
/// Byte address: 0x250
static constexpr size_t READ_STATUS_COUNT = 148;

/// Set to 0xff to turn the LED on, 0x00 to turn off
/// Byte address: 0x260
static constexpr size_t LED_STATUS = 152;

///// A read from this register will pop a value from the debug FIFO
///// Byte address: 0x270
//static constexpr size_t DEBUG_FIFO_POP = 156;
/// Board serial number
/// Byte address: 0x270
static constexpr size_t SERIAL_NUMBER = 156;


/// A write to this register will push a value into the debug FIFO
/// Byte address: 0x274
static constexpr size_t DEBUG_FIFO_PUSH = 157;

/// Register containing compilation info of the firmware
/// Can be used as a sort of version number
/// Byte address: 0x280
static constexpr size_t FIRMWARE_COMPILE_INFO = 160;

/// Reset control register
/// Write a 1 to reset the card
/// Byte address: 0x290
static constexpr size_t RESET_CONTROL = 164;

/// Temperature control & read register
/// Write 0x1 to reset
/// Write 0x0 to disable the reset
/// Write 0x2 to enable the temperature sampling
/// Read to get the temperature value
/// Byte address: 0x320
static constexpr size_t TEMPERATURE = 200;

/// Convert an index to a byte address
constexpr inline size_t toByteAddress(size_t address32)
{
  return address32 * 4;
}

} // namespace CruRegisterIndex
} // namespace Rorc
} // namespace AliceO2