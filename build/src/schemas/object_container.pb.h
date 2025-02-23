// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: src/schemas/object_container.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_src_2fschemas_2fobject_5fcontainer_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_src_2fschemas_2fobject_5fcontainer_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_src_2fschemas_2fobject_5fcontainer_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_src_2fschemas_2fobject_5fcontainer_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_src_2fschemas_2fobject_5fcontainer_2eproto;
class ObjectContainer;
struct ObjectContainerDefaultTypeInternal;
extern ObjectContainerDefaultTypeInternal _ObjectContainer_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::ObjectContainer* Arena::CreateMaybeMessage<::ObjectContainer>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

// ===================================================================

class ObjectContainer final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:ObjectContainer) */ {
 public:
  inline ObjectContainer() : ObjectContainer(nullptr) {}
  ~ObjectContainer() override;
  explicit PROTOBUF_CONSTEXPR ObjectContainer(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  ObjectContainer(const ObjectContainer& from);
  ObjectContainer(ObjectContainer&& from) noexcept
    : ObjectContainer() {
    *this = ::std::move(from);
  }

  inline ObjectContainer& operator=(const ObjectContainer& from) {
    CopyFrom(from);
    return *this;
  }
  inline ObjectContainer& operator=(ObjectContainer&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const ObjectContainer& default_instance() {
    return *internal_default_instance();
  }
  static inline const ObjectContainer* internal_default_instance() {
    return reinterpret_cast<const ObjectContainer*>(
               &_ObjectContainer_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(ObjectContainer& a, ObjectContainer& b) {
    a.Swap(&b);
  }
  inline void Swap(ObjectContainer* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(ObjectContainer* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  ObjectContainer* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<ObjectContainer>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const ObjectContainer& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const ObjectContainer& from) {
    ObjectContainer::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ObjectContainer* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "ObjectContainer";
  }
  protected:
  explicit ObjectContainer(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kIdFieldNumber = 1,
    kCreatedAtFieldNumber = 3,
  };
  // string id = 1;
  void clear_id();
  const std::string& id() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_id(ArgT0&& arg0, ArgT... args);
  std::string* mutable_id();
  PROTOBUF_NODISCARD std::string* release_id();
  void set_allocated_id(std::string* id);
  private:
  const std::string& _internal_id() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_id(const std::string& value);
  std::string* _internal_mutable_id();
  public:

  // int64 created_at = 3;
  void clear_created_at();
  int64_t created_at() const;
  void set_created_at(int64_t value);
  private:
  int64_t _internal_created_at() const;
  void _internal_set_created_at(int64_t value);
  public:

  // @@protoc_insertion_point(class_scope:ObjectContainer)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr id_;
    int64_t created_at_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_src_2fschemas_2fobject_5fcontainer_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// ObjectContainer

// string id = 1;
inline void ObjectContainer::clear_id() {
  _impl_.id_.ClearToEmpty();
}
inline const std::string& ObjectContainer::id() const {
  // @@protoc_insertion_point(field_get:ObjectContainer.id)
  return _internal_id();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void ObjectContainer::set_id(ArgT0&& arg0, ArgT... args) {
 
 _impl_.id_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:ObjectContainer.id)
}
inline std::string* ObjectContainer::mutable_id() {
  std::string* _s = _internal_mutable_id();
  // @@protoc_insertion_point(field_mutable:ObjectContainer.id)
  return _s;
}
inline const std::string& ObjectContainer::_internal_id() const {
  return _impl_.id_.Get();
}
inline void ObjectContainer::_internal_set_id(const std::string& value) {
  
  _impl_.id_.Set(value, GetArenaForAllocation());
}
inline std::string* ObjectContainer::_internal_mutable_id() {
  
  return _impl_.id_.Mutable(GetArenaForAllocation());
}
inline std::string* ObjectContainer::release_id() {
  // @@protoc_insertion_point(field_release:ObjectContainer.id)
  return _impl_.id_.Release();
}
inline void ObjectContainer::set_allocated_id(std::string* id) {
  if (id != nullptr) {
    
  } else {
    
  }
  _impl_.id_.SetAllocated(id, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.id_.IsDefault()) {
    _impl_.id_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:ObjectContainer.id)
}

// int64 created_at = 3;
inline void ObjectContainer::clear_created_at() {
  _impl_.created_at_ = int64_t{0};
}
inline int64_t ObjectContainer::_internal_created_at() const {
  return _impl_.created_at_;
}
inline int64_t ObjectContainer::created_at() const {
  // @@protoc_insertion_point(field_get:ObjectContainer.created_at)
  return _internal_created_at();
}
inline void ObjectContainer::_internal_set_created_at(int64_t value) {
  
  _impl_.created_at_ = value;
}
inline void ObjectContainer::set_created_at(int64_t value) {
  _internal_set_created_at(value);
  // @@protoc_insertion_point(field_set:ObjectContainer.created_at)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_src_2fschemas_2fobject_5fcontainer_2eproto
