// src/beman/execution26/execution.cpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// ----------------------------------------------------------------------------

module;
#include <execution>
#include <stop_token>
#include <beman/execution26/execution.hpp>
#include <beman/execution26/stop_token.hpp>

export module beman_execution26;

namespace beman::execution26 {
export int version(0);
// [stoptoken.concepts], stop token concepts
export using ::beman::execution26::stoppable_token;
export using ::beman::execution26::unstoppable_token;

// [stoptoken], class stop_token
export using ::beman::execution26::stop_token;

// [stopsource], class stop_source
export using ::beman::execution26::stop_source;

// no-shared-stop-state indicator
//-dk:TODO export using ::beman::execution26::no_stop_state_t;

// [stopcallback], class template stop_callback
export using ::beman::execution26::stop_callback;

// [stoptoken.never], class never_stop_token
export using ::beman::execution26::never_stop_token;

// [stoptoken.inplace], class inplace_stop_token
export using ::beman::execution26::inplace_stop_token;

// [stopsource.inplace], class inplace_stop_source
export using ::beman::execution26::inplace_stop_source;

// [stopcallback.inplace], class template inplace_stop_callback
export using ::beman::execution26::inplace_stop_callback;
export using ::beman::execution26::stop_callback_for_t;

#if 0
    //-dk:TODO enable the execution policies
    export using ::std::is_execution_policy;
    export using ::std::is_execution_policy_v;

    export using ::std::execution::sequenced_policy;
    export using ::std::execution::parallel_policy;
    export using ::std::execution::parallel_unsequenced_policy;
    export using ::std::execution::unsequenced_policy;

