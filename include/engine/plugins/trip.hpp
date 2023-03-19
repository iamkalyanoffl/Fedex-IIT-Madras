#ifndef TRIP_HPP
#define TRIP_HPP

#include "engine/plugins/plugin_base.hpp"

#include "engine/api/trip_parameters.hpp"
#include "engine/routing_algorithms.hpp"

#include "util/json_container.hpp"

#include <boost/assert.hpp>

#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace osrm::engine::plugins
{

class TripPlugin final : public BasePlugin
{
  private:
    const int max_locations_trip;

    InternalRouteResult ComputeRoute(const RoutingAlgorithmsInterface &algorithms,
                                     const std::vector<PhantomNodeCandidates> &candidates_list,
                                     const std::vector<NodeID> &trip,
                                     const bool roundtrip) const;

  public:
    explicit TripPlugin(const int max_locations_trip_, const boost::optional<double> default_bearing_radius) : max_locations_trip(max_locations_trip_) {
      this->default_bearing_radius = default_bearing_radius;
    }

    Status HandleRequest(const RoutingAlgorithmsInterface &algorithms,
                         const api::TripParameters &parameters,
                         osrm::engine::api::ResultT &json_result) const;
};
} // namespace osrm::engine::plugins

#endif // TRIP_HPP
