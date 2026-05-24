/*
 * Created by izak on 5/22/26.
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

#include "Event.h"
#include "RingHeader.h"
#include "SharedMemory.h"

#include <memory>

int
main ()
{
  auto event = std::make_shared<Event> ();

  const auto shared_memory = std::make_shared<SharedMemory<RingHeader>> ("nemo", RingHeader{256, 8});

  constexpr uint8_t d[2] = { 0x01, 0x02 };
  auto              res  = shared_memory->Write (d, 2);
  if (res)
    shared_memory->Stats ();

  constexpr uint8_t dummy_data[256] = { 0xAA };
  res                               = shared_memory->Write (dummy_data, 256);

  if (res)
    shared_memory->Stats ();
}
