#ifndef ABSTRACTPROGRESSCOUNTER_H
#define ABSTRACTPROGRESSCOUNTER_H

#include <list>
#include <string>

class ProgressStep;

class AbstractProgressCounter
{
public:
    AbstractProgressCounter();
    virtual ~AbstractProgressCounter() = default;

    void AddStep(const std::string& name, const int localScope);
    void PushStep(const std::string& name, const int localScope);
    void PopStep();

    void Increment(const int percentValue);

protected:
    virtual void NotifyStepChange(const std::string& name) = 0;
    virtual void NotifySetStepValue(const int value) = 0;

private:
    void AddStepAsChild(const std::string &name, const int localScope, ProgressStep* parentStep);
    int GetCurrentStepGlobalScope(void);

    ProgressStep* rootStep;
    ProgressStep* currentStep;
    float globalProgressCounter;
};

class ProgressStep
{
public :
    ProgressStep(const int scope, const std::string& _name);

    void AddChild(ProgressStep* step);

    std::string GetName() const;
    int GetGlobalScope(void) const;
    int GetStartingGlobalScope(void) const;
    int GetEndingGlobalScope(void) const;
    ProgressStep* GetParent();

private :
    void ComputeGlobalStartingScope();
    void ComputeGlobalEndingScope();

    int localScope;
    int globalScope;
    int startGlobalScope;
    int endGlobalScope;
    std::string name;
    std::list<ProgressStep*> steps;
    ProgressStep* parentStep;
};

#endif // ABSTRACTPROGRESSCOUNTER_H
