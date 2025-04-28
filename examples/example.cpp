#include "pmrspy/pmrspy.hpp"
#include <array>

template <typename Container, typename... Values>
auto create_container(auto* resource, Values&&... values) {
  Container result{resource};
  result.reserve(sizeof...(values));
  (result.emplace_back(std::forward<Values>(values)), ...);
  return result;
};

int main() {
  pmrspy::print_alloc_data default_alloc{"Rogue PMR Allocation!", std::pmr::null_memory_resource()};
  std::pmr::set_default_resource(&default_alloc);

  pmrspy::print_alloc_data oom{"Out of Memory", std::pmr::null_memory_resource()};

  std::array<std::uint8_t, 32768> buffer{};
  std::pmr::monotonic_buffer_resource
    underlying_bytes(buffer.data(), buffer.size(), &oom);

  pmrspy::print_alloc_data monotonic{"Monotonic Array", &underlying_bytes};

  std::pmr::unsynchronized_pool_resource unsync_pool(&monotonic);

  pmrspy::print_alloc_data pool("Pool", &unsync_pool);

  for (auto i = 0uz; i < 10; ++i)
  {
    fmt::print("Starting Loop Iteration\n");
    auto vec = create_container<std::pmr::vector<std::pmr::string>>(
        &pool, "Hello", "World", "Hello Long String", "Another Long String");
    fmt::print("Emplacing Long String\n");
    vec.emplace_back("a different long string");
    fmt::print("Emplacing Long String\n");
    vec.emplace_back("a different long string 1");
    fmt::print("Emplacing Long String\n");
    vec.emplace_back("a different long string");
    fmt::print("Emplacing Long String\n");
    vec.emplace_back("a different long string");
    fmt::print("Emplacing Short String\n");
    vec.emplace_back("bob");
    fmt::print("Emplacing Short String\n");
    vec.emplace_back("was");
    fmt::print("Erasing First Element\n");
    vec.erase(vec.begin());
    fmt::print("Erasing First Element\n");
    vec.erase(vec.begin());
    fmt::print("Erasing First Element\n");
    vec.erase(vec.begin());
    fmt::print("Finishing Loop Iteration\n");
  }

  fmt::print("Exiting Main");
}
