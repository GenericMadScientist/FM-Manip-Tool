#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include <cstdint>
#include <map>
#include <memory>
#include <vector>

#include "fmdata.hpp"
#include "manipanims.hpp"
#include "seedfuns.hpp"

struct RngState {
    bool validState = true;
    int seedIndex;
    int totalTime = 0;
    std::vector<Choice> choices;

    explicit RngState(uint32_t initialSeed)
        : seedIndex{static_cast<int>(seedToIndex(initialSeed))} {}
};

struct SearchResult {
    bool noMissedPaths = true;
    bool foundPath = false;
    int endingSeedIndex;
    std::vector<Choice> optimalPath;
};

class StateSpace {
public:
    StateSpace(int endAdvs, bool initialBool = true)
        : noInvalidPath{initialBool}, endOfDuelAdvs{endAdvs} {}
    void addState(RngState);
    const std::map<int, RngState>& currentStates() const;
    SearchResult fastestPathToDrop(const std::vector<uint32_t>&) const;
private:
    bool noInvalidPath;
    int endOfDuelAdvs;
    std::map<int, RngState> states;
};

class PathFinder {
public:
    explicit PathFinder(std::shared_ptr<FMData> fmDataPtr)
        : fmData{fmDataPtr} {}
    SearchResult findOptimalPath(uint32_t,
                                 const std::vector<std::vector<Choice>>&,
                                 const std::vector<uint32_t>&);
private:
    const std::shared_ptr<FMData> fmData;

    RngState animAdvance(RngState, Anim);
    RngState choiceAdvance(RngState, Choice);
    StateSpace advanceAcrossChoices(const StateSpace&,
                                    const std::vector<Choice>&);
};

#endif
