#include <cctype>
#include <cstddef>
#include <memory_resource>
#include <string>
#include <cassert>
#include <fmt/core.h>
#include <fmt/color.h>

namespace pmrspy
{
  /**
   * \brief Use this if data is NOT primitive type, no data information available
   */
  class print_alloc : public std::pmr::memory_resource
  {
  public:
    print_alloc(const std::string name, std::pmr::memory_resource* upstream)
      : name_(std::move(name))
      , upstream_(upstream)
    {
      assert(upstream);
    }

  private:
    std::string name_;
    std::pmr::memory_resource* upstream_;

    void* do_allocate(std::size_t bytes, std::size_t alignment) override
    {
      fmt::print(fg(fmt::color::light_green), "[{} (alloc)] Size: {} Alignment: {} ...\n", name_, bytes, alignment);
      auto result = upstream_->allocate(bytes, alignment);
      return result;
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
    {
      fmt::print(fg(fmt::color::light_salmon), "[{} (dealloc)] Address: {} Dealloc Size: {} Alignment: {}\n",
        name_, p, bytes, alignment
      );
      upstream_->deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
    {
      return this == &other;
    }
  };

  /**
   * \brief Use this if data is primitive type, data information available
   */
  class print_alloc_data : public std::pmr::memory_resource
  {
  public:
    print_alloc_data(const std::string name, std::pmr::memory_resource* upstream)
      : name_(std::move(name))
      , upstream_(upstream)
    {
      assert(upstream);
    }

  private:
    std::string name_;
    std::pmr::memory_resource* upstream_;

    void* do_allocate(std::size_t bytes, std::size_t alignment) override
    {
      fmt::print(fg(fmt::color::light_green), "[{} (alloc)] Size: {} Alignment: {} ...\n", name_, bytes, alignment);
      auto result = upstream_->allocate(bytes, alignment);
      return result;
    }

    std::string format_destroyed_bytes(std::byte* p, const std::size_t size)
    {
      std::string result = "";
      bool in_string = false;

      auto format_char = [](bool& in_string, const char c, const char next)
      {
        auto format_byte = [](const char byte)
        {
          return fmt::format(" {:02x}", static_cast<unsigned char>(byte));
        };

        if (std::isprint(static_cast<int>(c)))
        {
          if (!in_string)
          {
            if (std::isprint(static_cast<int>(next)))
            {
              in_string = true;
              return fmt::format(" \"{}", c);
            }
            else
            {
              return format_byte(c);
            }
          }
          else
          {
            return std::string(1, c);
          }
        }
        else
        {
          if (in_string)
          {
            in_string = false;
            return '"' + format_byte(c);
          }
          return format_byte(c);
        }
      };

      std::size_t pos = 0;
      for (; pos < std::min(size - 1, static_cast<std::size_t>(32)); ++pos)
      {
        result += format_char(in_string, static_cast<char>(p[pos]),
          static_cast<char>(p[pos + 1]));
      }
      result += format_char(in_string, static_cast<char>(p[pos]), 0);
      if (in_string)
      {
        result += '"';
      }
      if (pos < (size - 1))
      {
        result += " <truncated...>";
      }
      return result;
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
    {
      fmt::print(fg(fmt::color::light_salmon), "[{} (dealloc)] Address: {} Dealloc Size: {} Alignment: {} Data: {}\n",
        name_, p, bytes, alignment, format_destroyed_bytes(static_cast<std::byte*>(p), bytes)
      );
      upstream_->deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
    {
      return this == &other;
    }
  };
} // pmrspy
