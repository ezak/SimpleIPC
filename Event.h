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

#ifndef SIMPLEIPC_EVENT_H
#define SIMPLEIPC_EVENT_H

class Event
{
  int fd_;

public:
  Event ()  = default;
  ~Event () = default;

  [[nodiscard]] int  GetFd () const;
  [[nodiscard]] bool Read () const;
  [[nodiscard]] bool Write () const;
};

#endif // SIMPLEIPC_EVENT_H
