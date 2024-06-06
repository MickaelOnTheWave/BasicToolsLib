#include "abstractprogresscounter.h"

AbstractProgressCounter::AbstractProgressCounter()
    : globalProgressCounter(0)
{
    rootStep = new ProgressStep(100, "");
    currentStep = nullptr;
}

void AbstractProgressCounter::AddStep(const std::string &name, const int localScope)
{
    ProgressStep* stepParent = (currentStep) ? currentStep->GetParent() : rootStep;
    AddStepAsChild(name, localScope, stepParent);
}

void AbstractProgressCounter::PushStep(const std::string &name, const int localScope)
{
    AddStepAsChild(name, localScope, currentStep);
}

void AbstractProgressCounter::PopStep()
{
    globalProgressCounter = static_cast<float>(currentStep->GetEndingGlobalScope());
    NotifySetStepValue(static_cast<int>(globalProgressCounter));
    currentStep = currentStep->GetParent();
    NotifyStepChange(currentStep->GetName());
}

void AbstractProgressCounter::Increment(const int percentValue)
{
    if (currentStep == nullptr)
        return;

    const float floatingStepScope = static_cast<float>(currentStep->GetGlobalScope());
    const float globalIncrement = (static_cast<float>(percentValue) / 100.f) * floatingStepScope;

    globalProgressCounter += globalIncrement;

    if (globalProgressCounter > currentStep->GetEndingGlobalScope())
        globalProgressCounter = static_cast<float>(currentStep->GetEndingGlobalScope());

    NotifySetStepValue(static_cast<int>(globalProgressCounter));
}

void AbstractProgressCounter::AddStepAsChild(const std::string &name, const int localScope, ProgressStep *parentStep)
{
    ProgressStep* newStep = new ProgressStep(localScope, name);
    parentStep->AddChild(newStep);
    currentStep = newStep;

    globalProgressCounter = static_cast<float>(currentStep->GetStartingGlobalScope());
    NotifySetStepValue(static_cast<int>(globalProgressCounter));
    NotifyStepChange(name);
}

ProgressStep::ProgressStep(const int scope, const std::string &_name)
    : localScope(scope), globalScope(scope),
      startGlobalScope(0), endGlobalScope(scope), name(_name)
{
}

void ProgressStep::AddChild(ProgressStep* step)
{
    steps.push_back(step);
    step->parentStep = this;

    step->ComputeGlobalStartingScope();
    step->ComputeGlobalEndingScope();
}

std::string ProgressStep::GetName() const
{
    return name;
}

int ProgressStep::GetGlobalScope() const
{
    return globalScope;
}

int ProgressStep::GetStartingGlobalScope() const
{
    return startGlobalScope;
}

int ProgressStep::GetEndingGlobalScope() const
{
    return endGlobalScope;
}

ProgressStep *ProgressStep::GetParent()
{
    return parentStep;
}

void ProgressStep::ComputeGlobalStartingScope()
{
    if (parentStep->steps.size() == 1)
        startGlobalScope = parentStep->startGlobalScope;
    else
    {
        std::list<ProgressStep*>::reverse_iterator it=parentStep->steps.rbegin();
        ++it;
        ProgressStep* previousBrother = *it;
        startGlobalScope = previousBrother->endGlobalScope;
    }
}

void ProgressStep::ComputeGlobalEndingScope()
{
    float parentGlobalScope = static_cast<float>(parentStep->globalScope) / 100.f;
    globalScope = static_cast<int>(parentGlobalScope * localScope);
    endGlobalScope = startGlobalScope + globalScope;
}
