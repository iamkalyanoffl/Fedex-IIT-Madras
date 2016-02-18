#include "server/service/nearest_service.hpp"

#include "engine/api/nearest_parameters.hpp"
#include "server/api/parameters_parser.hpp"

#include "util/json_container.hpp"

#include <boost/format.hpp>

namespace osrm
{
namespace server
{
namespace service
{

engine::Status NearestService::RunQuery(std::vector<util::FixedPointCoordinate> coordinates,
                                        std::string &options,
                                        util::json::Object &result)
{
    // TODO(daniel-j-h)
    return Status::Error;
}
}
}
}
