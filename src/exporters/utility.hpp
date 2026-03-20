#pragma once

namespace CodeListener::Exporters
{

template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not strictly needed in C++20/23, but good practice)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

} // namespace CodeListener::Exporters
