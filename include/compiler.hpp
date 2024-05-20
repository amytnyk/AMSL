#ifndef AMSL_COMPILER_HPP
#define AMSL_COMPILER_HPP

#include "string.hpp"
#include "traits.hpp"
#include <bit>
#include <span>

template<typename ExpressionPack>
struct CompiledExpressionList {
  using expressions = ExpressionPack;
};

template<string_t Name, typename ParameterPack>
struct CompiledFunctionCallExpression {
  static constexpr auto name = Name;
  using parameters = ParameterPack;
};

template<typename Type>
struct CompiledVariableDeclarationExpression {
  using type = Type;
};

template<typename Type, typename Initializer>
struct CompiledVariableDeclarationWithInitializerExpression {
  using type = Type;
  using initializer = Initializer;
};

template<typename Initializer>
struct CompiledVariableDeclarationWithInitializerAutoTypeExpression {
  using initializer = Initializer;
};

template<std::size_t RefID>
struct CompiledVariableExpression {
  static constexpr auto ref_id = RefID;
};

template<typename Lhs, typename Rhs>
struct CompiledAssignmentExpression {
  using lhs = Lhs;
  using rhs = Rhs;
};

template<typename T, T Value>
struct CompiledLiteral {
  using type = T;
  static constexpr auto value = Value;
};

template<auto Value>
using CompiledLiteralAuto = CompiledLiteral<std::remove_const_t<decltype(Value)>, Value>;

template<auto Size>
constexpr auto span_to_array(const auto &span) {
  std::array<std::byte, Size> result;
  std::copy(span.begin(), span.end(), result.begin());
  return result;
}

template<auto Ptr, std::size_t Offset, Trivial T>
struct TrivialDecoder {
  static constexpr auto size = sizeof(T);
  static constexpr auto next_offset = Offset + size;
  static constexpr auto value = std::bit_cast<T>(
    span_to_array<size>(std::span<const std::byte, size>{std::next(Ptr, Offset), size}));
};

template<auto Ptr, std::size_t Offset>
using IntDecoder = TrivialDecoder<Ptr, Offset, int>;

template<auto Ptr, std::size_t Offset>
using SizeDecoder = TrivialDecoder<Ptr, Offset, std::size_t>;

template<auto Ptr, std::size_t Offset>
struct StringDecoder {
  using size_decoder = SizeDecoder<Ptr, Offset>;
  using data_decoder = TrivialDecoder<Ptr, size_decoder::next_offset, string_t<size_decoder::value>>;
  static constexpr auto next_offset = data_decoder::next_offset;
  static constexpr auto value = data_decoder::value;
};

template<string_t Str>
struct TypeDecoder;

template<>
struct TypeDecoder<"int"> {
  using type = int;
};

template<>
struct TypeDecoder<"uint"> {
  using type = unsigned int;
};

template<>
struct TypeDecoder<"int16"> {
  using type = int16_t;
};

template<>
struct TypeDecoder<"uint16"> {
  using type = u_int16_t;
};

template<>
struct TypeDecoder<"int32"> {
  using type = int32_t;
};

template<>
struct TypeDecoder<"uint32"> {
  using type = u_int32_t;
};

template<>
struct TypeDecoder<"int64"> {
  using type = int64_t;
};

template<>
struct TypeDecoder<"uint64"> {
  using type = u_int64_t;
};

template<>
struct TypeDecoder<"string"> {
  using type = std::string;
};

template<>
struct TypeDecoder<"float"> {
  using type = float;
};

template<>
struct TypeDecoder<"double"> {
  using type = double;
};


template<>
struct TypeDecoder<"bool"> {
  using type = bool;
};

template<>
struct TypeDecoder<"char"> {
  using type = char;
};

template<>
struct TypeDecoder<"size"> {
  using type = std::size_t;
};

template<auto Ptr, std::size_t Offset = 0>
struct Compiler {
};

template<typename ... Args>
class ParameterPack;

template<typename Pack1, typename Pack2>
struct parameter_pack_join;

template<typename... Args1, typename... Args2>
struct parameter_pack_join<ParameterPack<Args1...>, ParameterPack<Args2...>> {
  using type = ParameterPack<Args1..., Args2...>;
};

template<typename Pack1, typename Pack2>
using parameter_pack_join_t = typename parameter_pack_join<Pack1, Pack2>::type;

template<auto Ptr, std::size_t Offset, std::size_t Size>
struct SizedParameterPackCompiler {
  using this_compiler = Compiler<Ptr, Offset>;
  using next_compiler = SizedParameterPackCompiler<Ptr, this_compiler::next_offset, Size - 1>;
  static constexpr auto next_offset = next_compiler::next_offset;
  using compiled = parameter_pack_join_t<ParameterPack<typename this_compiler::compiled>, typename next_compiler::compiled>;
};

