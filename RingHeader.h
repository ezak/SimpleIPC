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

#ifndef CC_EXECUTABLE_RINGHEADER_H
#define CC_EXECUTABLE_RINGHEADER_H

#include <cstdint>

#define DEAFULE_NUM_SLOTS 8
#define DEAFULT_SLOT_SIZE 256 // Max size allocated per FlatBuffer payload

struct RingHeader
{

  RingHeader ()  = default;
  ~RingHeader () = default;

  [[nodiscard]] uint32_t GetHead () const;
  [[nodiscard]] uint32_t GetTail () const;
  [[nodiscard]] uint32_t GetSlotSize () const;
  [[nodiscard]] uint32_t GetNumberOfSlots () const;

  void SetHead (uint32_t head);
  void SetTail (uint32_t tail);
  void SetSlotSize (uint32_t slot_size);
  void SetNumberOfSlots (uint32_t number_of_slots);

  [[nodiscard]] bool Push (void *map, const uint8_t *data, uint32_t size);
  [[nodiscard]] bool Pop (void *map, uint8_t *data, uint32_t size);
  void               Stat (const void *map, uint32_t size) const;

private:
  uint32_t head_{ 0 };
  uint32_t tail_{ 0 };
  uint32_t slot_size_{ 0 };
  uint32_t number_of_slots_{ 0 };
};

#endif // CC_EXECUTABLE_RINGHEADER_H
