#pragma once

#include <vector>

#include "AnnotationBase.hpp"
#include "NodeId.hpp"

namespace CodeListener
{

namespace AnnotationServices
{

// TODO: very basic for now
struct CallGraphAnnotation : public AnnotationBase
{
    std::vector<Core::NodeId> callers;
    std::vector<Core::NodeId> callees;
};

} // namespace AnnotationServices

} // namespace CodeListener
