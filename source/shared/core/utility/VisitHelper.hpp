#ifndef __VISIT_HELPER_HPP__
#define __VISIT_HELPER_HPP__

namespace Soldank
{
// VisitOverload helps pass lambdas to std::visit like this:
// std::visit(VisitOverload{
//                          [](int a) { return true; },
//                          [](float b) { return false; }
//                         },
//            some_variant
// );
template<class... Ts>
struct VisitOverload : Ts...
{
    using Ts::operator()...;
};
template<class... Ts>
VisitOverload(Ts...) -> VisitOverload<Ts...>;
} // namespace Soldank

#endif
