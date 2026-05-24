/*
 * Created by izak on 5/23/26.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "RingHeader.h"

#include <cstring>
#include <iostream>

/**
 *
 * @return slot_size_
 */
uint32_t
RingHeader::GetSlotSize () const
{
  return slot_size_;
}

/**
 *
 * @return number_of_slots_
 */
uint32_t
RingHeader::GetNumberOfSlots () const
{
  return number_of_slots_;
}

/**
 *
 * @param map A void pointer to the Shared Memory file discriptor
 * @param data A pointer to which the value in the ring buffer is poped
 * @param size The size of the Shared Memory
 * @return On success return true else returnes false
 */
bool
RingHeader::Push (void *map, const uint8_t *data, const uint32_t size)
{
  if (!data || size == 0)
    return false;

  if (size > slot_size_)
    {
      std::cerr << R"(Error: Payload size )" << size << " exceeds maximum slot size " << slot_size_ << std::endl;
      return false;
    }

  const uint32_t current_head = head_;
  const uint32_t current_tail = tail_;
  const uint32_t num_slots    = number_of_slots_;

  if (((current_head + 1) % num_slots) == current_tail)
    return false; // Buffer overflow (Full) condition

  uint8_t *shm_base    = static_cast<uint8_t *> (map) + sizeof (RingHeader);
  uint8_t *target_slot = shm_base + (current_head * slot_size_);

  std::memmove (target_slot, data, size);

  head_ = (current_head + 1) % num_slots;

  return true;
}

/**
 *
 * @param map A void pointer to the Shared Memory file discriptor
 * @param data A pointer to which the value in the ring buffer is poped
 * @param size The size of the Shared Memory
 * @return On success return true else returnes false
 */
bool
RingHeader::Pop (void *map, uint8_t *data, const uint32_t size)
{
  if (!data || size == 0)
    {
      return false;
    }

  if (size < slot_size_)
    {
      std::cerr << "Error: Output buffer size " << size << " is too small for slot size " << slot_size_ << std::endl;
      return false;
    }

  const uint32_t current_head = head_;
  const uint32_t current_tail = tail_;
  const uint32_t num_slots    = number_of_slots_;

  if (current_head == current_tail)
    {
      return false; // Underflow condition (nothing new to read)
    }

  const uint8_t *base        = static_cast<uint8_t *> (map) + sizeof (RingHeader);
  const uint8_t *source_slot = base + (current_tail * slot_size_);

  std::memmove (data, source_slot, slot_size_);

  tail_ = (current_tail + 1) % num_slots;

  return true;
}

/**
 *
 * @param map A pointer to the map of the shared memory file discriptor
 * @param size The size of the Shared Memory
 */
void
RingHeader::Stat (const void *map, const uint32_t size) const
{
  const uint32_t current_head = head_;
  const uint32_t current_tail = tail_;
  const uint32_t num_slots    = number_of_slots_;

  // Calculate how many slots are currently holding unread data
  uint32_t occupied_slots = 0;
  if (current_head >= current_tail)
    {
      occupied_slots = current_head - current_tail;
    }
  else
    {
      occupied_slots = (num_slots - current_tail) + current_head;
    }

  std::cout << "========= SHM RING BUFFER STATE =========" << "\n"
            << "  Mapped Address : " << map << "\n"
            << "  Total SHM Size : " << size << " bytes\n"
            << "  Head Index     : " << current_head << "\n"
            << "  Tail Index     : " << current_tail << "\n"
            << "  Slot Dimension : " << slot_size_ << " bytes\n"
            << "  Capacity       : " << occupied_slots << " / " << num_slots << " slots used\n";
  if (current_head == current_tail)
    std::cout << "  Status         : EMPTY\n";
  else if (((current_head + 1) % num_slots) == current_tail)
    std::cout << "  Status         : FULL (Block Warning)\n";
  else
    std::cout << "  Status         : ACTIVE\n";
  std::cout << "=========================================" << std::endl;
}
