#include <algorithm>

#include "constants.hpp"
#include "pathfinder.hpp"
#include "seedfuns.hpp"

void StateSpace::addState(RngState newState)
{
    if (!newState.validState) {
        noInvalidPath = false;
    }
    else {
        int newSeedIndex = newState.seedIndex;
        if (states.find(newSeedIndex) == states.end()) {
            states.insert({newSeedIndex, newState});
        }
        else if (states.at(newSeedIndex).totalTime > newState.totalTime) {
            states.at(newSeedIndex) = newState;
        }
    }
}

const std::map<int, RngState>& StateSpace::currentStates() const
{
    return states;
}

SearchResult StateSpace::fastestPathToDrop(const std::vector<uint32_t>& goodEnds) const
{
    std::vector<Choice> optimalPath;
    bool foundPath = false;
    int optimalTime = 2147483647;
    int optimalEndingIndex = 0;

    for (auto const& pair : states) {
        int finalIndex = pair.first + endOfDuelAdvs;
        uint32_t nextPrn = prn(indexToSeed(static_cast<long long>(finalIndex)));
        nextPrn = nextPrn % totalProb;
        if (std::find(goodEnds.begin(), goodEnds.end(), nextPrn) != goodEnds.end()) {
            if (pair.second.totalTime < optimalTime) {
                foundPath = true;
                optimalTime = pair.second.totalTime;
                optimalPath = pair.second.choices;
                optimalEndingIndex = finalIndex;
            }
        }
    }

    SearchResult fastestPath;
    fastestPath.noMissedPaths = noInvalidPath;
    fastestPath.foundPath = foundPath;
    fastestPath.endingSeedIndex = optimalEndingIndex;

    if (foundPath) {
        fastestPath.optimalPath = optimalPath;
    }

    return fastestPath;
}

RngState PathFinder::animAdvance(RngState state, Anim anim)
{
    if (!state.validState) {
        return state;
    }

    const auto& advData = fmData->getVariableAdvs();
    if (fmData->getFixedAdvs().find(anim) != fmData->getFixedAdvs().end()) {
        state.seedIndex += static_cast<int>(fmData->getFixedAdvs().at(anim));
    }
    else {
        if (static_cast<int>(advData.at(anim).size()) <= state.seedIndex) {
            state.validState = false;
        }
        else {
            uint16_t advSize = advData.at(anim).at(state.seedIndex);
            state.seedIndex += static_cast<int>(advSize);
        }
    }

    return state;
}

RngState PathFinder::choiceAdvance(RngState state, Choice choice)
{
    for (Anim anim : fmData->getDecomps().at(choice)) {
        state = animAdvance(state, anim);
    }

    state.totalTime += fmData->getTimeCosts().at(choice);
    state.choices.push_back(choice);

    return state;
}

StateSpace PathFinder::advanceAcrossChoices(const StateSpace& states,
                                            const std::vector<Choice>& choices)
{
    StateSpace newStateSpace(fmData->getFixedAdvs().at(Anim::endOfDuel));

    for (auto const& pair : states.currentStates()) {
        for (Choice choice : choices) {
            newStateSpace.addState(choiceAdvance(pair.second, choice));
        }
    }

    return newStateSpace;
}

SearchResult PathFinder::findOptimalPath(uint32_t initialSeed,
                                         const std::vector<std::vector<Choice>>& tree,
                                         const std::vector<uint32_t>& goodEnds)
{
    StateSpace states(fmData->getFixedAdvs().at(Anim::endOfDuel));
    states.addState(RngState(initialSeed));

    for (std::vector<Choice> choices : tree) {
        states = advanceAcrossChoices(states, choices);
    }

    return states.fastestPathToDrop(goodEnds);
}
