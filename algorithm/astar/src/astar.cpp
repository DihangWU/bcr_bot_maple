#include "bcr_bot/astar/astar.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <queue>

namespace bcr_bot::astar {
namespace {
constexpr std::uint8_t kUnknown = 255;
constexpr double kSqrt2 = 1.4142135623730951;
struct Entry { std::size_t index; double g; double f; };
struct Compare { bool operator()(const Entry & a, const Entry & b) const { return a.f == b.f ? a.g < b.g : a.f > b.f; } };
bool freeCell(std::uint8_t c, const Options & o) { return c == kUnknown ? o.allow_unknown : c < o.lethal_cost; }
double h(int x, int y, int gx, int gy, bool diagonal) {
  const double dx = std::abs(gx - x), dy = std::abs(gy - y);
  return diagonal ? std::max(dx, dy) + (kSqrt2 - 1.0) * std::min(dx, dy) : dx + dy;
}
}
SearchResult AStar::search(const Grid & grid, std::size_t sx, std::size_t sy,
  std::size_t gx, std::size_t gy, const Options & o, const CancelChecker & cancel) const
{
  SearchResult r;
  if (!grid.width || !grid.height || grid.height > std::numeric_limits<std::size_t>::max() / grid.width ||
    grid.costs.size() != grid.width * grid.height || !o.lethal_cost) { r.status = SearchStatus::kInvalidGrid; return r; }
  if (sx >= grid.width || sy >= grid.height) { r.status = SearchStatus::kStartOutOfBounds; return r; }
  if (gx >= grid.width || gy >= grid.height) { r.status = SearchStatus::kGoalOutOfBounds; return r; }
  const std::size_t start = sy * grid.width + sx, goal = gy * grid.width + gx;
  if (!freeCell(grid.costs[start], o)) { r.status = SearchStatus::kStartOccupied; return r; }
  if (!freeCell(grid.costs[goal], o)) { r.status = SearchStatus::kGoalOccupied; return r; }
  if (start == goal) { r.status = SearchStatus::kSuccess; r.path = {start}; return r; }
  const std::size_t none = std::numeric_limits<std::size_t>::max();
  const double inf = std::numeric_limits<double>::infinity();
  std::vector<double> g(grid.costs.size(), inf); std::vector<std::size_t> parent(grid.costs.size(), none);
  std::vector<bool> closed(grid.costs.size(), false);
  std::priority_queue<Entry, std::vector<Entry>, Compare> open;
  g[start] = 0.0; open.push({start, 0.0, h(static_cast<int>(sx), static_cast<int>(sy), static_cast<int>(gx), static_cast<int>(gy), o.use_diagonal)});
  constexpr std::array<std::pair<int, int>, 8> dirs{{{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}}};
  const std::size_t n_dirs = o.use_diagonal ? 8U : 4U;
  while (!open.empty()) {
    if ((r.expanded_nodes & 255U) == 0U && cancel && cancel()) { r.status = SearchStatus::kCancelled; return r; }
    const Entry cur = open.top(); open.pop();
    if (closed[cur.index] || cur.g > g[cur.index]) continue;
    if (cur.index == goal) {
      for (std::size_t p = goal; p != none; p = parent[p]) { r.path.push_back(p); if (p == start) break; }
      if (r.path.empty() || r.path.back() != start) { r.path.clear(); r.status = SearchStatus::kNoPath; return r; }
      std::reverse(r.path.begin(), r.path.end()); r.status = SearchStatus::kSuccess; return r;
    }
    closed[cur.index] = true; ++r.expanded_nodes;
    const int x = static_cast<int>(cur.index % grid.width), y = static_cast<int>(cur.index / grid.width);
    for (std::size_t i = 0; i < n_dirs; ++i) {
      const int dx = dirs[i].first, dy = dirs[i].second, nx = x + dx, ny = y + dy;
      if (nx < 0 || ny < 0 || nx >= static_cast<int>(grid.width) || ny >= static_cast<int>(grid.height)) continue;
      const std::size_t ni = static_cast<std::size_t>(ny) * grid.width + static_cast<std::size_t>(nx);
      if (closed[ni] || !freeCell(grid.costs[ni], o)) continue;
      if (dx && dy && o.prevent_corner_cutting) {
        const std::size_t a = static_cast<std::size_t>(y) * grid.width + static_cast<std::size_t>(nx);
        const std::size_t b = static_cast<std::size_t>(ny) * grid.width + static_cast<std::size_t>(x);
        if (!freeCell(grid.costs[a], o) || !freeCell(grid.costs[b], o)) continue;
      }
      const auto c = grid.costs[ni] == kUnknown ? static_cast<std::uint8_t>(o.lethal_cost - 1U) : grid.costs[ni];
      const double step = (dx && dy ? kSqrt2 : 1.0) * (1.0 + o.cost_penalty * static_cast<double>(c) / static_cast<double>(o.lethal_cost - 1U));
      const double ng = g[cur.index] + step;
      if (ng >= g[ni]) continue;
      parent[ni] = cur.index; g[ni] = ng;
      open.push({ni, ng, ng + h(nx, ny, static_cast<int>(gx), static_cast<int>(gy), o.use_diagonal)});
    }
  }
  r.status = SearchStatus::kNoPath; return r;
}
}  // namespace bcr_bot::astar
