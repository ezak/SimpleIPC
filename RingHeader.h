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

#ifndef SIMPLEIPC_RINGHEADER_H
#define SIMPLEIPC_RINGHEADER_H

#include <cstdint>

struct RingHeader
{
  /**
   *
   * @param slot_size Number of slots for the data
   * @param number_of_slots Max size allocated per FlatBuffer payload
   */
  RingHeader (const uint32_t slot_size, const uint32_t number_of_slots) : slot_size_ (slot_size), number_of_slots_ (number_of_slots) {}
  ~RingHeader () = default;


  [[nodiscard]] uint32_t GetSlotSize () const;
  [[nodiscard]] uint32_t GetNumberOfSlots () const;

  [[nodiscard]] bool Push (void *map, const uint8_t *data, uint32_t size);
  [[nodiscard]] bool Pop (void *map, uint8_t *data, uint32_t size);
  void               Stat (const void *map, uint32_t size) const;

private:
  uint32_t head_{ 0 };
  uint32_t tail_{ 0 };
  uint32_t slot_size_{ 0 };
  uint32_t number_of_slots_{ 0 };
};

#endif // SIMPLEIPC_RINGHEADER_H