template<auto Ptr, std::size_t Offset>
struct SizedParameterPackCompiler<Ptr, Offset, 0> {
  static constexpr auto next_offset = Offset;
  using compiled = ParameterPack<>;
};

template<auto Ptr, std::size_t Offset>
struct ParameterPackCompiler {
  using size_decoder = SizeDecoder<Ptr, Offset>;
  static constexpr auto size = size_decoder::value;

  using this_compiler = SizedParameterPackCompiler<Ptr, size_decoder::next_offset, size>;
  static constexpr auto next_offset = this_compiler::next_offset;
  using compiled = this_compiler::compiled;
};

template<auto Ptr, std::size_t Offset> requires (*std::next(Ptr, Offset) == std::byte{0})
struct Compiler<Ptr, Offset> {
  using this_compiler = ParameterPackCompiler<Ptr, Offset + 1>;
  static constexpr auto next_offset = this_compiler::next_offset;
  using compiled = CompiledExpressionList<typename this_compiler::compiled>;
};

template<auto Ptr, std::size_t Offset> requires (*std::next(Ptr, Offset) == std::byte{1})
struct Compiler<Ptr, Offset> {
  using name_decoder = StringDecoder<Ptr, Offset + 1>;
  using parameters_compiler = ParameterPackCompiler<Ptr, name_decoder::next_offset>;
  static constexpr auto next_offset = parameters_compiler::next_offset;
  using compiled = CompiledFunctionCallExpression<name_decoder::value, typename parameters_compiler::compiled>;
};

template<auto Ptr, std::size_t Offset> requires (*std::next(Ptr, Offset) == std::byte{2})
struct Compiler<Ptr, Offset> {
  using type_string_decoder = StringDecoder<Ptr, Offset + 1>;
  using type_decoder = TypeDecoder<type_string_decoder::value>;
  static constexpr auto next_offset = type_string_decoder::next_offset;
  using compiled = CompiledVariableDeclarationExpression<typename type_decoder::type>;
};

template<auto Ptr, std::size_t Offset> requires (*std::next(Ptr, Offset) == std::byte{3})
struct Compiler<Ptr, Offset> {
  using type_string_decoder = StringDecoder<Ptr, Offset + 1>;
  using initializer_compiler = Compiler<Ptr, type_string_decoder::next_offset>;
  using type_decoder = TypeDecoder<type_string_decoder::value>;
  static constexpr auto next_offset = initializer_compiler::next_offset;
  using compiled = CompiledVariableDeclarationWithInitializerExpression<typename type_decoder::type, typename initializer_compiler::compiled>;
};

template<auto Ptr, std::size_t Offset> requires (*std::next(Ptr, Offset) == std::byte{4})
struct Compiler<Ptr, Offset> {
  using initializer_compiler = Compiler<Ptr, Offset + 1>;
  static constexpr auto next_offset = initializer_compiler::next_offset;
  using compiled = CompiledVariableDeclarationWithInitializerAutoTypeExpression<typename initializer_compiler::compiled>;
};

template<auto Ptr, std::size_t Offset> requires (*std::next(Ptr, Offset) == std::byte{5})
struct Compiler<Ptr, Offset> {
  using this_decoder = SizeDecoder<Ptr, Offset + 1>;
  static constexpr auto next_offset = this_decoder::next_offset;
  using compiled = CompiledVariableExpression<this_decoder::value>;
};

template<auto Ptr, std::size_t Offset> requires (*std::next(Ptr, Offset) == std::byte{6})
struct Compiler<Ptr, Offset> {
  using left_compiler = Compiler<Ptr, Offset + 1>;
  using right_compiler = Compiler<Ptr, left_compiler::next_offset>;
  static constexpr auto next_offset = right_compiler::next_offset;
  using compiled = CompiledAssignmentExpression<typename left_compiler::compiled, typename right_compiler::compiled>;
};

template<auto Ptr, std::size_t Offset> requires (*std::next(Ptr, Offset) == std::byte{7})
struct Compiler<Ptr, Offset> {
  using this_decoder = IntDecoder<Ptr, Offset + 1>;
  static constexpr auto next_offset = this_decoder::next_offset;
  using compiled = CompiledLiteralAuto<this_decoder::value>;
};

template<auto Ptr, std::size_t Offset> requires (*std::next(Ptr, Offset) == std::byte{8})
struct Compiler<Ptr, Offset> {
  using this_decoder = StringDecoder<Ptr, Offset + 1>;
  static constexpr auto next_offset = this_decoder::next_offset;
  using compiled = CompiledLiteralAuto<this_decoder::value>;
};

#endif // AMSL_COMPILER_HPP
