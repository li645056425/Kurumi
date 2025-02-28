project(protobuf)


set(PROTOBUF_CPP
  google/protobuf/any_lite.cc
  google/protobuf/arena.cc
  google/protobuf/extension_set.cc
  google/protobuf/generated_enum_util.cc
  google/protobuf/generated_message_table_driven_lite.cc
  google/protobuf/generated_message_util.cc
  google/protobuf/implicit_weak_message.cc
  google/protobuf/io/coded_stream.cc
  google/protobuf/io/io_win32.cc
  google/protobuf/io/strtod.cc
  google/protobuf/io/zero_copy_stream.cc
  google/protobuf/io/zero_copy_stream_impl.cc
  google/protobuf/io/zero_copy_stream_impl_lite.cc
  google/protobuf/message_lite.cc
  google/protobuf/parse_context.cc
  google/protobuf/repeated_field.cc
  google/protobuf/stubs/bytestream.cc
  google/protobuf/stubs/common.cc
  google/protobuf/stubs/int128.cc
  google/protobuf/stubs/status.cc
  google/protobuf/stubs/statusor.cc
  google/protobuf/stubs/stringpiece.cc
  google/protobuf/stubs/stringprintf.cc
  google/protobuf/stubs/structurally_valid.cc
  google/protobuf/stubs/strutil.cc
  google/protobuf/stubs/time.cc
  google/protobuf/wire_format_lite.cc
)

set(PROTOBUF_HPP
  google/protobuf/arena.h
  google/protobuf/arenastring.h
  google/protobuf/extension_set.h
  google/protobuf/generated_message_util.h
  google/protobuf/implicit_weak_message.h
  google/protobuf/parse_context.h
  google/protobuf/io/coded_stream.h
  google/protobuf/io/strtod.h
  google/protobuf/io/zero_copy_stream.h
  google/protobuf/io/zero_copy_stream_impl.h
  google/protobuf/io/zero_copy_stream_impl_lite.h
  google/protobuf/message_lite.h
  google/protobuf/repeated_field.h
  google/protobuf/stubs/bytestream.h
  google/protobuf/stubs/common.h
  google/protobuf/stubs/int128.h
  google/protobuf/stubs/once.h
  google/protobuf/stubs/status.h
  google/protobuf/stubs/statusor.h
  google/protobuf/stubs/stringpiece.h
  google/protobuf/stubs/stringprintf.h
  google/protobuf/stubs/strutil.h
  google/protobuf/stubs/time.h
  google/protobuf/wire_format_lite.h

  google/protobuf/arena_impl.h
  google/protobuf/any.h
  google/protobuf/generated_enum_util.h
  google/protobuf/generated_message_table_driven_lite.h
  google/protobuf/generated_message_table_driven.h
  google/protobuf/inlined_string_field.h
  google/protobuf/metadata_lite.h
  google/protobuf/port_undef.inc
  google/protobuf/port.h
  google/protobuf/port_def.inc
  google/protobuf/map.h
  google/protobuf/map_entry_lite.h
  google/protobuf/map_field_lite.h
  google/protobuf/stubs/port.h
  google/protobuf/stubs/logging.h
  google/protobuf/stubs/platform_macros.h
  google/protobuf/stubs/macros.h
  google/protobuf/stubs/fastmem.h
  google/protobuf/stubs/callback.h
  google/protobuf/stubs/stl_util.h
  google/protobuf/stubs/hash.h
  google/protobuf/stubs/mutex.h
  google/protobuf/stubs/casts.h
  google/protobuf/stubs/map_util.h
  google/protobuf/io/io_win32.h
  google/protobuf/has_bits.h
  google/protobuf/map_type_handler.h
  google/protobuf/extension_set_inl.h
)


# STATIC SHARED
G_add_library(${PROJECT_NAME} STATIC
    ${PROTOBUF_HPP}
	${PROTOBUF_CPP}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#target_compile_definitions(${PROJECT_NAME} PRIVATE _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS)
