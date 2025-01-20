# cmake-format: off
# src/beman/execution26/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# cmake-format: on

add_library(${TARGET_NAME} STATIC)
add_library(${TARGET_ALIAS} ALIAS ${TARGET_NAME})

if(NOT BEMAN_EXECUTION26_ENABLE_INSTALL OR CMAKE_SKIP_INSTALL_RULES)
  target_link_libraries(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${TARGET_NAME}_project_options>)
  target_link_libraries(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${TARGET_NAME}_project_warnings>)
endif()

include(CMakePrintHelpers)
cmake_print_variables(TARGET_ALIAS TARGET_NAME TARGET_PREFIX PROJECT_SOURCE_DIR)

target_sources(
  ${TARGET_NAME}
  PRIVATE execution.cpp
  PUBLIC FILE_SET
         ${TARGET_NAME}_public_headers
         TYPE
         HEADERS
         BASE_DIRS
         ${PROJECT_SOURCE_DIR}/include
         FILES
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/execution.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/functional.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/stop_token.hpp
  PUBLIC FILE_SET
         ${TARGET_NAME}_detail_headers
         TYPE
         HEADERS
         BASE_DIRS
         ${PROJECT_SOURCE_DIR}/include
         FILES
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/allocator_aware_move.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/almost_scheduler.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/apply_sender.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/as_awaitable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/as_except_ptr.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/atomic_intrusive_stack.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/await_result_type.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/await_suspend_result.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/awaitable_sender.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/basic_operation.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/basic_receiver.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/basic_sender.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/basic_state.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/call_result_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/callable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/check_type_alias_exist.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/child_type.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/class_type.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/common.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/completion_domain.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/completion_signature.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/completion_signatures.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/completion_signatures_for.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/completion_signatures_of_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/completion_tag.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/connect.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/connect_all.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/connect_all_result.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/connect_awaitable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/connect_result_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/continues_on.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/decayed_same_as.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/decayed_tuple.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/decayed_type_list.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/decayed_typeof.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/decays_to.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/default_domain.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/default_impls.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/emplace_from.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/empty_env.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/env_of_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/env_promise.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/env_type.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/error_types_of_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/forward_like.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/forwarding_query.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/fwd_env.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/gather_signatures.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/get_allocator.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/get_awaiter.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/get_completion_scheduler.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/get_completion_signatures.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/get_delegation_scheduler.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/get_domain.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/get_domain_early.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/get_domain_late.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/get_env.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/get_scheduler.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/get_stop_token.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/has_as_awaitable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/has_completions.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/immovable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/impls_for.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/indices_for.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/indirect_meta_apply.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/inplace_stop_source.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/into_variant.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/intrusive_stack.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/is_awaitable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/is_awaiter.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/join_env.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/just.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/let.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/make_env.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/make_sender.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/matching_sig.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/meta_combine.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/meta_contains.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/meta_filter.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/meta_prepend.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/meta_size.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/meta_to.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/meta_transform.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/meta_unique.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/movable_value.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/never_stop_token.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/nostopstate.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/nothrow_callable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/notify.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/on.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/on_stop_request.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/operation_state.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/operation_state_task.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/product_type.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/query_with_default.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/queryable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/read_env.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/receiver.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/receiver_of.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/run_loop.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/sched_attrs.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/sched_env.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/schedule.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/schedule_from.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/schedule_result_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/scheduler.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/scheduler_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/sender.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/sender_adaptor.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/sender_adaptor_closure.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/sender_awaitable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/sender_decompose.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/sender_for.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/sender_in.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/sends_stopped.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/set_error.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/set_stopped.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/set_value.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/simple_allocator.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/simple_counting_scope.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/single_sender.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/single_sender_value_type.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/split.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/start.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/starts_on.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/state_type.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/stop_callback_for_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/stop_source.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/stop_token_of_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/stoppable_source.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/stoppable_token.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/suppress_pop.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/suppress_push.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/suspend_complete.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/sync_wait.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/tag_of_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/then.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/transform_sender.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/type_list.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/unspecified_promise.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/unstoppable_token.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/valid_completion_for.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/valid_completion_signatures.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/valid_specialization.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/value_types_of_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/variant_or_empty.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/when_all.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/when_all_with_variant.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/with_await_transform.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/with_awaitable_senders.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution26/detail/write_env.hpp
)

# cmake-format: off
get_property(DETAIL_HEADER_FILES TARGET ${TARGET_NAME} PROPERTY HEADER_SET_${TARGET_NAME}_detail_headers)
source_group("Header Files\\detail" FILES ${DETAIL_HEADER_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON)

target_compile_features(${TARGET_NAME} PUBLIC
  "$<$<COMPILE_FEATURES:cxx_std_26>:cxx_std_26>"
  "$<$<NOT:$<COMPILE_FEATURES:cxx_std_26>>:cxx_std_23>"
)

if(NOT BEMAN_EXECUTION26_ENABLE_INSTALL OR CMAKE_SKIP_INSTALL_RULES)
  return()
endif()

install(
  TARGETS ${TARGET_NAME}
  EXPORT ${TARGETS_EXPORT_NAME}1
  ARCHIVE DESTINATION lib/$<CONFIG>
  FILE_SET ${TARGET_NAME}_public_headers
  FILE_SET ${TARGET_NAME}_detail_headers
)
# cmake-format: on

install(
  EXPORT ${TARGETS_EXPORT_NAME}1
  FILE ${TARGETS_EXPORT_NAME}.cmake
  DESTINATION "${INSTALL_CONFIGDIR}"
  NAMESPACE ${TARGET_NAMESPACE}::
)
