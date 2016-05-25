#ifndef OSRM_UTIL_GUIDANCE_ENTRY_CLASS_HPP_
#define OSRM_UTIL_GUIDANCE_ENTRY_CLASS_HPP_

#include <cstddef>
#include <cstdint>
#include <functional>

#include <bitset>

namespace osrm
{
namespace util
{
namespace guidance
{
class EntryClass;
} // namespace guidance
} // namespace util
} // namespace osrm

namespace std
{
template <> struct hash<::osrm::util::guidance::EntryClass>
{
    inline std::size_t operator()(const ::osrm::util::guidance::EntryClass &entry_class) const;
};
} // namespace std

namespace osrm
{
namespace util
{
namespace guidance
{

class EntryClass
{
    using FlagBaseType = std::uint32_t;

  public:
    EntryClass();

    // we are hiding the access to the flags behind a protection wall, to make sure the bit logic
    // isn't tempered with. zero based indexing
    void activate(std::uint32_t index);

    // check whether a certain turn allows entry
    bool allowsEntry(std::uint32_t index) const;

    // required for hashing
    bool operator==(const EntryClass &) const;

    // sorting
    bool operator<(const EntryClass &) const;

  private:
    // given a list of possible discrete angles, the available angles flag indicates the presence of
    // a given turn at the intersection
    FlagBaseType enabled_entries_flags;

    // allow hash access to internal representation
    friend std::size_t std::hash<EntryClass>::operator()(const EntryClass &) const;
};

} // namespace guidance
} // namespace utilr
} // namespace osrm

// make Entry Class hasbable
namespace std
{
inline size_t hash<::osrm::util::guidance::EntryClass>::
operator()(const ::osrm::util::guidance::EntryClass &entry_class) const
{
    return hash<::osrm::util::guidance::EntryClass::FlagBaseType>()(
        entry_class.enabled_entries_flags);
}
} // namespace std

#endif /* OSRM_UTIL_GUIDANCE_ENTRY_CLASS_HPP_ */
