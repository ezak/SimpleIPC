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

#ifndef CC_EXECUTABLE_SHAREDMEMORY_H
#define CC_EXECUTABLE_SHAREDMEMORY_H

#include <cstddef>
#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <sys/mman.h>
#include <unistd.h>

template <typename T>
struct SharedMemory
{
  SharedMemory (const char *name, T header) : name_ (name), header_ (header)
  {
    fd_ = shm_open (name_, O_CREAT | O_RDWR, 0666);
    if (fd_ == -1)
      throw std::runtime_error ("shm_open failed");

    // Uses safely initialized fields from our stack instance 'header_'
    size_ = sizeof (T) + (header_.GetSlotSize () * header_.GetNumberOfSlots ());

    if (ftruncate (fd_, size_) == -1)
      {
        close (fd_);
        throw std::runtime_error ("ftruncate failed to size shared memory");
      }

    map_ = mmap (nullptr, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (map_ == MAP_FAILED)
      {
        close (fd_);
        throw std::runtime_error ("mmap failed");
      }

    // Bind the pointer to the region
    buffer_ = static_cast<T *> (map_);

    // Only run placement-new constructor if we are initializing it for the first time
    // This prevents a secondary process from wiping out head/tail markers!
    if (buffer_->GetSlotSize () != header_.GetSlotSize ())
      {
        ::new (map_) T (header_.GetSlotSize (), header_.GetNumberOfSlots ());
      }
  }

  ~SharedMemory ()
  {
    if (buffer_)
      buffer_->~T ();

    if (map_ && map_ != MAP_FAILED)
      munmap (map_, size_);

    if (fd_ != -1)
      close (fd_);

    // Automatically remove the shared memory file from /dev/shm/
    // so the next execution starts completely fresh from 0
    shm_unlink (name_);
  }

  SharedMemory (const SharedMemory &)            = delete;
  SharedMemory &operator= (const SharedMemory &) = delete;
  SharedMemory (SharedMemory &&)                 = delete;
  SharedMemory &operator= (SharedMemory &&)      = delete;

  bool
  Write (const uint8_t *data, size_t size)
  {
    return buffer_->Push (map_, data, size);
  }

  bool
  Read (uint8_t *data, size_t size)
  {
    return buffer_->Pop (map_, data, size);
  }

  void
  Stats () const
  {
    buffer_->Stat (map_, size_);
  }

private:
  // The order must be as follows matching the constructor
  const char *name_;
  T           header_;

  T          *buffer_;
  void       *map_;
  int         fd_;
  std::size_t size_;
};

#endif // CC_EXECUTABLE_SHAREDMEMORY_H
