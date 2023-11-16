
# Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
# Author: Edward Deighton
# License: Please see license.txt in the project root folder.
# 
# Use and copying of this software and preparation of derivative works
# based upon this software are permitted. Any copy of this software or
# of any derivative work must include the above copyright notice, this
# paragraph and the one after it.  Any distribution of this software or
# derivative works must comply with all applicable laws.
# 
# This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
# ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
# LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
# EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
# NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGES.

cmake_minimum_required( VERSION 3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get fmt
include( ${WORKSPACE_ROOT_PATH}/thirdparty/fmt/fmt_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get pybind11
include( ${WORKSPACE_ROOT_PATH}/thirdparty/pybind11/pybind11_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

add_library( mega_pch STATIC ${MEGA_SRC_DIR}/environment/pch.cpp )

#link_boost( mega_pch atomic )
#link_boost( mega_pch timer )
#link_boost( mega_pch filesystem )
#link_boost( mega_pch iostreams )
#link_boost( mega_pch serialization )
#link_boost( mega_pch program_options )

#link_fmt( mega_pch )
#link_spdlog( mega_pch )
#link_json( mega_pch )
#link_inja( mega_pch )
#link_pybind11( mega_pch )

#link_common( mega_pch )

target_precompile_headers( mega_pch
PRIVATE
    <algorithm>
    <array>
    <atomic>
    <bitset>
    <cassert>
    <ccomplex>
    <cctype>
    <cerrno>
    <cfenv>
    <cfloat>
    <chrono>
    <cinttypes>
    <ciso646>
    <climits>
    <clocale>
    <cmath>
    <codecvt>
    <complex>
    <condition_variable>
    <csetjmp>
    <csignal>
    <cstdalign>
    <cstdarg>
    <cstdbool>
    <cstddef>
    <cstdint>
    <cstdio>
    <cstdlib>
    <cstring>
    <ctgmath>
    <ctime>
    <cuchar>
    <cwchar>
    <cwctype>
    <deque>
    <exception>
    <forward_list>
    <fstream>
    <functional>
    <future>
    <initializer_list>
    <iomanip>
    <ios>
    <iosfwd>
    <iostream>
    <istream>
    <iterator>
    <limits>
    <list>
    <locale>
    <map>
    <memory>
    <mutex>
    <new>
    <numeric>
    <ostream>
    <queue>
    <random>
    <ratio>
    <regex>
    <scoped_allocator>
    <set>
    <sstream>
    <stack>
    <stdexcept>
    <streambuf>
    <string>
    <system_error>
    <thread>
    <tuple>
    <type_traits>
    <typeindex>
    <typeinfo>
    <unordered_map>
    <unordered_set>
    <utility>
    <valarray>
    <vector>

    #<boost/algorithm/string/detail/formatter.hpp>
    #<boost/algorithm/string.hpp>
    #<boost/archive/archive_exception.hpp>
    #<boost/archive/basic_binary_iarchive.hpp>
    #<boost/archive/basic_binary_iprimitive.hpp>
    #<boost/archive/basic_binary_oarchive.hpp>
    #<boost/archive/basic_binary_oprimitive.hpp>
    #<boost/archive/basic_xml_archive.hpp>
    #<boost/archive/basic_xml_iarchive.hpp>
    #<boost/archive/basic_xml_oarchive.hpp>
    #<boost/archive/binary_iarchive.hpp>
    #<boost/archive/binary_iarchive_impl.hpp>
    #<boost/archive/binary_oarchive.hpp>
    #<boost/archive/binary_oarchive_impl.hpp>
    #<boost/archive/text_oarchive.hpp>
    #<boost/archive/xml_iarchive.hpp>
    #<boost/archive/xml_oarchive.hpp>
    #<boost/asio/associated_allocator.hpp>
    #<boost/asio/buffer.hpp>
    #<boost/asio/connect.hpp>
    #<boost/asio/detached.hpp>
    #<boost/asio/experimental/channel_error.hpp>
    #<boost/asio/experimental/channel.hpp>
    #<boost/asio/experimental/concurrent_channel.hpp>
    #<boost/asio/io_context.hpp>
    #<boost/asio/io_service.hpp>
    #<boost/asio/ip/host_name.hpp>
    #<boost/asio/ip/tcp.hpp>
    #<boost/asio/ip/udp.hpp>
    #<boost/asio/placeholders.hpp>
    #<boost/asio/read.hpp>
    #<boost/asio/signal_set.hpp>
    #<boost/asio/spawn.hpp>
    #<boost/asio/steady_timer.hpp>
    #<boost/asio/strand.hpp>
    #<boost/asio/this_coro.hpp>
    #<boost/asio/thread_pool.hpp>
    #<boost/asio/use_future.hpp>
    #<boost/asio/write.hpp>
    #<boost/beast/core.hpp>
    #<boost/beast/http.hpp>
    #<boost/beast/version.hpp>
    #<boost/bind/bind.hpp>
    #<boost/chrono.hpp>
    #<boost/circular_buffer.hpp>
    #<boost/config.hpp>
    #<boost/current_function.hpp>
    #<boost/date_time/posix_time/posix_time.hpp>
    #<boost/dll/alias.hpp>
    #<boost/dll.hpp>
    #<boost/dll/import.hpp>
    #<boost/dll/library_info.hpp>
    #<boost/dll/runtime_symbol_info.hpp>
    #<boost/dll/shared_library.hpp>
    #<boost/dll/shared_library_load_mode.hpp>
    #<boost/dynamic_bitset.hpp>
    #<boost/dynamic_bitset/serialization.hpp>
    #<boost/enable_shared_from_this.hpp>
    #<boost/filesystem/file_status.hpp>
    #<boost/filesystem.hpp>
    #<boost/filesystem/operations.hpp>
    #<boost/filesystem/path.hpp>
    #<boost/function.hpp>
    #<boost/geometry/geometries/box.hpp>
    #<boost/geometry/geometries/point.hpp>
    #<boost/geometry/geometries/segment.hpp>
    #<boost/geometry.hpp>
    #<boost/geometry/index/rtree.hpp>
    #<boost/graph/adjacency_list.hpp>
    #<boost/graph/connected_components.hpp>
    #<boost/graph/graph_traits.hpp>
    #<boost/interprocess/file_mapping.hpp>
    #<boost/interprocess/interprocess_fwd.hpp>
    #<boost/interprocess/mapped_region.hpp>
    #<boost/interprocess/streams/bufferstream.hpp>
    #<boost/interprocess/streams/vectorstream.hpp>
    #<boost/optional.hpp>
    #<boost/phoenix/core.hpp>
    #<boost/phoenix/fusion.hpp>
    #<boost/phoenix/object.hpp>
    #<boost/phoenix/operator.hpp>
    #<boost/phoenix/stl.hpp>
    #<boost/process/environment.hpp>
    #<boost/process.hpp>
    #<boost/qvm/all.hpp>
    #<boost/scoped_ptr.hpp>
    #<boost/serialization/array.hpp>
    #<boost/serialization/array_wrapper.hpp>
    #<boost/serialization/map.hpp>
    #<boost/serialization/nvp.hpp>
    #<boost/serialization/serialization.hpp>
    #<boost/serialization/split_member.hpp>
    #<boost/serialization/unordered_map.hpp>
    #<boost/serialization/variant.hpp>
    #<boost/serialization/vector.hpp>
    #<boost/shared_ptr.hpp>
    #<boost/spirit/home/support/iterators/line_pos_iterator.hpp>
    #<boost/spirit/include/qi_eoi.hpp>
    #<boost/spirit/include/qi.hpp>
    #<boost/spirit/include/support_istream_iterator.hpp>
    #<boost/spirit/include/support_line_pos_iterator.hpp>
    #<boost/spirit/include/support_multi_pass.hpp>
    #<boost/stacktrace.hpp>
    #<boost/system/detail/error_code.hpp>
    #<boost/timer/timer.hpp>
    #<boost/tokenizer.hpp>
    #<boost/url.hpp>
    #<boost/uuid/random_generator.hpp>
    #<boost/uuid/uuid_generators.hpp>
    #<boost/uuid/uuid.hpp>
    #<boost/uuid/uuid_io.hpp>
    #<boost/variant/detail/apply_visitor_unary.hpp>
    #<boost/variant.hpp>
    #<boost/weak_ptr.hpp>

    #<llvm/ADT/APSInt.h>
    #<llvm/ExecutionEngine/ExecutionEngine.h>
    #<llvm/ExecutionEngine/JITSymbol.h>
    #<llvm/ExecutionEngine/Orc/CompileUtils.h>
    #<llvm/ExecutionEngine/Orc/Core.h>
    #<llvm/ExecutionEngine/Orc/ExecutionUtils.h>
    #<llvm/ExecutionEngine/Orc/IRCompileLayer.h>
    #<llvm/ExecutionEngine/Orc/LLJIT.h>
    #<llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
    #<llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
    #<llvm/ExecutionEngine/SectionMemoryManager.h>
    #<llvm/IR/BasicBlock.h>
    #<llvm/IR/DataLayout.h>
    #<llvm/IR/Function.h>
    #<llvm/IR/Instructions.h>
    #<llvm/IR/LegacyPassManager.h>
    #<llvm/IR/LLVMContext.h>
    #<llvm/IR/Module.h>
    #<llvm/IRReader/IRReader.h>
    #<llvm/IR/Type.h>
    #<llvm/Pass.h>
    #<llvm/Support/Casting.h>
    #<llvm/Support/Error.h>
    #<llvm/Support/FileSystem.h>
    #<llvm/Support/raw_os_ostream.h>
    #<llvm/Support/SourceMgr.h>
    #<llvm/Support/TargetSelect.h>
    #<llvm/Target/TargetMachine.h>
    #<llvm/Transforms/InstCombine/InstCombine.h>
    #<llvm/Transforms/IPO.h>
    #<llvm/Transforms/Scalar/GVN.h>
    #<llvm/Transforms/Scalar.h>

    #<clang/AST/ASTConsumer.h>
    #<clang/AST/ASTContext.h>
    #<clang/AST/DeclCXX.h>
    #<clang/AST/Decl.h>
    #<clang/AST/Expr.h>
    #<clang/ASTMatchers/ASTMatchers.h>
    #<clang/ASTMatchers/ASTMatchFinder.h>
    #<clang/AST/Type.h>
    #<clang/Basic/BitmaskEnum.h>
    #<clang/Basic/Diagnostic.h>
    #<clang/Basic/DiagnosticIDs.h>
    #<clang/Basic/DiagnosticOptions.h>
    #<clang/Basic/DiagnosticParse.h>
    #<clang/Basic/DiagnosticSema.h>
    #<clang/Basic/DirectoryEntry.h>
    #<clang/Basic/FileManager.h>
    #<clang/Basic/FileSystemOptions.h>
    #<clang/Basic/LangOptions.h>
    #<clang/Basic/LLVM.h>
    #<clang/Basic/MemoryBufferCache.h>
    #<clang/Basic/SourceLocation.h>
    #<clang/Basic/SourceManager.h>
    #<clang/Basic/TargetInfo.h>
    #<clang/Basic/TokenKinds.h>
    #<clang/Format/Format.h>
    #<clang/Lex/DirectoryLookup.h>
    #<clang/Lex/HeaderSearch.h>
    #<clang/Lex/HeaderSearchOptions.h>
    #<clang/Lex/Lexer.h>
    #<clang/Lex/ModuleLoader.h>
    #<clang/Lex/Pragma.h>
    #<clang/Lex/Preprocessor.h>
    #<clang/Lex/PreprocessorOptions.h>
    #<clang/Lex/Token.h>
    #<clang/Parse/ParseDiagnostic.h>
    #<clang/Rewrite/Core/Rewriter.h>
    #<clang/Sema/Lookup.h>
    #<clang/Sema/Ownership.h>
    #<clang/Sema/ParsedTemplate.h>
    #<clang/Sema/Sema.h>
    #<clang/Tooling/Refactoring.h>

    #<pybind11/pybind11.h>
    #<pybind11/stl.h>
    #<pybind11/iostream.h>
    #<pybind11/operators.h>

    #<spdlog/spdlog.h>
    #<spdlog/async.h>
    #<spdlog/common.h>
    #<spdlog/sinks/daily_file_sink.h>
    #<spdlog/sinks/stdout_color_sinks.h>
    #<spdlog/stopwatch.h>

    #<inja/inja.hpp>
    #<inja/environment.hpp>
    #<inja/template.hpp>

    #<common/angle.hpp>
    #<common/assert_verify.hpp>
    #<common/astar.hpp>
    #<common/backtrace.hpp>
    #<common/clang_warnings.hpp>
    #<common/compose.hpp>
    #<common/file.hpp>
    #<common/grammar.hpp>
    #<common/hash.hpp>
    #<common/math.hpp>
    #<common/optimisation.hpp>
    #<common/process.hpp>
    #<common/processID.hpp>
    #<common/requireSemicolon.hpp>
    #<common/rounding.hpp>
    #<common/serialisation.hpp>
    #<common/stash.hpp>
    #<common/stl.hpp>
    #<common/string.hpp>
    #<common/terminal.hpp>
    #<common/tick.hpp>
    #<common/time.hpp>
    #<common/unreachable.hpp>
    #<common/variant_utils.hpp>
    
)
#${MEGA_API_DIR}/environment/include.hpp
#${MEGA_API_DIR}/reports/value.hpp
#${MEGA_API_DIR}/reports/report.hpp