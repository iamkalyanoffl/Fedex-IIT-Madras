#ifndef ENGINE_RESPONSE_OBJECTS_HPP_
#define ENGINE_RESPONSE_OBJECTS_HPP_

#include "engine/guidance/turn_instruction.hpp"
#include "extractor/travel_mode.hpp"
#include "engine/polyline_compressor.hpp"
#include "util/coordinate.hpp"
#include "util/json_container.hpp"

#include <boost/optional.hpp>

#include <string>
#include <algorithm>
#include <vector>

namespace osrm
{
namespace engine
{

struct Hint;

namespace guidance
{
struct RouteLeg;
struct RouteStep;
struct StepManeuver;
struct Route;
struct LegGeometry;
}

namespace api
{
namespace json
{
namespace detail
{

std::string instructionTypeToString(guidance::TurnType type);
std::string instructionModifierToString(guidance::DirectionModifier modifier);

util::json::Array coordinateToLonLat(const util::Coordinate &coordinate);

std::string modeToString(const extractor::TravelMode mode);

} // namespace detail

template <typename ForwardIter> util::json::String makePolyline(ForwardIter begin, ForwardIter end)
{
    util::json::String polyline;
    polyline.value = encodePolyline(begin, end);
    return polyline;
}

template <typename ForwardIter>
util::json::Object makeGeoJSONLineString(ForwardIter begin, ForwardIter end)
{
    util::json::Object geojson;
    geojson.values["type"] = "LineString";
    util::json::Array coordinates;
    std::transform(begin, end, std::back_inserter(coordinates.values),
                   [](const util::Coordinate loc)
                   {
                       return detail::coordinateToLonLat(loc);
                   });
    geojson.values["coordinates"] = std::move(coordinates);
    return geojson;
}

util::json::Object makeStepManeuver(const guidance::StepManeuver &maneuver);

util::json::Object makeRouteStep(guidance::RouteStep &&step,
                                 boost::optional<util::json::Value> geometry);

util::json::Object makeRoute(const guidance::Route &route,
                             util::json::Array &&legs,
                             boost::optional<util::json::Value> geometry);

util::json::Object
makeWaypoint(const util::Coordinate location, std::string &&name, const Hint &hint);

util::json::Object makeRouteLeg(guidance::RouteLeg &&leg, util::json::Array &&steps);

util::json::Array makeRouteLegs(std::vector<guidance::RouteLeg> &&legs,
                                const std::vector<guidance::LegGeometry> &leg_geometries);
}
}
} // namespace engine
} // namespace osrm

#endif // ENGINE_GUIDANCE_API_RESPONSE_GENERATOR_HPP_
