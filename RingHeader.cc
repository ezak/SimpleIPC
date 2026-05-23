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

uint32_t
RingHeader::GetHead () const
{
  return head_;
}

uint32_t
RingHeader::GetTail () const
{
  return tail_;
}

uint32_t
RingHeader::GetSlotSize () const
{
  return slot_size_ == 0 ? DEAFULT_SLOT_SIZE : slot_size_;
}

uint32_t
RingHeader::GetNumberOfSlots () const
{
  return number_of_slots_ == 0 ? DEAFULE_NUM_SLOTS : number_of_slots_;
}

void
RingHeader::SetHead (const uint32_t head)
{
  head_ = head;
}

void
RingHeader::SetTail (const uint32_t tail)
{
  tail_ = tail;
}

void
RingHeader::SetSlotSize (const uint32_t slot_size)
{
  slot_size_ = slot_size;
}

void
RingHeader::SetNumberOfSlots (const uint32_t number_of_slots)
{
  number_of_slots_ = number_of_slots;
}

bool
RingHeader::Push (void *map, const uint8_t *data, const uint32_t size)
{
  if (!data || size == 0)
    return false;

  if (size > GetSlotSize ())
    {
      std::cerr << R"(Error: Payload size )" << size << " exceeds maximum slot size " << GetSlotSize () << std::endl;
      return false;
    }

  const uint32_t current_head = GetHead ();
  const uint32_t current_tail = GetTail ();
  const uint32_t num_slots    = GetNumberOfSlots ();

  if (((current_head + 1) % num_slots) == current_tail)
    return false; // Buffer overflow (Full) condition

  uint8_t *shm_base    = static_cast<uint8_t *> (map) + sizeof (RingHeader);
  uint8_t *target_slot = shm_base + (current_head * GetSlotSize ());

  std::memmove (target_slot, data, size);

  SetHead ((current_head + 1) % num_slots);

  return true;
}

bool
RingHeader::Pop (void *map, uint8_t *data, const uint32_t size)
{
  if (!data || size == 0)
    {
      return false;
    }

  if (size < GetSlotSize ())
    {
      std::cerr << "Error: Output buffer size " << size << " is too small for slot size " << GetSlotSize () << std::endl;
      return false;
    }

  const uint32_t current_head = GetHead ();
  const uint32_t current_tail = GetTail ();
  const uint32_t num_slots    = GetNumberOfSlots ();

  if (current_head == current_tail)
    {
      return false; // Underflow condition (nothing new to read)
    }

  const uint8_t *base        = static_cast<uint8_t *> (map) + sizeof (RingHeader);
  const uint8_t *source_slot = base + (current_tail * GetSlotSize ());

  std::memmove (data, source_slot, GetSlotSize ());

  SetTail ((current_tail + 1) % num_slots);
  return true;
}

void
RingHeader::Stat (const void *map, const uint32_t size) const
{
  const uint32_t head  = GetHead ();
  const uint32_t tail  = GetTail ();
  const uint32_t slots = GetNumberOfSlots ();

  // Calculate how many slots are currently holding unread data
  uint32_t occupied_slots = 0;
  if (head >= tail)
    {
      occupied_slots = head - tail;
    }
  else
    {
      occupied_slots = (slots - tail) + head;
    }

  std::cout << "========= SHM RING BUFFER STATE =========" << "\n"
            << "  Mapped Address : " << map << "\n"
            << "  Total SHM Size : " << size << " bytes\n"
            << "  Head Index     : " << head << "\n"
            << "  Tail Index     : " << tail << "\n"
            << "  Slot Dimension : " << GetSlotSize () << " bytes\n"
            << "  Capacity       : " << occupied_slots << " / " << slots << " slots used\n";
  if (head == tail)
    std::cout << "  Status         : EMPTY\n";
  else if (((head + 1) % slots) == tail)
    std::cout << "  Status         : FULL (Block Warning)\n";
  else
    std::cout << "  Status         : ACTIVE\n";
  std::cout << "=========================================" << std::endl;
}