    export using ::std::execution::seq;
    export using ::std::execution::par;
    export using ::std::execution::par_unseq;
    export using ::std::execution::unseq;
#endif

// [exec.queries], queries
export using ::beman::execution26::forwarding_query_t;
export using ::beman::execution26::get_allocator_t;
export using ::beman::execution26::get_stop_token_t;

export using ::beman::execution26::forwarding_query;
export using ::beman::execution26::get_allocator;
export using ::beman::execution26::get_stop_token;

export using ::beman::execution26::stop_token_of_t;

export using ::beman::execution26::get_domain_t;
export using ::beman::execution26::get_scheduler_t;
export using ::beman::execution26::get_delegation_scheduler_t;
//-dk:TODO export using ::beman::execution26::get_forward_progress_guarantee_t;
export using ::beman::execution26::get_completion_scheduler_t;

export using ::beman::execution26::get_domain;
export using ::beman::execution26::get_scheduler;
export using ::beman::execution26::get_delegation_scheduler;
//-dk:TODO export using ::beman::execution26::forward_progress_guarantee;
//-dk:TODO export using ::beman::execution26::get_forward_progress_guarantee;
export using ::beman::execution26::get_completion_scheduler;

export using ::beman::execution26::empty_env;
export using ::beman::execution26::get_env_t;
export using ::beman::execution26::get_env;

export using ::beman::execution26::env_of_t;

// [exec.domain.default], execution_domains
export using ::beman::execution26::default_domain;

// [exec.sched], schedulers
export using ::beman::execution26::scheduler_t;
export using ::beman::execution26::scheduler;

// [exec.recv], receivers
export using ::beman::execution26::receiver_t;
export using ::beman::execution26::receiver;
export using ::beman::execution26::receiver_of;

export using ::beman::execution26::set_value_t;
export using ::beman::execution26::set_error_t;
export using ::beman::execution26::set_stopped_t;

export using ::beman::execution26::set_value;
export using ::beman::execution26::set_error;
export using ::beman::execution26::set_stopped;

// [exec.opstate], operation states
export using ::beman::execution26::operation_state_t;
export using ::beman::execution26::operation_state;
export using ::beman::execution26::start_t;
export using ::beman::execution26::start;

// [exec.snd], senders
export using ::beman::execution26::sender_t;
export using ::beman::execution26::sender;
export using ::beman::execution26::sender_in;
//-dk:TODO export using ::beman::execution26::sender_to;

// [exec.getcomplsigs], completion signatures
export using ::beman::execution26::get_completion_signatures_t;
export using ::beman::execution26::get_completion_signatures;
export using ::beman::execution26::completion_signatures_of_t;
export using ::beman::execution26::value_types_of_t;
export using ::beman::execution26::error_types_of_t;
export using ::beman::execution26::sends_stopped;
export using ::beman::execution26::tag_of_t;

// [exec.snd.transform], sender transformations
export using ::beman::execution26::transform_sender;

// [exec.snd.transform.env], environment transformations
//-dk:TODO export using ::beman::execution26::transform_env;

// [exec.snd.apply], sender algorithm application
export using ::beman::execution26::apply_sender;

// [exec.connect], the connect sender algorithm
export using ::beman::execution26::connect_t;
export using ::beman::execution26::connect;
export using ::beman::execution26::connect_result_t;

// [exec.factories], sender factories
export using ::beman::execution26::just_t;
export using ::beman::execution26::just_error_t;
export using ::beman::execution26::just_stopped_t;
export using ::beman::execution26::schedule_t;

export using ::beman::execution26::just;
export using ::beman::execution26::just_error;
export using ::beman::execution26::just_stopped;
export using ::beman::execution26::schedule;
export using ::beman::execution26::read_env;

export using ::beman::execution26::schedule_result_t;

// [exec.adapt], sender adaptors
export using ::beman::execution26::sender_adaptor_closure;

export using ::beman::execution26::starts_on_t;
export using ::beman::execution26::continues_on_t;
//-dk:TODO export using ::beman::execution26::on_t;
export using ::beman::execution26::schedule_from_t;
export using ::beman::execution26::then_t;
export using ::beman::execution26::upon_error_t;
export using ::beman::execution26::upon_stopped_t;
export using ::beman::execution26::let_value_t;
export using ::beman::execution26::let_error_t;
export using ::beman::execution26::let_stopped_t;
//-dk:TODO export using ::beman::execution26::bulk_t;
//-dk:TODO export using ::beman::execution26::split_t;
export using ::beman::execution26::when_all_t;
export using ::beman::execution26::when_all_with_variant_t;
export using ::beman::execution26::into_variant_t;
//-dk:TODO export using ::beman::execution26::stopped_as_optional_t;
//-dk:TODO export using ::beman::execution26::stopped_as_error_t;

export using ::beman::execution26::starts_on;
export using ::beman::execution26::continues_on;
//-dk:TODO export using ::beman::execution26::on;
export using ::beman::execution26::schedule_from;
export using ::beman::execution26::then;
export using ::beman::execution26::upon_error;
export using ::beman::execution26::upon_stopped;
export using ::beman::execution26::let_value;
export using ::beman::execution26::let_error;
export using ::beman::execution26::let_stopped;
//-dk:TODO export using ::beman::execution26::bulk;
//-dk:TODO export using ::beman::execution26::split;
export using ::beman::execution26::when_all;
export using ::beman::execution26::when_all_with_variant;
export using ::beman::execution26::into_variant;
//-dk:TODO export using ::beman::execution26::stopped_as_optional;
//-dk:TODO export using ::beman::execution26::stopped_as_error;

// [exec.util.cmplsig]
export using ::beman::execution26::completion_signatures;

// [exec.util.cmplsig.trans]
//-dk:TODO export using ::beman::execution26::transform_completion_signatures;
//-dk:TODO export using ::beman::execution26::transform_completion_signatures_of;

// [exec.run.loop], run_loop
export using ::beman::execution26::run_loop;

// [exec.consumers], consumers
export using ::beman::execution26::sync_wait_t;
//-dk:TODO export using ::beman::execution26::sync_wait_with_variant_t;

export using ::beman::execution26::sync_wait;
//-dk:TODO export using ::beman::execution26::sync_wait_with_variant;

// [exec.as.awaitable]
//-dk:TODO export using ::beman::execution26::as_awaitable_t;
//-dk:TODO export using ::beman::execution26::as_awaitable;

// [exec.with.awaitable.senders]
//-dk:TODO export using ::beman::execution26::with_awaitable_senders;

} // namespace beman::execution26
