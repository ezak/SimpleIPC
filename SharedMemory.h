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
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>

template <typename T>
struct SharedMemory
{
  explicit SharedMemory (const char *name) : name_ (name)
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
        ::new (map_) T ();
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
  Write (const uint8_t *buffer, size_t size)
  {
    if (!buffer || size == 0)
      return false;

    if (size > buffer_->GetSlotSize ())
      {
        std::cerr << R"(Error: Payload size )" << size << " exceeds maximum slot size " << buffer_->GetSlotSize () << std::endl;
        return false;
      }

    const uint32_t current_head = buffer_->GetHead ();
    const uint32_t current_tail = buffer_->GetTail ();
    const uint32_t num_slots    = buffer_->GetNumberOfSlots ();

    if (((current_head + 1) % num_slots) == current_tail)
      return false; // Buffer overflow (Full) condition

    uint8_t *shm_base    = static_cast<uint8_t *> (map_) + sizeof (T);
    uint8_t *target_slot = shm_base + (current_head * buffer_->GetSlotSize ());

    std::memmove (target_slot, buffer, size);

    buffer_->SetHead ((current_head + 1) % num_slots);

    return true;
  }

  bool
  Read (uint8_t *buffer, size_t size)
  {
    if (!buffer || size == 0)
      {
        return false;
      }

    if (size < buffer_->GetSlotSize ())
      {
        std::cerr << "Error: Output buffer size " << size << " is too small for slot size " << buffer_->GetSlotSize () << std::endl;
        return false;
      }

    const uint32_t current_head = buffer_->GetHead ();
    const uint32_t current_tail = buffer_->GetTail ();
    const uint32_t num_slots    = buffer_->GetNumberOfSlots ();

    if (current_head == current_tail)
      {
        return false; // Underflow condition (nothing new to read)
      }

    uint8_t       *base        = static_cast<uint8_t *> (map_) + sizeof (T);
    const uint8_t *source_slot = base + (current_tail * buffer_->GetSlotSize ());

    std::memmove (buffer, source_slot, buffer_->GetSlotSize ());

    buffer_->SetTail ((current_tail + 1) % num_slots);

    return true;
  }

  void
  Stats () const
  {
    if (!buffer_)
      {
        std::cout << "[SHM Log] Shared memory region is not mapped.\n";
        return;
      }

    const uint32_t head  = buffer_->GetHead ();
    const uint32_t tail  = buffer_->GetTail ();
    const uint32_t slots = buffer_->GetNumberOfSlots ();

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
              << "  Mapped Address : " << map_ << "\n"
              << "  Total SHM Size : " << size_ << " bytes\n"
              << "  Head Index     : " << head << "\n"
              << "  Tail Index     : " << tail << "\n"
              << "  Slot Dimension : " << buffer_->GetSlotSize () << " bytes\n"
              << "  Capacity       : " << occupied_slots << " / " << slots << " slots used\n";

    if (head == tail)
      {
        std::cout << "  Status         : EMPTY\n";
      }
    else if (((head + 1) % slots) == tail)
      {
        std::cout << "  Status         : FULL (Block Warning)\n";
      }
    else
      {
        std::cout << "  Status         : ACTIVE\n";
      }
    std::cout << "=========================================" << std::endl;
  }

private:
  const char *name_;
  T           header_;
  std::size_t size_;
  int         fd_;
  void       *map_;
  T          *buffer_;
};

#endif // CC_EXECUTABLE_SHAREDMEMORY_H
