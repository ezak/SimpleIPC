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
RingHeader::Push (uint8_t *dst, uint8_t *src, uint32_t slot_size)
{

  return false;
}

bool
RingHeader::Pop (uint8_t *dst, uint8_t *src, uint32_t slot_size)
{
  return false;
}

