#ifndef OSMIUM_DYNAMIC_HANDLER_HPP
#define OSMIUM_DYNAMIC_HANDLER_HPP

/*

This file is part of Osmium (https://osmcode.org/libosmium).

Copyright 2013-2022 Jochen Topf <jochen@topf.org> and others (see README).

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

#include <memory>
#include <utility>

namespace osmium {

    class Node;
    class Way;
    class Relation;
    class Area;
    class Changeset;

    namespace handler {

        namespace detail {

            class HandlerWrapperBase {

            public:

                HandlerWrapperBase() = default;

                HandlerWrapperBase(const HandlerWrapperBase&) = default;
                HandlerWrapperBase& operator=(const HandlerWrapperBase&) = default;

                HandlerWrapperBase(HandlerWrapperBase&&) noexcept = default;
                HandlerWrapperBase& operator=(HandlerWrapperBase&&) noexcept = default;

                virtual ~HandlerWrapperBase() noexcept = default;

                virtual void node(const osmium::Node& /*node*/) {
                }

                virtual void way(const osmium::Way& /*way*/) {
                }

                virtual void relation(const osmium::Relation& /*relation*/) {
                }

                virtual void area(const osmium::Area& /*area*/) {
                }

                virtual void changeset(const osmium::Changeset& /*changeset*/) {
                }

                virtual void flush() {
                }

            }; // class HandlerWrapperBase


            // The following uses trick from
            // https://stackoverflow.com/questions/257288/is-it-possible-to-write-a-c-template-to-check-for-a-functions-existence
            // to either call handler style functions or visitor style operator().

#define OSMIUM_DYNAMIC_HANDLER_DISPATCH(_name_, _type_) \
template <typename THandler> \
auto _name_##_dispatch(THandler& handler, const osmium::_type_& object, int) -> decltype(handler._name_(object), void()) { \
    handler._name_(object); \
} \
template <typename THandler> \
auto _name_##_dispatch(THandler& handler, const osmium::_type_& object, long) -> decltype(handler(object), void()) { \
    handler(object); \
}

            OSMIUM_DYNAMIC_HANDLER_DISPATCH(node, Node)
            OSMIUM_DYNAMIC_HANDLER_DISPATCH(way, Way)
            OSMIUM_DYNAMIC_HANDLER_DISPATCH(relation, Relation)
            OSMIUM_DYNAMIC_HANDLER_DISPATCH(changeset, Changeset)
            OSMIUM_DYNAMIC_HANDLER_DISPATCH(area, Area)

            template <typename THandler>
            auto flush_dispatch(THandler& handler, int /*dispatch*/) -> decltype(handler.flush(), void()) {
                handler.flush();
            }

            template <typename THandler>
            void flush_dispatch(THandler& /*handler*/, long /*dispatch*/) { // NOLINT(google-runtime-int)
            }

            template <typename THandler>
            class HandlerWrapper : public HandlerWrapperBase {

                THandler m_handler;

            public:

                template <typename... TArgs>
                explicit HandlerWrapper(TArgs&&... args) :
                    m_handler(std::forward<TArgs>(args)...) {
                }

                HandlerWrapper(const HandlerWrapper&) = default;
                HandlerWrapper& operator=(const HandlerWrapper&) = default;

                HandlerWrapper(HandlerWrapper&&) noexcept = default;
                HandlerWrapper& operator=(HandlerWrapper&&) noexcept = default;

                ~HandlerWrapper() noexcept override = default;

                void node(const osmium::Node& node) final {
                    node_dispatch(m_handler, node, 0);
                }

                void way(const osmium::Way& way) final {
                    way_dispatch(m_handler, way, 0);
                }

                void relation(const osmium::Relation& relation) final {
                    relation_dispatch(m_handler, relation, 0);
                }

                void area(const osmium::Area& area) final {
                    area_dispatch(m_handler, area, 0);
                }

                void changeset(const osmium::Changeset& changeset) final {
                    changeset_dispatch(m_handler, changeset, 0);
                }

                void flush() final {
                    flush_dispatch(m_handler, 0);
                }

            }; // class HandlerWrapper

        } // namespace detail

        class DynamicHandler : public osmium::handler::Handler {

            using impl_ptr = std::unique_ptr<osmium::handler::detail::HandlerWrapperBase>;
            impl_ptr m_impl;

        public:

            DynamicHandler() :
                m_impl(new osmium::handler::detail::HandlerWrapperBase) {
            }

            template <typename THandler, typename... TArgs>
            void set(TArgs&&... args) {
                m_impl.reset(new osmium::handler::detail::HandlerWrapper<THandler>{std::forward<TArgs>(args)...});
            }

            void node(const osmium::Node& node) {
                m_impl->node(node);
            }

            void way(const osmium::Way& way) {
                m_impl->way(way);
            }

            void relation(const osmium::Relation& relation) {
                m_impl->relation(relation);
            }

            void area(const osmium::Area& area) {
                m_impl->area(area);
            }

            void changeset(const osmium::Changeset& changeset) {
                m_impl->changeset(changeset);
            }

            void flush() {
                m_impl->flush();
            }

        }; // class DynamicHandler

    } // namespace handler

} // namespace osmium

#endif // OSMIUM_DYNAMIC_HANDLER_HPP
