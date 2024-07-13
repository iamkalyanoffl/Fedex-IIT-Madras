#ifndef OSMIUM_HANDLER_NODE_LOCATIONS_FOR_WAYS_HPP
#define OSMIUM_HANDLER_NODE_LOCATIONS_FOR_WAYS_HPP

/*

This file is part of Osmium (https://osmcode.org/libosmium).

Copyright 2013-2023 Jochen Topf <jochen@topf.org> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <osmium/handler.hpp>
#include <osmium/index/index.hpp>
#include <osmium/index/map/dummy.hpp>
#include <osmium/index/node_locations_map.hpp>
#include <osmium/osm/location.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/node_ref.hpp>
#include <osmium/osm/types.hpp>
#include <osmium/osm/way.hpp>

#include <limits>
#include <type_traits>

namespace osmium {

    namespace handler {

        using dummy_type = osmium::index::map::Dummy<osmium::unsigned_object_id_type, osmium::Location>;

        /**
         * Handler to retrieve locations from nodes and add them to ways.
         *
         * @tparam TStoragePosIDs Class that handles the actual storage of the node locations
         *                        (for positive IDs). It must support the set(id, value) and
         *                        get(id) methods.
         * @tparam TStorageNegIDs Same but for negative IDs.
         */
        template <typename TStoragePosIDs, typename TStorageNegIDs = dummy_type>
        class NodeLocationsForWays : public osmium::handler::Handler {

            template <typename T>
            using based_on_map = std::is_base_of<osmium::index::map::Map<osmium::unsigned_object_id_type, osmium::Location>, T>;

            static_assert(based_on_map<TStoragePosIDs>::value, "Index class must be derived from osmium::index::map::Map<osmium::unsigned_object_id_type, osmium::Location>");
            static_assert(based_on_map<TStorageNegIDs>::value, "Index class must be derived from osmium::index::map::Map<osmium::unsigned_object_id_type, osmium::Location>");

        public:

            using index_pos_type = TStoragePosIDs;
            using index_neg_type = TStorageNegIDs;

        private:

            /// Object that handles the actual storage of the node locations (with positive IDs).
            TStoragePosIDs& m_storage_pos;

            /// Object that handles the actual storage of the node locations (with negative IDs).
            TStorageNegIDs& m_storage_neg;

            osmium::unsigned_object_id_type m_last_id = 0;

            bool m_ignore_errors = false;

            bool m_must_sort = false;

            // It is okay to have this static dummy instance, even when using several threads,
            // because it is read-only.
            static dummy_type& get_dummy() {
                static dummy_type instance;
                return instance;
            }

        public:

            explicit NodeLocationsForWays(TStoragePosIDs& storage_pos,
                                          TStorageNegIDs& storage_neg = get_dummy()) noexcept :
                m_storage_pos(storage_pos),
                m_storage_neg(storage_neg) {
            }

            NodeLocationsForWays(const NodeLocationsForWays&) = delete;
            NodeLocationsForWays& operator=(const NodeLocationsForWays&) = delete;

            NodeLocationsForWays(NodeLocationsForWays&&) noexcept = default;
            NodeLocationsForWays& operator=(NodeLocationsForWays&&) noexcept = default;

            ~NodeLocationsForWays() noexcept = default;

            void ignore_errors() {
                m_ignore_errors = true;
            }

            TStoragePosIDs& storage_pos() noexcept {
                return m_storage_pos;
            }

            TStorageNegIDs& storage_neg() noexcept {
                return m_storage_neg;
            }

            /**
             * Store the location of the node in the storage.
             */
            void node(const osmium::Node& node) {
                if (node.positive_id() < m_last_id) {
                    m_must_sort = true;
                }
                m_last_id = node.positive_id();

                const auto id = node.id();
                if (id >= 0) {
                    m_storage_pos.set(static_cast<osmium::unsigned_object_id_type>( id), node.location());
                } else {
                    m_storage_neg.set(static_cast<osmium::unsigned_object_id_type>(-id), node.location());
                }
            }

            /**
             * Get location of node with given id.
             */
            osmium::Location get_node_location(const osmium::object_id_type id) const {
                if (id >= 0) {
                    return m_storage_pos.get_noexcept(static_cast<osmium::unsigned_object_id_type>(id));
                }
                return m_storage_neg.get_noexcept(static_cast<osmium::unsigned_object_id_type>(-id));
            }

            /**
             * Retrieve locations of all nodes in the way from storage and add
             * them to the way object.
             */
            void way(osmium::Way& way) {
                if (m_must_sort) {
                    m_storage_pos.sort();
                    m_storage_neg.sort();
                    m_must_sort = false;
                    m_last_id = std::numeric_limits<osmium::unsigned_object_id_type>::max();
                }
                bool error = false;
                for (auto& node_ref : way.nodes()) {
                    node_ref.set_location(get_node_location(node_ref.ref()));
                    if (!node_ref.location()) {
                        error = true;
                    }
                }
                if (!m_ignore_errors && error) {
                    throw osmium::not_found{"location for one or more nodes not found in node location index"};
                }
            }

            /**
             * Call clear on the location indexes. Makes the
             * NodeLocationsForWays handler unusable. Used to explicitly free
             * memory if thats needed.
             */
            void clear() {
                m_storage_pos.clear();
                m_storage_neg.clear();
            }

        }; // class NodeLocationsForWays

    } // namespace handler

} // namespace osmium

#endif // OSMIUM_HANDLER_NODE_LOCATIONS_FOR_WAYS_HPP
