#ifndef BCR_BOT__ASTAR__ASTAR_HPP_
#define BCR_BOT__ASTAR__ASTAR_HPP_

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace bcr_bot::astar
{
struct Grid { std::size_t width{0}; std::size_t height{0}; std::vector<std::uint8_t> costs; };
struct Options {
  bool allow_unknown{true}; bool use_diagonal{true}; bool prevent_corner_cutting{true};
  std::uint8_t lethal_cost{253}; double cost_penalty{2.0};
};
enum class SearchStatus { kSuccess, kInvalidGrid, kStartOutOfBounds, kGoalOutOfBounds,
  kStartOccupied, kGoalOccupied, kNoPath, kCancelled };
struct SearchResult { SearchStatus status{SearchStatus::kNoPath}; std::vector<std::size_t> path; std::size_t expanded_nodes{0}; };
class AStar {
public:
  using CancelChecker = std::function<bool()>;
  SearchResult search(const Grid &, std::size_t, std::size_t, std::size_t, std::size_t,
    const Options &, const CancelChecker & = {}) const;
};
}  // namespace bcr_bot::astar
#endif
